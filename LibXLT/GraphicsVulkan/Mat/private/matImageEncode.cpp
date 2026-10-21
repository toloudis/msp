/*****************************************************************************
**  matImageEncode.cpp
**
**      See matImageEncode.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_MATIMAGEENCODE_HPP
#include "GraphicsVulkan/Mat/private/matImageEncode.hpp"
#endif
#ifndef GVK_VKDFORMAT_HPP
#include "GraphicsVulkan/vkd/vkdFormat.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/Dbg/dbgMsg.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif

#include <tiffio.h>

//	static: ImportExport (glTF) links its own stb_image_write into MachStudio
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_WRITE_NO_STDIO
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <memory>
#include <string>

namespace gvk
{
namespace
{
	//	The DXGI_FORMAT values used here
	enum : uint32_t
	{
		DXGI_R32G32B32A32_FLOAT	= 2,
		DXGI_R16G16B16A16_FLOAT	= 10,
		DXGI_R16G16B16A16_UNORM	= 11,
		DXGI_R16G16B16A16_SNORM	= 13,
		DXGI_R32G32_FLOAT		= 16,
		DXGI_R8G8B8A8_UNORM		= 28,
		DXGI_R8G8B8A8_UNORM_SRGB = 29,
		DXGI_R8G8B8A8_SNORM		= 31,
		DXGI_R16G16_FLOAT		= 34,
		DXGI_R16G16_UNORM		= 35,
		DXGI_R16G16_SNORM		= 37,
		DXGI_D32_FLOAT			= 40,
		DXGI_R32_FLOAT			= 41,
		DXGI_R8G8_UNORM			= 49,
		DXGI_R8G8_SNORM			= 51,
		DXGI_R16_FLOAT			= 54,
		DXGI_R16_UNORM			= 56,
		DXGI_R8_UNORM			= 61,
		DXGI_A8_UNORM			= 65,
		DXGI_B5G6R5_UNORM		= 85,
		DXGI_B5G5R5A1_UNORM		= 86,
		DXGI_B8G8R8A8_UNORM		= 87,
		DXGI_B8G8R8X8_UNORM		= 88,
		DXGI_B8G8R8A8_UNORM_SRGB = 91,
		DXGI_B8G8R8X8_UNORM_SRGB = 93,
		DXGI_B4G4R4A4_UNORM		= 115,
	};

	typedef std::vector<uint8_t> Bytes;

	//------------------------------------------------------------------------
	//	The WIC pixel format DirectXTex's DXGIToWIC gives (the ones
	//	GraphicsVulkan's images and textures can have)
	//------------------------------------------------------------------------
	enum Source
	{
		e_None,
		e_RGBA8,		// 32bppRGBA
		e_BGRA8,		// 32bppBGRA
		e_BGRX8,		// 32bppBGR
		e_Grey8,		// 8bppGray
		e_RGBA16,		// 64bppRGBA
		e_RGBA16Half,	// 64bppRGBAHalf
		e_RGBA32Float,	// 128bppRGBAFloat
		e_Grey16Half,	// 16bppGrayHalf
		e_Grey32Float,	// 32bppGrayFloat
	};

	Source source_of(uint32_t i_Dxgi)
	{
		switch (i_Dxgi)
		{
			case DXGI_R8G8B8A8_UNORM:
			case DXGI_R8G8B8A8_UNORM_SRGB:	return e_RGBA8;
			case DXGI_B8G8R8A8_UNORM:
			case DXGI_B8G8R8A8_UNORM_SRGB:	return e_BGRA8;
			case DXGI_B8G8R8X8_UNORM:
			case DXGI_B8G8R8X8_UNORM_SRGB:	return e_BGRX8;
			case DXGI_R8_UNORM:				return e_Grey8;
			case DXGI_R16G16B16A16_UNORM:	return e_RGBA16;
			case DXGI_R16G16B16A16_FLOAT:	return e_RGBA16Half;
			case DXGI_R32G32B32A32_FLOAT:	return e_RGBA32Float;
			case DXGI_R16_FLOAT:			return e_Grey16Half;
			case DXGI_R32_FLOAT:
			case DXGI_D32_FLOAT:			return e_Grey32Float;
		}
		return e_None;
	}

	bool is_srgb(uint32_t i_Dxgi)
	{
		return i_Dxgi == DXGI_R8G8B8A8_UNORM_SRGB || i_Dxgi == DXGI_B8G8R8A8_UNORM_SRGB || i_Dxgi == DXGI_B8G8R8X8_UNORM_SRGB;
	}

	bool is_float(Source i_Source)
	{
		return i_Source == e_RGBA16Half || i_Source == e_RGBA32Float || i_Source == e_Grey16Half || i_Source == e_Grey32Float;
	}

	bool is_grey(Source i_Source)
	{
		return i_Source == e_Grey8 || i_Source == e_Grey16Half || i_Source == e_Grey32Float;
	}

	Source checked_source(const matEncodeImage& i_Image)
	{
		const Source source = source_of(i_Image.m_DxgiFormat);
		if (source == e_None || !i_Image.m_pPixels || i_Image.m_Width <= 0 || i_Image.m_Height <= 0)
		{
			DBG_TRACE("matImageEncode: can't write DXGI format " << i_Image.m_DxgiFormat << " (" << i_Image.m_Width << "x"
					  << i_Image.m_Height << ")");
			throw g2dImageSaveX();
		}
		return source;
	}

	//------------------------------------------------------------------------
	//	A source pixel's channels as numbers: unorm / max, or the float;
	//	RGBA order (grey: the grey in R, G and B, alpha 1)
	//------------------------------------------------------------------------
	struct Reader
	{
		Reader(const matEncodeImage& i_Image, Source i_Source) : m_Image(i_Image), m_Source(i_Source) {}

		const uint8_t* pixel(int i_X, int i_Y) const
		{
			static const int c_Bytes[] = { 0, 4, 4, 4, 1, 8, 8, 16, 2, 4 };
			return m_Image.m_pPixels + (size_t)i_Y * m_Image.m_Stride + (size_t)i_X * c_Bytes[m_Source];
		}

		//	8-bit sources: channel i_C (RGBA order) as stored
		uint8_t byte(int i_X, int i_Y, int i_C) const
		{
			const uint8_t* p = pixel(i_X, i_Y);
			switch (m_Source)
			{
				case e_RGBA8:	return p[i_C];
				case e_BGRA8:
				case e_BGRX8:	return p[i_C == 3 ? 3 : 2 - i_C];
				default:		return i_C == 3 ? 255 : p[0];
			}
		}

		//	R16G16B16A16_UNORM: channel i_C as stored
		uint16_t word(int i_X, int i_Y, int i_C) const
		{
			return reinterpret_cast<const uint16_t*>(pixel(i_X, i_Y))[i_C];
		}

		float value(int i_X, int i_Y, int i_C) const
		{
			const uint8_t* p = pixel(i_X, i_Y);
			switch (m_Source)
			{
				case e_RGBA16:		return word(i_X, i_Y, i_C) / 65535.0f;
				case e_RGBA16Half:	return maFunctions::HalfToFloat(reinterpret_cast<const uint16_t*>(p)[i_C]);
				case e_RGBA32Float:	return reinterpret_cast<const float*>(p)[i_C];
				case e_Grey16Half:	return i_C == 3 ? 1.0f : maFunctions::HalfToFloat(*reinterpret_cast<const uint16_t*>(p));
				case e_Grey32Float:	return i_C == 3 ? 1.0f : *reinterpret_cast<const float*>(p);
				default:			return byte(i_X, i_Y, i_C) / 255.0f;
			}
		}

		const matEncodeImage&	m_Image;
		Source					m_Source;
	};

	double clamp01(double i_V)
	{
		return i_V < 0 ? 0 : (i_V > 1 ? 1 : i_V);
	}

	//	WIC converts between float (linear) and integer (sRGB) formats
	//	through the sRGB curve
	double to_srgb(double i_Linear)
	{
		const double v = clamp01(i_Linear);
		return v <= 0.0031308 ? v * 12.92 : 1.055 * std::pow(v, 1.0 / 2.4) - 0.055;
	}

	double to_linear(double i_Srgb)
	{
		return i_Srgb <= 0.04045 ? i_Srgb / 12.92 : std::pow((i_Srgb + 0.055) / 1.055, 2.4);
	}

	uint16_t unorm16(double i_V)
	{
		return (uint16_t)std::floor(clamp01(i_V) * 65535.0 + 0.5);
	}

	uint8_t unorm8(double i_V)
	{
		return (uint8_t)std::floor(clamp01(i_V) * 255.0 + 0.5);
	}

	//	64bppBGRAFixedPoint: s2.13 (8192 is 1), ties rounded away from zero
	uint16_t fixed(double i_V)
	{
		const double f = std::round(i_V * 8192.0);
		return (uint16_t)(int16_t)std::max(-32768.0, std::min(32767.0, f));
	}

	void put16(Bytes& io_Out, uint32_t i_V)	{ io_Out.push_back((uint8_t)i_V); io_Out.push_back((uint8_t)(i_V >> 8)); }
	void put32(Bytes& io_Out, uint32_t i_V)	{ put16(io_Out, i_V & 0xffff); put16(io_Out, i_V >> 16); }
	void put32be(Bytes& io_Out, uint32_t i_V)
	{
		io_Out.push_back((uint8_t)(i_V >> 24)); io_Out.push_back((uint8_t)(i_V >> 16));
		io_Out.push_back((uint8_t)(i_V >> 8)); io_Out.push_back((uint8_t)i_V);
	}

	//------------------------------------------------------------------------
	//	The integer layout WIC's PNG and TIFF encoders take for each source:
	//	8-bit RGBA, RGB or grey as stored; 16-bit RGBA or grey, from floats
	//	through the sRGB curve (alpha linear)
	//------------------------------------------------------------------------
	struct IntLayout
	{
		int	m_Channels;		// 1, 3 or 4
		int	m_Bits;			// 8 or 16
	};

	IntLayout int_layout(Source i_Source)
	{
		switch (i_Source)
		{
			case e_RGBA8:
			case e_BGRA8:		return { 4, 8 };
			case e_BGRX8:		return { 3, 8 };
			case e_Grey8:		return { 1, 8 };
			case e_Grey16Half:
			case e_Grey32Float:	return { 1, 16 };
			default:			return { 4, 16 };
		}
	}

	uint16_t int_sample(const Reader& i_Reader, int i_X, int i_Y, int i_C, const IntLayout& i_Layout)
	{
		if (i_Layout.m_Bits == 8)
		{
			return i_Reader.byte(i_X, i_Y, i_C);
		}
		if (i_Reader.m_Source == e_RGBA16)
		{
			return i_Reader.word(i_X, i_Y, i_C);
		}
		const float v = i_Reader.value(i_X, i_Y, i_C);
		return (i_C == 3) ? unorm16(v) : unorm16(to_srgb(v));
	}

	//	Rows top first, tightly packed; 16-bit samples big-endian (PNG) or
	//	little-endian (TIFF)
	Bytes int_rows(const Reader& i_Reader, const IntLayout& i_Layout, bool i_bBigEndian)
	{
		const matEncodeImage& image = i_Reader.m_Image;
		Bytes out;
		out.reserve((size_t)image.m_Width * image.m_Height * i_Layout.m_Channels * i_Layout.m_Bits / 8);
		for (int y = 0; y < image.m_Height; y++)
		{
			for (int x = 0; x < image.m_Width; x++)
			{
				for (int c = 0; c < i_Layout.m_Channels; c++)
				{
					const uint16_t s = int_sample(i_Reader, x, y, c, i_Layout);
					if (i_Layout.m_Bits == 8)
					{
						out.push_back((uint8_t)s);
					}
					else if (i_bBigEndian)
					{
						out.push_back((uint8_t)(s >> 8));
						out.push_back((uint8_t)s);
					}
					else
					{
						out.push_back((uint8_t)s);
						out.push_back((uint8_t)(s >> 8));
					}
				}
			}
		}
		return out;
	}

	//------------------------------------------------------------------------
	//	PNG chunks
	//------------------------------------------------------------------------
	void png_chunk(Bytes& io_Out, const char* i_pType, const Bytes& i_Data)
	{
		put32be(io_Out, (uint32_t)i_Data.size());
		const size_t start = io_Out.size();
		io_Out.insert(io_Out.end(), i_pType, i_pType + 4);
		io_Out.insert(io_Out.end(), i_Data.begin(), i_Data.end());
		put32be(io_Out, stbiw__crc32(io_Out.data() + start, (int)(io_Out.size() - start)));
	}

	//	stb_image_write's filter choice: per row, the filter whose output
	//	has the smallest sum of absolute (signed) bytes
	Bytes png_filtered(const Bytes& i_Rows, size_t i_RowBytes, int i_PixelBytes, int i_Height)
	{
		Bytes out;
		out.reserve(i_Rows.size() + i_Height);
		Bytes line(i_RowBytes), best;
		for (int y = 0; y < i_Height; y++)
		{
			const uint8_t* cur = i_Rows.data() + (size_t)y * i_RowBytes;
			const uint8_t* up = y > 0 ? cur - i_RowBytes : NULL;
			long best_sum = -1;
			uint8_t best_filter = 0;
			for (uint8_t filter = 0; filter < 5; filter++)
			{
				long sum = 0;
				for (size_t i = 0; i < i_RowBytes; i++)
				{
					const int a = i >= (size_t)i_PixelBytes ? cur[i - i_PixelBytes] : 0;
					const int b = up ? up[i] : 0;
					const int c = (up && i >= (size_t)i_PixelBytes) ? up[i - i_PixelBytes] : 0;
					int predicted = 0;
					switch (filter)
					{
						case 1: predicted = a; break;
						case 2: predicted = b; break;
						case 3: predicted = (a + b) >> 1; break;
						case 4:
						{
							const int p = a + b - c, pa = std::abs(p - a), pb = std::abs(p - b), pc = std::abs(p - c);
							predicted = (pa <= pb && pa <= pc) ? a : (pb <= pc ? b : c);
							break;
						}
					}
					line[i] = (uint8_t)(cur[i] - predicted);
					sum += std::abs((int)(signed char)line[i]);
				}
				if (best_sum < 0 || sum < best_sum)
				{
					best_sum = sum;
					best_filter = filter;
					best = line;
				}
			}
			out.push_back(best_filter);
			out.insert(out.end(), best.begin(), best.end());
		}
		return out;
	}

	//------------------------------------------------------------------------
	//	TIFF in memory
	//------------------------------------------------------------------------
	struct TiffMemory
	{
		Bytes		m_Bytes;
		uint64_t	m_At = 0;
		std::string	m_Error;
	};

	tmsize_t tiff_read(thandle_t i_Handle, void* o_pData, tmsize_t i_Size)
	{
		TiffMemory* m = static_cast<TiffMemory*>(i_Handle);
		if (i_Size < 0 || m->m_At > m->m_Bytes.size())
			return 0;
		const uint64_t n = std::min<uint64_t>((uint64_t)i_Size, m->m_Bytes.size() - m->m_At);
		memcpy(o_pData, m->m_Bytes.data() + m->m_At, (size_t)n);
		m->m_At += n;
		return (tmsize_t)n;
	}

	tmsize_t tiff_write(thandle_t i_Handle, void* i_pData, tmsize_t i_Size)
	{
		TiffMemory* m = static_cast<TiffMemory*>(i_Handle);
		if (i_Size < 0)
			return 0;
		if (m->m_At + i_Size > m->m_Bytes.size())
			m->m_Bytes.resize((size_t)(m->m_At + i_Size));
		memcpy(m->m_Bytes.data() + m->m_At, i_pData, (size_t)i_Size);
		m->m_At += i_Size;
		return i_Size;
	}

	toff_t tiff_seek(thandle_t i_Handle, toff_t i_Offset, int i_Whence)
	{
		TiffMemory* m = static_cast<TiffMemory*>(i_Handle);
		uint64_t base = 0;
		switch (i_Whence)
		{
			case SEEK_SET:	base = 0; break;
			case SEEK_CUR:	base = m->m_At; break;
			case SEEK_END:	base = m->m_Bytes.size(); break;
			default:		return (toff_t)-1;
		}
		m->m_At = base + i_Offset;
		return m->m_At;
	}

	int tiff_close(thandle_t)				{ return 0; }
	toff_t tiff_size(thandle_t i_Handle)	{ return static_cast<TiffMemory*>(i_Handle)->m_Bytes.size(); }
	int tiff_map(thandle_t, void**, toff_t*)	{ return 0; }
	void tiff_unmap(thandle_t, void*, toff_t)	{}

	int tiff_error(TIFF*, void* i_pUser, const char* i_pModule, const char* i_pFormat, va_list i_Args)
	{
		std::string* error = static_cast<std::string*>(i_pUser);
		if (error->empty())
		{
			char text[512];
			vsnprintf(text, sizeof(text), i_pFormat, i_Args);
			*error = std::string("libtiff: ") + (i_pModule ? std::string(i_pModule) + ": " : std::string()) + text;
		}
		return 1;
	}

	int tiff_warning(TIFF*, void*, const char*, const char*, va_list)
	{
		return 1;
	}

	//------------------------------------------------------------------------
	//	DDS (DirectXTex's DDS.h)
	//------------------------------------------------------------------------
	const uint32_t DDS_MAGIC = 0x20534444;		// "DDS "
	const uint32_t DDS_FOURCC = 0x4, DDS_RGB = 0x40, DDS_RGBA = 0x41, DDS_LUMINANCE = 0x20000, DDS_LUMINANCEA = 0x20001,
				   DDS_ALPHA = 0x2, DDS_BUMPDUDV = 0x80000;
	const uint32_t DDS_HEADER_FLAGS_TEXTURE = 0x00001007;	// CAPS | HEIGHT | WIDTH | PIXELFORMAT
	const uint32_t DDS_HEADER_FLAGS_MIPMAP = 0x00020000;
	const uint32_t DDS_HEADER_FLAGS_PITCH = 0x00000008;
	const uint32_t DDS_SURFACE_FLAGS_TEXTURE = 0x00001000;
	const uint32_t DDS_DIMENSION_TEXTURE2D = 3;

	constexpr uint32_t fourcc(char a, char b, char c, char d)
	{
		return (uint32_t)(uint8_t)a | ((uint32_t)(uint8_t)b << 8) | ((uint32_t)(uint8_t)c << 16) | ((uint32_t)(uint8_t)d << 24);
	}

	struct DdsPixelFormat
	{
		uint32_t m_Size, m_Flags, m_FourCC, m_Bits, m_R, m_G, m_B, m_A;
	};

	//	EncodeDDSHeader's legacy table (DDS_FLAGS_NONE; the packed and
	//	block-compressed rows aside); m_Size 0: a "DX10" header
	DdsPixelFormat dds_pixel_format(uint32_t i_Dxgi)
	{
		const uint32_t s = 32;
		switch (i_Dxgi)
		{
			case DXGI_R8G8B8A8_UNORM:		return { s, DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };	// A8B8G8R8
			case DXGI_R16G16_UNORM:			return { s, DDS_RGB, 0, 32, 0x0000ffff, 0xffff0000, 0, 0 };						// G16R16
			case DXGI_R8G8_UNORM:			return { s, DDS_LUMINANCEA, 0, 16, 0x00ff, 0, 0, 0xff00 };						// A8L8
			case DXGI_R16_UNORM:			return { s, DDS_LUMINANCE, 0, 16, 0xffff, 0, 0, 0 };							// L16
			case DXGI_R8_UNORM:				return { s, DDS_LUMINANCE, 0, 8, 0xff, 0, 0, 0 };								// L8
			case DXGI_A8_UNORM:				return { s, DDS_ALPHA, 0, 8, 0, 0, 0, 0xff };									// A8
			case DXGI_B5G6R5_UNORM:			return { s, DDS_RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0 };						// R5G6B5
			case DXGI_B5G5R5A1_UNORM:		return { s, DDS_RGBA, 0, 16, 0x7c00, 0x03e0, 0x001f, 0x8000 };					// A1R5G5B5
			case DXGI_R8G8_SNORM:			return { s, DDS_BUMPDUDV, 0, 16, 0x00ff, 0xff00, 0, 0 };						// V8U8
			case DXGI_R8G8B8A8_SNORM:		return { s, DDS_BUMPDUDV, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };	// Q8W8V8U8
			case DXGI_R16G16_SNORM:			return { s, DDS_BUMPDUDV, 0, 32, 0x0000ffff, 0xffff0000, 0, 0 };				// V16U16
			case DXGI_B8G8R8A8_UNORM:		return { s, DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };	// A8R8G8B8
			case DXGI_B8G8R8X8_UNORM:		return { s, DDS_RGB, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0 };			// X8R8G8B8
			case DXGI_B4G4R4A4_UNORM:		return { s, DDS_RGBA, 0, 16, 0x0f00, 0x00f0, 0x000f, 0xf000 };					// A4R4G4B4
			//	Legacy D3DX formats using D3DFMT enum value as FourCC
			case DXGI_R32G32B32A32_FLOAT:	return { s, DDS_FOURCC, 116, 0, 0, 0, 0, 0 };	// D3DFMT_A32B32G32R32F
			case DXGI_R16G16B16A16_FLOAT:	return { s, DDS_FOURCC, 113, 0, 0, 0, 0, 0 };	// D3DFMT_A16B16G16R16F
			case DXGI_R16G16B16A16_UNORM:	return { s, DDS_FOURCC, 36, 0, 0, 0, 0, 0 };	// D3DFMT_A16B16G16R16
			case DXGI_R16G16B16A16_SNORM:	return { s, DDS_FOURCC, 110, 0, 0, 0, 0, 0 };	// D3DFMT_Q16W16V16U16
			case DXGI_R32G32_FLOAT:			return { s, DDS_FOURCC, 115, 0, 0, 0, 0, 0 };	// D3DFMT_G32R32F
			case DXGI_R16G16_FLOAT:			return { s, DDS_FOURCC, 112, 0, 0, 0, 0, 0 };	// D3DFMT_G16R16F
			case DXGI_R32_FLOAT:			return { s, DDS_FOURCC, 114, 0, 0, 0, 0, 0 };	// D3DFMT_R32F
			case DXGI_R16_FLOAT:			return { s, DDS_FOURCC, 111, 0, 0, 0, 0, 0 };	// D3DFMT_R16F
		}
		return { 0, 0, 0, 0, 0, 0, 0, 0 };
	}
}

//----------------------------------------------------------------------------
//	BMP: WIC's BMP encoder takes 32bppBGRA (with a V5 header), 32bppBGR,
//	8bppIndexed and 64bppBGRAFixedPoint; DirectXTex's formats become one of
//	them
//----------------------------------------------------------------------------
std::vector<uint8_t> matEncodeBMP(const matEncodeImage& i_Image)
{
	const Source source = checked_source(i_Image);
	const Reader reader(i_Image, source);
	const int w = i_Image.m_Width, h = i_Image.m_Height;

	const bool v5 = (source == e_RGBA8 || source == e_BGRA8);
	const int bpp = (source == e_Grey8) ? 8 : ((source == e_RGBA8 || source == e_BGRA8 || source == e_BGRX8) ? 32 : 64);

	//	8-bit grey: a palette of the greys used, in the order they first
	//	appear (top row first), as WIC's converter makes it. Its pixels
	//	aren't the exact entries: WIC reduces each grey to 4 bits (g & 0xf0
	//	| g >> 4) and takes the palette entry nearest that, the first one
	//	of two as near (measured)
	std::vector<int> index(256, -1);
	std::vector<uint8_t> palette;
	if (source == e_Grey8)
	{
		std::vector<bool> used(256, false);
		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				const uint8_t g = reader.byte(x, y, 0);
				if (!used[g])
				{
					used[g] = true;
					palette.push_back(g);
				}
			}
		}
		for (int g = 0; g < 256; g++)
		{
			const int q = (g & 0xf0) | (g >> 4);
			int best = 0;
			for (size_t i = 1; i < palette.size(); i++)
			{
				if (std::abs(palette[i] - q) < std::abs(palette[best] - q))
					best = (int)i;
			}
			index[g] = best;
		}
	}

	const uint32_t info_size = v5 ? 124 : 40;
	const uint32_t extra = v5 ? 12 : (uint32_t)palette.size() * 4;
	const uint32_t offset = 14 + info_size + extra;
	const size_t stride = (((size_t)w * bpp + 31) / 32) * 4;

	Bytes out;
	out.reserve(offset + stride * h);
	out.push_back('B');
	out.push_back('M');
	put32(out, (uint32_t)(offset + stride * h));
	put32(out, 0);
	put32(out, offset);

	put32(out, info_size);
	put32(out, (uint32_t)w);
	put32(out, (uint32_t)h);			// bottom-up
	put16(out, 1);
	put16(out, (uint32_t)bpp);
	put32(out, v5 ? 3 : 0);				// BI_BITFIELDS : BI_RGB
	put32(out, 0);
	put32(out, 2835);					// 72 dpi
	put32(out, 2835);
	put32(out, (uint32_t)palette.size());
	put32(out, (uint32_t)palette.size());
	if (v5)
	{
		put32(out, 0x00ff0000);
		put32(out, 0x0000ff00);
		put32(out, 0x000000ff);
		put32(out, 0xff000000);
		put32(out, 0x57696e20);			// LCS_WINDOWS_COLOR_SPACE
		out.insert(out.end(), 36 + 12 + 16, 0);	// endpoints, gammas, intent, profile, reserved
		//	WIC follows the V5 header with a colour table of three masks
		put32(out, 0x000000ff);
		put32(out, 0x0000ff00);
		put32(out, 0x00ff0000);
	}
	for (uint8_t g : palette)
	{
		out.push_back(g);
		out.push_back(g);
		out.push_back(g);
		out.push_back(255);
	}

	for (int y = h - 1; y >= 0; y--)
	{
		const size_t start = out.size();
		for (int x = 0; x < w; x++)
		{
			switch (bpp)
			{
				case 8:
					out.push_back((uint8_t)index[reader.byte(x, y, 0)]);
					break;
				case 32:
					if (source == e_BGRX8)
					{
						const uint8_t* p = reader.pixel(x, y);
						out.insert(out.end(), p, p + 4);
					}
					else
					{
						out.push_back(reader.byte(x, y, 2));
						out.push_back(reader.byte(x, y, 1));
						out.push_back(reader.byte(x, y, 0));
						out.push_back(reader.byte(x, y, 3));
					}
					break;
				default:
				{
					//	64bppBGRAFixedPoint is linear: 16-bit integers (sRGB) are
					//	linearised, floats are taken as they are
					for (int c : { 2, 1, 0, 3 })
					{
						double v = reader.value(x, y, c);
						if (source == e_RGBA16 && c != 3)
						{
							v = to_linear(v);
						}
						put16(out, fixed(v));
					}
					break;
				}
			}
		}
		out.insert(out.end(), stride - (out.size() - start), 0);
	}
	return out;
}

//----------------------------------------------------------------------------
//	JPG: stb_image_write at WIC's default quality
//----------------------------------------------------------------------------
std::vector<uint8_t> matEncodeJPG(const matEncodeImage& i_Image)
{
	const Source source = checked_source(i_Image);
	const Reader reader(i_Image, source);
	const int channels = is_grey(source) ? 1 : 3;
	Bytes rgb;
	rgb.reserve((size_t)i_Image.m_Width * i_Image.m_Height * channels);
	for (int y = 0; y < i_Image.m_Height; y++)
	{
		for (int x = 0; x < i_Image.m_Width; x++)
		{
			for (int c = 0; c < channels; c++)
			{
				if (source == e_RGBA16)
					rgb.push_back((uint8_t)((reader.word(x, y, c) * 255u + 32767u) / 65535u));
				else if (is_float(source))
					rgb.push_back(unorm8(to_srgb(reader.value(x, y, c))));
				else
					rgb.push_back(reader.byte(x, y, c));
			}
		}
	}
	Bytes out;
	auto append = [](void* i_pContext, void* i_pData, int i_Size)
	{
		Bytes& bytes = *static_cast<Bytes*>(i_pContext);
		bytes.insert(bytes.end(), static_cast<const uint8_t*>(i_pData), static_cast<const uint8_t*>(i_pData) + i_Size);
	};
	if (!stbi_write_jpg_to_func(append, &out, i_Image.m_Width, i_Image.m_Height, channels, rgb.data(), 90))
	{
		DBG_TRACE("stbi_write_jpg failed");
		throw g2dImageSaveX();
	}
	return out;
}

//----------------------------------------------------------------------------
//	PNG: 8-bit RGBA, RGB or grey, or 16-bit RGBA or grey (WIC's PNG
//	encoder takes them all); DirectXTex's gAMA (or sRGB) chunk and WIC's
//	pHYs
//----------------------------------------------------------------------------
std::vector<uint8_t> matEncodePNG(const matEncodeImage& i_Image)
{
	const Source source = checked_source(i_Image);
	const Reader reader(i_Image, source);
	const IntLayout layout = int_layout(source);
	const int pixel_bytes = layout.m_Channels * layout.m_Bits / 8;
	const Bytes rows = int_rows(reader, layout, true);
	const Bytes filtered = png_filtered(rows, (size_t)i_Image.m_Width * pixel_bytes, pixel_bytes, i_Image.m_Height);

	int compressed_size = 0;
	unsigned char* compressed = stbi_zlib_compress(const_cast<unsigned char*>(filtered.data()), (int)filtered.size(),
												   &compressed_size, stbi_write_png_compression_level);
	if (!compressed)
	{
		DBG_TRACE("stbi_zlib_compress failed");
		throw g2dImageSaveX();
	}
	const Bytes idat(compressed, compressed + compressed_size);
	STBIW_FREE(compressed);

	Bytes out = { 0x89, 'P', 'N', 'G', '\r', '\n', 0x1a, '\n' };
	Bytes ihdr;
	put32be(ihdr, (uint32_t)i_Image.m_Width);
	put32be(ihdr, (uint32_t)i_Image.m_Height);
	ihdr.push_back((uint8_t)layout.m_Bits);
	ihdr.push_back(layout.m_Channels == 4 ? 6 : (layout.m_Channels == 3 ? 2 : 0));
	ihdr.insert(ihdr.end(), 3, 0);		// deflate, adaptive filtering, no interlace
	png_chunk(out, "IHDR", ihdr);
	if (is_srgb(i_Image.m_DxgiFormat))
	{
		png_chunk(out, "sRGB", Bytes(1, 0));
	}
	else
	{
		Bytes gamma;
		put32be(gamma, 100000);			// gamma 1.0
		png_chunk(out, "gAMA", gamma);
	}
	Bytes phys;
	put32be(phys, 2834);				// 72 dpi
	put32be(phys, 2834);
	phys.push_back(1);					// per metre
	png_chunk(out, "pHYs", phys);
	png_chunk(out, "IDAT", idat);
	png_chunk(out, "IEND", Bytes());
	return out;
}

//----------------------------------------------------------------------------
//	TIFF: PNG's samples, LZW-compressed, in strips of about 12 KB
//----------------------------------------------------------------------------
std::vector<uint8_t> matEncodeTIFF(const matEncodeImage& i_Image)
{
	const Source source = checked_source(i_Image);
	const Reader reader(i_Image, source);
	const IntLayout layout = int_layout(source);
	const size_t row_bytes = (size_t)i_Image.m_Width * layout.m_Channels * layout.m_Bits / 8;
	const Bytes rows = int_rows(reader, layout, false);

	TiffMemory memory;
	TIFFOpenOptions* options = TIFFOpenOptionsAlloc();
	TIFFOpenOptionsSetErrorHandlerExtR(options, tiff_error, &memory.m_Error);
	TIFFOpenOptionsSetWarningHandlerExtR(options, tiff_warning, nullptr);
	TIFF* tif = TIFFClientOpenExt("memory", "wl", &memory, tiff_read, tiff_write, tiff_seek, tiff_close, tiff_size,
								  tiff_map, tiff_unmap, options);
	TIFFOpenOptionsFree(options);
	if (!tif)
	{
		DBG_TRACE("matEncodeTIFF: " << memory.m_Error.c_str());
		throw g2dImageSaveX();
	}
	const uint32_t rows_per_strip = (uint32_t)std::max<size_t>(1, std::min<size_t>(i_Image.m_Height, 12288 / row_bytes));
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, 0);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)i_Image.m_Width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)i_Image.m_Height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)layout.m_Bits);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)layout.m_Channels);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, layout.m_Channels == 1 ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	TIFFSetField(tif, TIFFTAG_PREDICTOR, layout.m_Bits == 8 ? PREDICTOR_HORIZONTAL : PREDICTOR_NONE);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rows_per_strip);
	TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72.0f);
	TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72.0f);
	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	if (layout.m_Channels == 4)
	{
		const uint16_t extra = EXTRASAMPLE_UNASSALPHA;
		TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extra);
	}
	bool written = true;
	std::vector<uint8_t> line(row_bytes);
	for (int y = 0; y < i_Image.m_Height && written; y++)
	{
		//	TIFFWriteScanline may change the buffer (the predictor)
		memcpy(line.data(), rows.data() + y * row_bytes, row_bytes);
		written = TIFFWriteScanline(tif, line.data(), (uint32_t)y, 0) >= 0;
	}
	TIFFClose(tif);
	if (!written || !memory.m_Error.empty())
	{
		DBG_TRACE("matEncodeTIFF: " << memory.m_Error.c_str());
		throw g2dImageSaveX();
	}
	return memory.m_Bytes;
}

//----------------------------------------------------------------------------
//	DDS: DirectXTex's EncodeDDSHeader and SaveToDDSFile for one 2D image
//	(one mip, one layer), DDS_FLAGS_NONE
//----------------------------------------------------------------------------
std::vector<uint8_t> matEncodeDDS(const matEncodeImage& i_Image)
{
	const vkdFormatInfo info = vkdFormatInfoOf(vkdFormatFromDxgi(i_Image.m_DxgiFormat));
	if (info.m_BlockBytes == 0 || info.m_bCompressed || info.m_bDepth || !i_Image.m_pPixels ||
		i_Image.m_Width <= 0 || i_Image.m_Height <= 0)
	{
		DBG_TRACE("matEncodeDDS: can't write DXGI format " << i_Image.m_DxgiFormat);
		throw g2dImageSaveX();
	}
	const DdsPixelFormat pf = dds_pixel_format(i_Image.m_DxgiFormat);
	const size_t row_pitch = (size_t)i_Image.m_Width * info.m_BlockBytes;

	Bytes out;
	put32(out, DDS_MAGIC);
	put32(out, 124);
	put32(out, DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP | DDS_HEADER_FLAGS_PITCH);
	put32(out, (uint32_t)i_Image.m_Height);
	put32(out, (uint32_t)i_Image.m_Width);
	put32(out, (uint32_t)row_pitch);
	put32(out, 1);						// depth
	put32(out, 1);						// mipMapCount
	out.insert(out.end(), 11 * 4, 0);	// reserved1
	put32(out, 32);
	put32(out, pf.m_Size ? pf.m_Flags : DDS_FOURCC);
	put32(out, pf.m_Size ? pf.m_FourCC : fourcc('D', 'X', '1', '0'));
	put32(out, pf.m_Bits);
	put32(out, pf.m_R);
	put32(out, pf.m_G);
	put32(out, pf.m_B);
	put32(out, pf.m_A);
	put32(out, DDS_SURFACE_FLAGS_TEXTURE);
	out.insert(out.end(), 4 * 4, 0);	// caps2, caps3, caps4, reserved2
	if (!pf.m_Size)
	{
		put32(out, i_Image.m_DxgiFormat);
		put32(out, DDS_DIMENSION_TEXTURE2D);
		put32(out, 0);					// miscFlag
		put32(out, 1);					// arraySize
		put32(out, 0);					// miscFlags2
	}
	for (int y = 0; y < i_Image.m_Height; y++)
	{
		const uint8_t* row = i_Image.m_pPixels + (size_t)y * i_Image.m_Stride;
		out.insert(out.end(), row, row + row_pitch);
	}
	return out;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void matEncodeWriteFile(const fsLocator& i_File, const std::vector<uint8_t>& i_Bytes)
{
	itString filename;
	fsFileUtil::LocatorToUnicodeString(i_File, filename);
	std::ofstream file(std::filesystem::path(filename.GetString()), std::ios::binary | std::ios::trunc);
	file.write(reinterpret_cast<const char*>(i_Bytes.data()), (std::streamsize)i_Bytes.size());
	if (!file)
	{
		DBG_TRACE("Couldn't write " << filename);
		throw g2dImageSaveX();
	}
}
}
