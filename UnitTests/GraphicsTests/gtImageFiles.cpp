/*****************************************************************************
**  gtImageFiles.cpp
**
**      See gtImageFiles.hpp. PNG, BMP, GIF, TGA, HDR and legacy DDS files
**      are written byte by byte here, so each case is exactly what its name
**      says; JPEGs and 32-bit BMPs come from WIC (through DirectXTex),
**      TIFFs from libtiff and modern DDS files from DirectXTex.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtImageFiles.hpp"

#include "Graphics/G2d/private/rgbe.h"

#include <DirectXPackedVector.h>
#include <tiffio.h>
#include <zlib.h>
#include <objbase.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <fstream>
#include <memory>
#include <sstream>

namespace
{
	const uint32_t c_W = 37, c_H = 23;		// odd sizes, not powers of two

	//------------------------------------------------------------------------
	//	Content
	//------------------------------------------------------------------------
	uint32_t hash(uint32_t i_X)
	{
		i_X ^= i_X >> 16; i_X *= 0x7feb352d; i_X ^= i_X >> 15; i_X *= 0x846ca68b; i_X ^= i_X >> 16;
		return i_X;
	}

	//	A gradient with some noise; i_bSmooth leaves the noise out (for JPEG)
	uint8_t value(uint32_t i_X, uint32_t i_Y, uint32_t i_C, uint32_t i_Seed, bool i_bSmooth = false)
	{
		const uint32_t base = (i_X * 255 / c_W * (i_C == 0) + i_Y * 255 / c_H * (i_C == 1) +
							   (i_X + i_Y) * 255 / (c_W + c_H) * (i_C == 2) + (200 + i_X) * (i_C == 3)) & 0xFF;
		if (i_bSmooth)
		{
			return (uint8_t)base;
		}
		return (uint8_t)((base + (hash(i_X * 7919 + i_Y * 104729 + i_C * 31 + i_Seed * 977) & 15)) & 0xFF);
	}

	std::vector<uint8_t> random_bytes(size_t i_Size, uint32_t i_Seed)
	{
		std::vector<uint8_t> bytes(i_Size);
		for (size_t i = 0; i < i_Size; i++)
		{
			bytes[i] = (uint8_t)hash((uint32_t)i * 2654435761u + i_Seed);
		}
		return bytes;
	}

	void write_file(const std::filesystem::path& i_Path, const std::vector<uint8_t>& i_Bytes)
	{
		std::ofstream out(i_Path, std::ios::binary);
		out.write((const char*)i_Bytes.data(), (std::streamsize)i_Bytes.size());
	}

	void put16le(std::vector<uint8_t>& o, uint32_t v) { o.push_back((uint8_t)v); o.push_back((uint8_t)(v >> 8)); }
	void put32le(std::vector<uint8_t>& o, uint32_t v) { put16le(o, v & 0xFFFF); put16le(o, v >> 16); }
	void put32be(std::vector<uint8_t>& o, uint32_t v)
	{
		o.push_back((uint8_t)(v >> 24)); o.push_back((uint8_t)(v >> 16)); o.push_back((uint8_t)(v >> 8)); o.push_back((uint8_t)v);
	}

	//------------------------------------------------------------------------
	//	PNG
	//------------------------------------------------------------------------
	void png_chunk(std::vector<uint8_t>& o_Out, const char* i_Type, const std::vector<uint8_t>& i_Data)
	{
		put32be(o_Out, (uint32_t)i_Data.size());
		std::vector<uint8_t> body(i_Type, i_Type + 4);
		body.insert(body.end(), i_Data.begin(), i_Data.end());
		o_Out.insert(o_Out.end(), body.begin(), body.end());
		put32be(o_Out, (uint32_t)crc32(0, body.data(), (uInt)body.size()));
	}

	struct PngOptions
	{
		std::vector<uint8_t> m_Palette;		// RGB triples
		std::vector<uint8_t> m_Trns;
		bool m_bSrgb = false;
		uint32_t m_Gama = 0;
	};

	//	i_Rows: packed rows as PNG stores them (16-bit samples big-endian)
	std::vector<uint8_t> png(uint32_t i_W, uint32_t i_H, int i_ColorType, int i_Depth, const std::vector<uint8_t>& i_Rows,
							 const PngOptions& i_Options = PngOptions())
	{
		std::vector<uint8_t> out = { 0x89, 'P', 'N', 'G', 0x0D, 0x0A, 0x1A, 0x0A };
		std::vector<uint8_t> ihdr;
		put32be(ihdr, i_W);
		put32be(ihdr, i_H);
		ihdr.insert(ihdr.end(), { (uint8_t)i_Depth, (uint8_t)i_ColorType, 0, 0, 0 });
		png_chunk(out, "IHDR", ihdr);
		if (i_Options.m_Gama)
		{
			std::vector<uint8_t> gama;
			put32be(gama, i_Options.m_Gama);
			png_chunk(out, "gAMA", gama);
		}
		if (i_Options.m_bSrgb)
		{
			png_chunk(out, "sRGB", { 0 });
		}
		if (!i_Options.m_Palette.empty())
		{
			png_chunk(out, "PLTE", i_Options.m_Palette);
		}
		if (!i_Options.m_Trns.empty())
		{
			png_chunk(out, "tRNS", i_Options.m_Trns);
		}
		const size_t row = i_Rows.size() / i_H;
		std::vector<uint8_t> filtered;
		for (uint32_t y = 0; y < i_H; y++)
		{
			filtered.push_back(0);
			filtered.insert(filtered.end(), i_Rows.begin() + y * row, i_Rows.begin() + (y + 1) * row);
		}
		uLongf size = compressBound((uLong)filtered.size());
		std::vector<uint8_t> idat(size);
		compress2(idat.data(), &size, filtered.data(), (uLong)filtered.size(), 6);
		idat.resize(size);
		png_chunk(out, "IDAT", idat);
		png_chunk(out, "IEND", {});
		return out;
	}

	//	Rows of i_Channels samples of i_Depth bits (8 or 16)
	std::vector<uint8_t> png_rows(uint32_t i_Channels, int i_Depth, uint32_t i_Seed, const uint32_t* i_pMap = nullptr)
	{
		std::vector<uint8_t> rows;
		for (uint32_t y = 0; y < c_H; y++)
		{
			for (uint32_t x = 0; x < c_W; x++)
			{
				for (uint32_t c = 0; c < i_Channels; c++)
				{
					const uint32_t channel = i_pMap ? i_pMap[c] : c;
					const uint8_t v = value(x, y, channel, i_Seed);
					rows.push_back(v);
					if (i_Depth == 16)
					{
						rows.push_back((uint8_t)hash(x * 13 + y * 17 + c + i_Seed));
					}
				}
			}
		}
		return rows;
	}

	//	A 256-colour palette and indices into it
	std::vector<uint8_t> palette_rgb(uint32_t i_Seed)
	{
		std::vector<uint8_t> palette(256 * 3);
		for (uint32_t i = 0; i < 256; i++)
		{
			palette[i * 3 + 0] = (uint8_t)i;
			palette[i * 3 + 1] = (uint8_t)(255 - i);
			palette[i * 3 + 2] = (uint8_t)hash(i + i_Seed);
		}
		return palette;
	}

	uint8_t index_at(uint32_t i_X, uint32_t i_Y)
	{
		return (uint8_t)((i_X * 7 + i_Y * 3 + (hash(i_X + i_Y * 64) & 3)) & 0xFF);
	}

	//------------------------------------------------------------------------
	//	BMP (bottom-up, rows padded to 4 bytes)
	//------------------------------------------------------------------------
	std::vector<uint8_t> bmp(uint32_t i_Bits, uint32_t i_Seed)
	{
		const uint32_t row = ((c_W * i_Bits / 8) + 3) & ~3u;
		const uint32_t palette = (i_Bits == 8) ? 256 * 4 : 0;
		const uint32_t offset = 14 + 40 + palette;
		std::vector<uint8_t> out = { 'B', 'M' };
		put32le(out, offset + row * c_H);
		put32le(out, 0);
		put32le(out, offset);
		put32le(out, 40);
		put32le(out, c_W);
		put32le(out, c_H);
		put16le(out, 1);
		put16le(out, i_Bits);
		put32le(out, 0);			// BI_RGB
		put32le(out, row * c_H);
		put32le(out, 2835);
		put32le(out, 2835);
		put32le(out, i_Bits == 8 ? 256 : 0);
		put32le(out, 0);
		if (i_Bits == 8)
		{
			const std::vector<uint8_t> rgb = palette_rgb(i_Seed);
			for (uint32_t i = 0; i < 256; i++)
			{
				out.insert(out.end(), { rgb[i * 3 + 2], rgb[i * 3 + 1], rgb[i * 3 + 0], 0 });
			}
		}
		for (uint32_t y = c_H; y-- > 0;)
		{
			const size_t start = out.size();
			for (uint32_t x = 0; x < c_W; x++)
			{
				if (i_Bits == 8)
				{
					out.push_back(index_at(x, y));
				}
				else
				{
					out.insert(out.end(), { value(x, y, 2, i_Seed), value(x, y, 1, i_Seed), value(x, y, 0, i_Seed) });
				}
			}
			out.resize(start + row, 0);
		}
		return out;
	}

	//------------------------------------------------------------------------
	//	GIF: 256-colour global palette; the LZW stream sends a clear code
	//	every 254 indices, so codes always stay 9 bits
	//------------------------------------------------------------------------
	std::vector<uint8_t> gif(uint32_t i_Seed)
	{
		std::vector<uint8_t> out = { 'G', 'I', 'F', '8', '9', 'a' };
		put16le(out, c_W);
		put16le(out, c_H);
		out.insert(out.end(), { 0xF7, 0, 0 });		// global colour table, 256 entries
		const std::vector<uint8_t> rgb = palette_rgb(i_Seed);
		out.insert(out.end(), rgb.begin(), rgb.end());
		out.push_back(0x2C);
		put16le(out, 0);
		put16le(out, 0);
		put16le(out, c_W);
		put16le(out, c_H);
		out.push_back(0);
		out.push_back(8);							// LZW minimum code size

		std::vector<uint8_t> codes;
		uint32_t bits = 0, nbits = 0;
		auto emit = [&](uint32_t i_Code)
		{
			bits |= i_Code << nbits;
			nbits += 9;
			while (nbits >= 8)
			{
				codes.push_back((uint8_t)bits);
				bits >>= 8;
				nbits -= 8;
			}
		};
		uint32_t run = 0;
		emit(256);
		for (uint32_t y = 0; y < c_H; y++)
		{
			for (uint32_t x = 0; x < c_W; x++)
			{
				if (run == 254)
				{
					emit(256);
					run = 0;
				}
				emit(index_at(x, y));
				run++;
			}
		}
		emit(257);
		if (nbits)
		{
			codes.push_back((uint8_t)bits);
		}
		for (size_t i = 0; i < codes.size(); i += 255)
		{
			const size_t n = std::min<size_t>(255, codes.size() - i);
			out.push_back((uint8_t)n);
			out.insert(out.end(), codes.begin() + i, codes.begin() + i + n);
		}
		out.push_back(0);
		out.push_back(0x3B);
		return out;
	}

	//------------------------------------------------------------------------
	//	TGA
	//------------------------------------------------------------------------
	struct TgaOptions
	{
		int m_Bits = 32;			// 8 (grey), 24, 32
		bool m_bRle = false;
		bool m_bTopDown = true;
		bool m_bZeroAlpha = false;
	};

	std::vector<uint8_t> tga(const TgaOptions& i_Options, uint32_t i_Seed)
	{
		const int bpp = i_Options.m_Bits / 8;
		std::vector<uint8_t> out(18, 0);
		out[2] = (uint8_t)((i_Options.m_Bits == 8 ? 3 : 2) + (i_Options.m_bRle ? 8 : 0));
		out[12] = (uint8_t)c_W; out[13] = (uint8_t)(c_W >> 8);
		out[14] = (uint8_t)c_H; out[15] = (uint8_t)(c_H >> 8);
		out[16] = (uint8_t)i_Options.m_Bits;
		out[17] = (uint8_t)((i_Options.m_Bits == 32 ? 8 : 0) | (i_Options.m_bTopDown ? 0x20 : 0));
		for (uint32_t row = 0; row < c_H; row++)
		{
			const uint32_t y = i_Options.m_bTopDown ? row : c_H - 1 - row;
			std::vector<uint8_t> pixels;
			for (uint32_t x = 0; x < c_W; x++)
			{
				//	Runs of equal pixels every 8 columns, so RLE packets of both kinds appear
				const uint32_t sx = (x % 8 < 4) ? x - x % 8 : x;
				if (bpp == 1)
				{
					pixels.push_back(value(sx, y, 0, i_Seed));
				}
				else
				{
					pixels.insert(pixels.end(), { value(sx, y, 2, i_Seed), value(sx, y, 1, i_Seed), value(sx, y, 0, i_Seed) });
					if (bpp == 4)
					{
						pixels.push_back(i_Options.m_bZeroAlpha ? 0 : value(sx, y, 3, i_Seed));
					}
				}
			}
			if (!i_Options.m_bRle)
			{
				out.insert(out.end(), pixels.begin(), pixels.end());
				continue;
			}
			uint32_t x = 0;
			while (x < c_W)
			{
				uint32_t n = 1;
				while (x + n < c_W && n < 128 && !memcmp(&pixels[(x + n) * bpp], &pixels[x * bpp], bpp))
				{
					n++;
				}
				if (n > 1)
				{
					out.push_back((uint8_t)(0x80 | (n - 1)));
					out.insert(out.end(), pixels.begin() + x * bpp, pixels.begin() + (x + 1) * bpp);
				}
				else
				{
					while (x + n < c_W && n < 128 && memcmp(&pixels[(x + n) * bpp], &pixels[(x + n - 1) * bpp], bpp))
					{
						n++;
					}
					out.push_back((uint8_t)(n - 1));
					out.insert(out.end(), pixels.begin() + x * bpp, pixels.begin() + (x + n) * bpp);
				}
				x += n;
			}
		}
		return out;
	}

	//------------------------------------------------------------------------
	//	TIFF (libtiff)
	//------------------------------------------------------------------------
	bool write_tiff(const std::filesystem::path& i_Path, uint16_t i_Samples, uint16_t i_Bits, uint16_t i_Format, uint32_t i_Seed)
	{
		TIFF* tif = TIFFOpenW(i_Path.wstring().c_str(), "w");
		if (!tif)
		{
			return false;
		}
		TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, c_W);
		TIFFSetField(tif, TIFFTAG_IMAGELENGTH, c_H);
		TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, i_Samples);
		TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, i_Bits);
		TIFFSetField(tif, TIFFTAG_SAMPLEFORMAT, i_Format);
		TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
		TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, i_Samples >= 3 ? PHOTOMETRIC_RGB : PHOTOMETRIC_MINISBLACK);
		if (i_Samples == 4)
		{
			uint16_t extra = EXTRASAMPLE_UNASSALPHA;
			TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extra);
		}
		TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
		TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, 8);
		std::vector<uint8_t> row(c_W * i_Samples * (i_Bits / 8));
		bool ok = true;
		for (uint32_t y = 0; y < c_H && ok; y++)
		{
			for (uint32_t x = 0; x < c_W; x++)
			{
				for (uint32_t c = 0; c < i_Samples; c++)
				{
					const uint8_t v = value(x, y, c, i_Seed);
					uint8_t* p = &row[(x * i_Samples + c) * (i_Bits / 8)];
					if (i_Bits == 8)
					{
						*p = v;
					}
					else if (i_Bits == 16)
					{
						const uint16_t s = (uint16_t)(v * 257 + (hash(x + y * 64 + c) & 0xFF) - 128 + 128);
						memcpy(p, &s, 2);
					}
					else
					{
						const float f = v / 64.0f - 0.5f;
						memcpy(p, &f, 4);
					}
				}
			}
			ok = TIFFWriteScanline(tif, row.data(), y, 0) == 1;
		}
		TIFFClose(tif);
		return ok;
	}

	//------------------------------------------------------------------------
	//	HDR (Radiance RGBE, run-length encoded scanlines)
	//------------------------------------------------------------------------
	//	i_Program: the header's "#?" program type (RGBE_WriteHeader writes
	//	"RGBE" without one; GraphicsDX11's texture manager only takes files
	//	that start "#?RADIANCE")
	std::vector<uint8_t> hdr(uint32_t i_Seed, const char* i_Program = NULL, uint32_t i_W = c_W, uint32_t i_H = c_H)
	{
		std::vector<float> rgb(i_W * i_H * 3);
		for (uint32_t y = 0; y < i_H; y++)
		{
			for (uint32_t x = 0; x < i_W; x++)
			{
				for (uint32_t c = 0; c < 3; c++)
				{
					//	A wide range, with runs (every 4th column repeats) for the RLE
					const uint32_t sx = x - x % 4 * (x % 4 < 2);
					rgb[(y * i_W + x) * 3 + c] = value(sx, y, c, i_Seed) / 32.0f * (1 + (y % 5));
				}
			}
		}
		std::ostringstream os(std::ios::binary);
		rgbe_header_info info = {};
		if (i_Program)
		{
			info.valid = RGBE_VALID_PROGRAMTYPE;
			strncpy(info.programtype, i_Program, sizeof(info.programtype) - 1);
		}
		RGBE_WriteHeader(os, i_W, i_H, &info);
		RGBE_WritePixels_RLE(os, rgb.data(), i_W, i_H);
		const std::string s = os.str();
		return std::vector<uint8_t>(s.begin(), s.end());
	}

	//------------------------------------------------------------------------
	//	DDS: legacy (D3D9) headers written here; DX10 ones by DirectXTex
	//------------------------------------------------------------------------
	const uint32_t DDS_FOURCC = 0x4, DDS_RGB = 0x40, DDS_RGBA = 0x41, DDS_LUMINANCE = 0x20000, DDS_LUMINANCEA = 0x20001,
				   DDS_ALPHA = 0x2, DDS_PAL8 = 0x20, DDS_PAL8A = 0x21, DDS_BUMPDUDV = 0x80000;

	constexpr uint32_t fourcc(char a, char b, char c, char d)
	{
		return (uint32_t)(uint8_t)a | ((uint32_t)(uint8_t)b << 8) | ((uint32_t)(uint8_t)c << 16) | ((uint32_t)(uint8_t)d << 24);
	}

	struct LegacyFormat
	{
		const char* m_Name;
		uint32_t m_Flags, m_FourCC, m_Bits, m_R, m_G, m_B, m_A;
		uint32_t m_BlockBytes;		// for FourCC formats: bytes per pixel (0 = 4x4 blocks of 16)
	};

	const LegacyFormat c_Legacy[] =
	{
		{ "r8g8b8",			DDS_RGB, 0, 24, 0xff0000, 0x00ff00, 0x0000ff, 0, 0 },
		{ "a8r8g8b8",		DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000, 0 },
		{ "x8r8g8b8",		DDS_RGB, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0, 0 },
		{ "a8b8g8r8",		DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000, 0 },
		{ "x8b8g8r8",		DDS_RGB, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0, 0 },
		{ "g16r16",			DDS_RGB, 0, 32, 0x0000ffff, 0xffff0000, 0, 0, 0 },
		{ "r5g6b5",			DDS_RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0, 0 },
		{ "a1r5g5b5",		DDS_RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000, 0 },
		{ "x1r5g5b5",		DDS_RGB, 0, 16, 0x7c00, 0x03e0, 0x001f, 0, 0 },
		{ "a4r4g4b4",		DDS_RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000, 0 },
		{ "x4r4g4b4",		DDS_RGB, 0, 16, 0x0f00, 0x00f0, 0x000f, 0, 0 },
		{ "a8r3g3b2",		DDS_RGBA, 0, 16, 0x00e0, 0x001c, 0x0003, 0xff00, 0 },
		{ "r3g3b2",			DDS_RGB, 0, 8, 0xe0, 0x1c, 0x03, 0, 0 },
		{ "a4l4",			DDS_LUMINANCEA, 0, 8, 0x0f, 0, 0, 0xf0, 0 },
		{ "l8",				DDS_LUMINANCE, 0, 8, 0xff, 0, 0, 0, 0 },
		{ "l16",			DDS_LUMINANCE, 0, 16, 0xffff, 0, 0, 0, 0 },
		{ "a8l8",			DDS_LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00, 0 },
		{ "a8",				DDS_ALPHA, 0, 8, 0, 0, 0, 0xff, 0 },
		{ "a2r10g10b10",	DDS_RGBA, 0, 32, 0x000003ff, 0x000ffc00, 0x3ff00000, 0xc0000000, 0 },
		{ "a2b10g10r10",	DDS_RGBA, 0, 32, 0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000, 0 },
		{ "r32f_rgb",		DDS_RGB, 0, 32, 0xffffffff, 0, 0, 0, 0 },
		{ "p8",				DDS_PAL8, 0, 8, 0, 0, 0, 0, 0 },
		{ "a8p8",			DDS_PAL8A, 0, 16, 0, 0, 0, 0xff00, 0 },
		{ "v8u8",			DDS_BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0, 0, 0 },
		{ "q8w8v8u8",		DDS_BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000, 0 },
		{ "v16u16",			DDS_BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0, 0, 0 },
		{ "dxt1",			DDS_FOURCC, fourcc('D', 'X', 'T', '1'), 0, 0, 0, 0, 0, 0 },
		{ "dxt2",			DDS_FOURCC, fourcc('D', 'X', 'T', '2'), 0, 0, 0, 0, 0, 0 },
		{ "dxt3",			DDS_FOURCC, fourcc('D', 'X', 'T', '3'), 0, 0, 0, 0, 0, 0 },
		{ "dxt4",			DDS_FOURCC, fourcc('D', 'X', 'T', '4'), 0, 0, 0, 0, 0, 0 },
		{ "dxt5",			DDS_FOURCC, fourcc('D', 'X', 'T', '5'), 0, 0, 0, 0, 0, 0 },
		{ "ati1",			DDS_FOURCC, fourcc('A', 'T', 'I', '1'), 0, 0, 0, 0, 0, 0 },
		{ "ati2",			DDS_FOURCC, fourcc('A', 'T', 'I', '2'), 0, 0, 0, 0, 0, 0 },
		{ "bc4u",			DDS_FOURCC, fourcc('B', 'C', '4', 'U'), 0, 0, 0, 0, 0, 0 },
		{ "bc5s",			DDS_FOURCC, fourcc('B', 'C', '5', 'S'), 0, 0, 0, 0, 0, 0 },
		{ "a16b16g16r16",	DDS_FOURCC, 36, 0, 0, 0, 0, 0, 8 },
		{ "q16w16v16u16",	DDS_FOURCC, 110, 0, 0, 0, 0, 0, 8 },
		{ "r16f",			DDS_FOURCC, 111, 0, 0, 0, 0, 0, 2 },
		{ "g16r16f",		DDS_FOURCC, 112, 0, 0, 0, 0, 0, 4 },
		{ "a16b16g16r16f",	DDS_FOURCC, 113, 0, 0, 0, 0, 0, 8 },
		{ "r32f",			DDS_FOURCC, 114, 0, 0, 0, 0, 0, 4 },
		{ "g32r32f",		DDS_FOURCC, 115, 0, 0, 0, 0, 0, 8 },
		{ "a32b32g32r32f",	DDS_FOURCC, 116, 0, 0, 0, 0, 0, 16 },
		{ "cxv8u8",			DDS_FOURCC, 117, 0, 0, 0, 0, 0, 2 },
	};

	bool is_bc(const LegacyFormat& i_F)
	{
		return i_F.m_Flags == DDS_FOURCC && i_F.m_BlockBytes == 0;
	}

	//	A legacy DDS of c_W x c_H (BC: 20 x 12) with a full mip chain
	std::vector<uint8_t> legacy_dds(const LegacyFormat& i_F, uint32_t i_Seed)
	{
		const bool bc = is_bc(i_F);
		const uint32_t w = bc ? 20 : c_W, h = bc ? 12 : c_H;
		uint32_t mips = 1;
		while ((w >> mips) || (h >> mips))
		{
			mips++;
		}
		std::vector<uint8_t> out;
		put32le(out, 0x20534444);			// "DDS "
		put32le(out, 124);
		put32le(out, 0x00001007 | 0x00020000);
		put32le(out, h);
		put32le(out, w);
		put32le(out, 0);
		put32le(out, 0);
		put32le(out, mips);
		for (int i = 0; i < 11; i++) put32le(out, 0);
		put32le(out, 32);
		put32le(out, i_F.m_Flags);
		put32le(out, i_F.m_FourCC);
		put32le(out, i_F.m_Bits);
		put32le(out, i_F.m_R);
		put32le(out, i_F.m_G);
		put32le(out, i_F.m_B);
		put32le(out, i_F.m_A);
		put32le(out, 0x00001000 | 0x00400008);
		for (int i = 0; i < 4; i++) put32le(out, 0);
		if (i_F.m_Flags & DDS_PAL8)
		{
			const std::vector<uint8_t> rgb = palette_rgb(i_Seed);
			for (uint32_t i = 0; i < 256; i++)
			{
				out.insert(out.end(), { rgb[i * 3], rgb[i * 3 + 1], rgb[i * 3 + 2], (uint8_t)(255 - i / 2) });
			}
		}
		size_t data = 0;
		for (uint32_t m = 0; m < mips; m++)
		{
			const uint32_t mw = std::max(1u, w >> m), mh = std::max(1u, h >> m);
			if (bc)
			{
				const uint32_t block = (i_F.m_FourCC == fourcc('D', 'X', 'T', '1') || i_F.m_FourCC == fourcc('A', 'T', 'I', '1') ||
										i_F.m_FourCC == fourcc('B', 'C', '4', 'U')) ? 8 : 16;
				data += ((mw + 3) / 4) * ((mh + 3) / 4) * block;
			}
			else
			{
				const uint32_t bytes = i_F.m_BlockBytes ? i_F.m_BlockBytes : i_F.m_Bits / 8;
				data += (size_t)mw * mh * bytes;
			}
		}
		std::vector<uint8_t> payload = random_bytes(data, i_Seed);
		if (i_F.m_FourCC >= 111 && i_F.m_FourCC <= 116 && i_F.m_FourCC != 112 && i_F.m_FourCC != 111 && i_F.m_FourCC != 113)
		{
			//	32-bit floats: keep them finite and in a sane range
			for (size_t i = 0; i + 4 <= payload.size(); i += 4)
			{
				const float f = (payload[i] / 255.0f - 0.25f) * 8;
				memcpy(&payload[i], &f, 4);
			}
		}
		if (i_F.m_FourCC == 111 || i_F.m_FourCC == 112 || i_F.m_FourCC == 113)
		{
			for (size_t i = 0; i + 2 <= payload.size(); i += 2)
			{
				const DirectX::PackedVector::HALF half = DirectX::PackedVector::XMConvertFloatToHalf((payload[i] / 255.0f - 0.25f) * 8);
				memcpy(&payload[i], &half, 2);
			}
		}
		out.insert(out.end(), payload.begin(), payload.end());
		return out;
	}

	//	A DirectXTex image of i_Format filled with valid values
	bool fill_image(DirectX::ScratchImage& io_Image, uint32_t i_Seed)
	{
		const DXGI_FORMAT format = io_Image.GetMetadata().format;
		uint8_t* pixels = io_Image.GetPixels();
		const size_t size = io_Image.GetPixelsSize();
		const std::vector<uint8_t> bytes = random_bytes(size, i_Seed);
		memcpy(pixels, bytes.data(), size);
		if (format == DXGI_FORMAT_R32G32B32A32_FLOAT || format == DXGI_FORMAT_R32_FLOAT)
		{
			for (size_t i = 0; i + 4 <= size; i += 4)
			{
				const float f = (bytes[i] / 255.0f - 0.25f) * 8;
				memcpy(pixels + i, &f, 4);
			}
		}
		else if (format == DXGI_FORMAT_R16G16B16A16_FLOAT || format == DXGI_FORMAT_R16_FLOAT)
		{
			for (size_t i = 0; i + 2 <= size; i += 2)
			{
				const DirectX::PackedVector::HALF half = DirectX::PackedVector::XMConvertFloatToHalf((bytes[i] / 255.0f - 0.25f) * 8);
				memcpy(pixels + i, &half, 2);
			}
		}
		return true;
	}

	struct DdsCase
	{
		const char* m_Name;
		DXGI_FORMAT m_Format;
		uint32_t m_W, m_H, m_Depth, m_Array, m_Mips;
		bool m_bCube;
		bool m_bForceDx10;
	};

	const DdsCase c_Dds[] =
	{
		{ "dds_rgba8_mips",		DXGI_FORMAT_R8G8B8A8_UNORM,		 37, 23, 1, 1, 0, false, false },
		{ "dds_rgba8_dx10",		DXGI_FORMAT_R8G8B8A8_UNORM,		 37, 23, 1, 1, 1, false, true },
		{ "dds_rgba8_srgb",		DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, 37, 23, 1, 1, 3, false, false },
		{ "dds_bgra8",			DXGI_FORMAT_B8G8R8A8_UNORM,		 37, 23, 1, 1, 2, false, false },
		{ "dds_bgrx8",			DXGI_FORMAT_B8G8R8X8_UNORM,		 37, 23, 1, 1, 2, false, true },
		{ "dds_r8",				DXGI_FORMAT_R8_UNORM,			 37, 23, 1, 1, 2, false, true },
		{ "dds_rg8",			DXGI_FORMAT_R8G8_UNORM,			 37, 23, 1, 1, 2, false, true },
		{ "dds_r16f",			DXGI_FORMAT_R16_FLOAT,			 37, 23, 1, 1, 2, false, true },
		{ "dds_rgba16f",		DXGI_FORMAT_R16G16B16A16_FLOAT,	 37, 23, 1, 1, 2, false, false },
		{ "dds_rgba16",			DXGI_FORMAT_R16G16B16A16_UNORM,	 37, 23, 1, 1, 2, false, true },
		{ "dds_r32f",			DXGI_FORMAT_R32_FLOAT,			 37, 23, 1, 1, 2, false, true },
		{ "dds_rgba32f",		DXGI_FORMAT_R32G32B32A32_FLOAT,	 37, 23, 1, 1, 2, false, false },
		{ "dds_rgb10a2",		DXGI_FORMAT_R10G10B10A2_UNORM,	 37, 23, 1, 1, 2, false, true },
		{ "dds_r11g11b10",		DXGI_FORMAT_R11G11B10_FLOAT,	 37, 23, 1, 1, 2, false, true },
		{ "dds_bc1",			DXGI_FORMAT_BC1_UNORM,			 20, 12, 1, 1, 0, false, false },
		{ "dds_bc1_srgb",		DXGI_FORMAT_BC1_UNORM_SRGB,		 20, 12, 1, 1, 0, false, true },
		{ "dds_bc2",			DXGI_FORMAT_BC2_UNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc2_srgb",		DXGI_FORMAT_BC2_UNORM_SRGB,		 20, 12, 1, 1, 0, false, true },
		{ "dds_bc3",			DXGI_FORMAT_BC3_UNORM,			 20, 12, 1, 1, 0, false, false },
		{ "dds_bc3_srgb",		DXGI_FORMAT_BC3_UNORM_SRGB,		 20, 12, 1, 1, 0, false, true },
		{ "dds_bc4",			DXGI_FORMAT_BC4_UNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc4_snorm",		DXGI_FORMAT_BC4_SNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc5",			DXGI_FORMAT_BC5_UNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc5_snorm",		DXGI_FORMAT_BC5_SNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc6h_uf16",		DXGI_FORMAT_BC6H_UF16,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc6h_sf16",		DXGI_FORMAT_BC6H_SF16,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc7",			DXGI_FORMAT_BC7_UNORM,			 20, 12, 1, 1, 0, false, true },
		{ "dds_bc7_srgb",		DXGI_FORMAT_BC7_UNORM_SRGB,		 20, 12, 1, 1, 0, false, true },
		{ "dds_cube_rgba8",		DXGI_FORMAT_R8G8B8A8_UNORM,		 8, 8, 1, 6, 4, true, false },
		{ "dds_cube_bc1",		DXGI_FORMAT_BC1_UNORM,			 16, 16, 1, 6, 0, true, false },
		{ "dds_cube_rgba16f",	DXGI_FORMAT_R16G16B16A16_FLOAT,	 8, 8, 1, 6, 1, true, true },
		{ "dds_volume_rgba8",	DXGI_FORMAT_R8G8B8A8_UNORM,		 8, 8, 4, 1, 0, false, false },
		{ "dds_volume_r16f",	DXGI_FORMAT_R16_FLOAT,			 8, 4, 6, 1, 2, false, true },
		{ "dds_array_rgba8",	DXGI_FORMAT_R8G8B8A8_UNORM,		 8, 8, 1, 3, 0, false, true },
	};

	bool dds(const DdsCase& i_Case, const std::filesystem::path& i_Path, uint32_t i_Seed)
	{
		DirectX::ScratchImage image;
		uint32_t mips = i_Case.m_Mips;
		if (mips == 0)
		{
			mips = 1;
			while ((i_Case.m_W >> mips) || (i_Case.m_H >> mips) || (i_Case.m_Depth >> mips))
			{
				mips++;
			}
		}
		HRESULT hr;
		if (i_Case.m_Depth > 1)
		{
			hr = image.Initialize3D(i_Case.m_Format, i_Case.m_W, i_Case.m_H, i_Case.m_Depth, mips);
		}
		else if (i_Case.m_bCube)
		{
			hr = image.InitializeCube(i_Case.m_Format, i_Case.m_W, i_Case.m_H, 1, mips);
		}
		else
		{
			hr = image.Initialize2D(i_Case.m_Format, i_Case.m_W, i_Case.m_H, i_Case.m_Array, mips);
		}
		if (FAILED(hr) || !fill_image(image, i_Seed))
		{
			return false;
		}
		const DirectX::DDS_FLAGS flags = i_Case.m_bForceDx10 ? DirectX::DDS_FLAGS_FORCE_DX10_EXT : DirectX::DDS_FLAGS_NONE;
		return SUCCEEDED(DirectX::SaveToDDSFile(image.GetImages(), image.GetImageCount(), image.GetMetadata(), flags,
												i_Path.wstring().c_str()));
	}

	//	An image saved with WIC (through DirectXTex)
	bool wic(const std::filesystem::path& i_Path, DirectX::WICCodecs i_Codec, DXGI_FORMAT i_Format, uint32_t i_Seed)
	{
		DirectX::ScratchImage image;
		if (FAILED(image.Initialize2D(i_Format, c_W, c_H, 1, 1)))
		{
			return false;
		}
		const DirectX::Image* img = image.GetImage(0, 0, 0);
		const size_t bpp = DirectX::BitsPerPixel(i_Format) / 8;
		for (uint32_t y = 0; y < c_H; y++)
		{
			for (uint32_t x = 0; x < c_W; x++)
			{
				for (size_t c = 0; c < bpp; c++)
				{
					//	smooth content: JPEG's lossy steps stay small
					img->pixels[y * img->rowPitch + x * bpp + c] = value(x, y, (uint32_t)c, i_Seed, i_Codec == DirectX::WIC_CODEC_JPEG);
				}
			}
		}
		return SUCCEEDED(DirectX::SaveToWICFile(*img, DirectX::WIC_FLAGS_NONE, DirectX::GetWICCodec(i_Codec), i_Path.wstring().c_str()));
	}
}

//----------------------------------------------------------------------------
std::vector<gtImageFile> gtWriteImageFiles(const std::filesystem::path& i_Dir)
{
	std::filesystem::create_directories(i_Dir);
	std::vector<gtImageFile> files;
	uint32_t seed = 1;
	auto add = [&](const std::string& i_Name, const char* i_Ext, gtImageFile::Kind i_Kind, gtImageFile::Reference i_Ref,
				   int i_Tolerance = 0, bool i_bLegacy = false) -> const std::filesystem::path&
	{
		gtImageFile f;
		f.m_Name = i_Name;
		f.m_Path = i_Dir / (i_Name + i_Ext);
		f.m_Kind = i_Kind;
		f.m_Reference = i_Ref;
		f.m_Tolerance = i_Tolerance;
		f.m_bLegacyDds = i_bLegacy;
		files.push_back(f);
		seed++;
		return files.back().m_Path;
	};

	//	PNG
	{
		const uint32_t rgba[] = { 0, 1, 2, 3 }, grey_alpha[] = { 0, 3 };
		write_file(add("png_rgba8", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 6, 8, png_rows(4, 8, seed)));
		write_file(add("png_rgb8", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 2, 8, png_rows(3, 8, seed)));
		write_file(add("png_grey8", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 0, 8, png_rows(1, 8, seed)));
		write_file(add("png_grey16", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 0, 16, png_rows(1, 16, seed)));
		write_file(add("png_greyalpha8", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 4, 8, png_rows(2, 8, seed, grey_alpha)));
		write_file(add("png_greyalpha16", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 4, 16, png_rows(2, 16, seed, grey_alpha)));
		write_file(add("png_rgba16", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 6, 16, png_rows(4, 16, seed, rgba)));
		write_file(add("png_rgb16", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 2, 16, png_rows(3, 16, seed)));
		PngOptions srgb;
		srgb.m_bSrgb = true;
		write_file(add("png_srgb_chunk", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 6, 8, png_rows(4, 8, seed), srgb));
		PngOptions gama22;
		gama22.m_Gama = 45455;
		write_file(add("png_gama22", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 2, 8, png_rows(3, 8, seed), gama22));
		PngOptions gama10;
		gama10.m_Gama = 100000;
		write_file(add("png_gama10", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 6, 8, png_rows(4, 8, seed), gama10));
		PngOptions grey_srgb;
		grey_srgb.m_bSrgb = true;
		write_file(add("png_grey8_srgb", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 0, 8, png_rows(1, 8, seed), grey_srgb));

		//	Palettized: 8-bit with transparency, 4-bit without
		std::vector<uint8_t> indices8;
		std::vector<uint8_t> indices4;
		for (uint32_t y = 0; y < c_H; y++)
		{
			for (uint32_t x = 0; x < c_W; x++)
			{
				indices8.push_back(index_at(x, y));
			}
			for (uint32_t x = 0; x < c_W; x += 2)
			{
				indices4.push_back((uint8_t)(((index_at(x, y) & 15) << 4) | (x + 1 < c_W ? index_at(x + 1, y) & 15 : 0)));
			}
		}
		PngOptions pal8;
		pal8.m_Palette = palette_rgb(seed);
		for (uint32_t i = 0; i < 200; i++)
		{
			pal8.m_Trns.push_back((uint8_t)(255 - i));
		}
		write_file(add("png_pal8_trns", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 3, 8, indices8, pal8));
		PngOptions pal4;
		pal4.m_Palette = palette_rgb(seed);
		pal4.m_Palette.resize(16 * 3);
		write_file(add("png_pal4", ".png", gtImageFile::e_PNG, gtImageFile::e_RefWIC), png(c_W, c_H, 3, 4, indices4, pal4));
	}

	//	JPEG (WIC's encoder): colour, grey, and colour tagged sRGB in EXIF
	wic(add("jpg_rgb", ".jpg", gtImageFile::e_JPG, gtImageFile::e_RefWIC, 2), DirectX::WIC_CODEC_JPEG, DXGI_FORMAT_R8G8B8A8_UNORM, seed);
	wic(add("jpg_grey", ".jpg", gtImageFile::e_JPG, gtImageFile::e_RefWIC, 2), DirectX::WIC_CODEC_JPEG, DXGI_FORMAT_R8_UNORM, seed);
	wic(add("jpg_srgb", ".jpg", gtImageFile::e_JPG, gtImageFile::e_RefWIC, 2), DirectX::WIC_CODEC_JPEG, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, seed);

	//	BMP
	write_file(add("bmp_rgb24", ".bmp", gtImageFile::e_BMP, gtImageFile::e_RefWIC), bmp(24, seed));
	write_file(add("bmp_pal8", ".bmp", gtImageFile::e_BMP, gtImageFile::e_RefWIC), bmp(8, seed));
	wic(add("bmp_bgra32", ".bmp", gtImageFile::e_BMP, gtImageFile::e_RefWIC), DirectX::WIC_CODEC_BMP, DXGI_FORMAT_B8G8R8A8_UNORM, seed);

	//	GIF
	write_file(add("gif_pal8", ".gif", gtImageFile::e_GIF, gtImageFile::e_RefWIC), gif(seed));

	//	TGA
	{
		TgaOptions o;
		write_file(add("tga_bgra32", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o.m_Bits = 24;
		write_file(add("tga_bgr24", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o.m_bTopDown = false;
		write_file(add("tga_bgr24_bottomup", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o.m_Bits = 8;
		write_file(add("tga_grey8", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o = TgaOptions();
		o.m_bRle = true;
		write_file(add("tga_rle32", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o.m_Bits = 24;
		o.m_bTopDown = false;
		write_file(add("tga_rle24_bottomup", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
		o = TgaOptions();
		o.m_bZeroAlpha = true;
		write_file(add("tga_zero_alpha32", ".tga", gtImageFile::e_TGA, gtImageFile::e_RefTGA), tga(o, seed));
	}

	//	TIFF
	write_tiff(add("tif_rgba8", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 4, 8, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_rgb8", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 3, 8, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_grey8", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 1, 8, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_rgba16", ".tiff", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 4, 16, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_rgb16", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 3, 16, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_grey16", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 1, 16, SAMPLEFORMAT_UINT, seed);
	write_tiff(add("tif_rgbaf32", ".tif", gtImageFile::e_TIFF, gtImageFile::e_RefWIC), 4, 32, SAMPLEFORMAT_IEEEFP, seed);

	//	HDR
	write_file(add("hdr_rle", ".hdr", gtImageFile::e_HDR, gtImageFile::e_RefHDR), hdr(seed));
	write_file(add("hdr_radiance", ".hdr", gtImageFile::e_HDR, gtImageFile::e_RefHDR), hdr(seed, "RADIANCE", 32, 16));		// a power of two: DX11 makes its mips with DirectXTex's box filter

	//	DDS
	for (const DdsCase& c : c_Dds)
	{
		dds(c, add(c.m_Name, ".dds", gtImageFile::e_DDS, gtImageFile::e_RefDDS), seed);
	}
	for (const LegacyFormat& f : c_Legacy)
	{
		const gtImageFile::Reference ref = (f.m_FourCC == 117) ? gtImageFile::e_RefCxV8U8 : gtImageFile::e_RefDDS;
		write_file(add(std::string("dds_legacy_") + f.m_Name, ".dds", gtImageFile::e_DDS, ref, 0, true), legacy_dds(f, seed));
	}
	return files;
}

//----------------------------------------------------------------------------
namespace
{
	//	GraphicsDX11's CxV8U8 conversion (matTextureLoaderDDSWin.cpp,
	//	CreateTextureFromDDS), applied to a file's mip chain
	bool load_cxv8u8(const std::filesystem::path& i_Path, DirectX::ScratchImage& o_Image)
	{
		std::ifstream in(i_Path, std::ios::binary);
		std::vector<uint8_t> file((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		if (file.size() < 128)
		{
			return false;
		}
		uint32_t h, w, mips;
		memcpy(&h, &file[12], 4);
		memcpy(&w, &file[16], 4);
		memcpy(&mips, &file[28], 4);
		mips = std::max(1u, mips);
		if (FAILED(o_Image.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, w, h, 1, mips)))
		{
			return false;
		}
		const char* src = (const char*)&file[128];
		for (uint32_t m = 0; m < mips; m++)
		{
			const DirectX::Image* img = o_Image.GetImage(m, 0, 0);
			const uint32_t mw = (uint32_t)img->width, mh = (uint32_t)img->height;
			for (uint32_t y = 0; y < mh; y++)
			{
				for (uint32_t x = 0; x < mw; x++)
				{
					const float r = src[y * mw * 2 + x * 2 + 0] / 127.0f * 2.0f - 1.0f;
					const float g = src[y * mw * 2 + x * 2 + 1] / 127.0f * 2.0f - 1.0f;
					const float b = std::sqrt(std::max(0.0f, 1.0f - r * r - g * g));
					uint8_t* d = img->pixels + y * img->rowPitch + x * 4;
					d[0] = (uint8_t)((r + 1.0f) * 0.5f * 255);
					d[1] = (uint8_t)((g + 1.0f) * 0.5f * 255);
					d[2] = (uint8_t)((b + 1.0f) * 0.5f * 255);
					d[3] = 255;
				}
			}
			src += mw * mh * 2;
		}
		return true;
	}

	//	GraphicsDX11's HDR path (matTextureLoaderHDRWin.cpp, load_hdr_data)
	bool load_hdr(const std::filesystem::path& i_Path, DirectX::ScratchImage& o_Image)
	{
		FILE* fp = _wfopen(i_Path.wstring().c_str(), L"rb");
		if (!fp)
		{
			return false;
		}
		int w = 0, h = 0;
		rgbe_header_info info;
		bool ok = RGBE_ReadHeader(fp, &w, &h, &info) == 0 && w > 0 && h > 0;
		std::vector<float> rgb(ok ? 3 * (size_t)w * h : 0);
		ok = ok && RGBE_ReadPixels_RLE(fp, rgb.data(), w, h) == 0;
		fclose(fp);
		if (!ok || FAILED(o_Image.Initialize2D(DXGI_FORMAT_R16G16B16A16_FLOAT, w, h, 1, 1)))
		{
			return false;
		}
		DirectX::PackedVector::HALF* d = (DirectX::PackedVector::HALF*)o_Image.GetPixels();
		const size_t n = (size_t)w * h;
		DirectX::PackedVector::XMConvertFloatToHalfStream(d, 8, rgb.data(), 12, n);
		DirectX::PackedVector::XMConvertFloatToHalfStream(d + 1, 8, rgb.data() + 1, 12, n);
		DirectX::PackedVector::XMConvertFloatToHalfStream(d + 2, 8, rgb.data() + 2, 12, n);
		for (size_t i = 0; i < n; i++)
		{
			d[i * 4 + 3] = DirectX::PackedVector::XMConvertFloatToHalf(1.0f);
		}
		return true;
	}
}

bool gtLoadReference(const gtImageFile& i_File, DirectX::ScratchImage& o_Image, std::string& o_Error)
{
	const std::wstring path = i_File.m_Path.wstring();
	HRESULT hr = E_FAIL;
	switch (i_File.m_Reference)
	{
		case gtImageFile::e_RefWIC:		hr = DirectX::LoadFromWICFile(path.c_str(), DirectX::WIC_FLAGS_NONE, nullptr, o_Image); break;
		case gtImageFile::e_RefTGA:		hr = DirectX::LoadFromTGAFile(path.c_str(), nullptr, o_Image); break;
		case gtImageFile::e_RefDDS:		hr = DirectX::LoadFromDDSFile(path.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, o_Image); break;
		case gtImageFile::e_RefHDR:		hr = load_hdr(i_File.m_Path, o_Image) ? S_OK : E_FAIL; break;
		case gtImageFile::e_RefCxV8U8:	hr = load_cxv8u8(i_File.m_Path, o_Image) ? S_OK : E_FAIL; break;
	}
	if (FAILED(hr))
	{
		std::ostringstream s;
		s << i_File.m_Name << ": reference load failed (hr 0x" << std::hex << (uint32_t)hr << ")";
		o_Error = s.str();
		return false;
	}
	return true;
}
