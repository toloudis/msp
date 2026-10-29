/****************************************************************************\
**  g2dImageSaveUtil.cpp
**
**	See g2dImageSaveUtil.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef G2D_IMAGESAVEUTIL_HPP
#include "Graphics/G2d/g2dImageSaveUtil.hpp"
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
#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef CPTR_SCANLINEEXRUTIL_HPP
#include "Graphics/G2d/g2dScanlineOpenEXRUtil.hpp"
#endif

#include <tiffio.h>

//	static: ImportExport (glTF) links its own stb_image_write into
//	MachStudio. Not STBI_WRITE_NO_STDIO: it leaves out
//	stbi_write_hdr_to_func too
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <algorithm>
#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>


//============================================================================
//============================================================================
namespace
{
	typedef std::vector<uint8_t> Bytes;

	//------------------------------------------------------------------------
	//	The pixel layouts read from a locked g2dImage
	//------------------------------------------------------------------------
	enum Source
	{
		e_None,
		e_RGBA8,		// e_Color, 32 bits (R8G8B8A8)
		e_BGRA8,		// e_Color, 32 bits, red at bit 16
		e_BGRX8,		// e_RColor (B8G8R8X8)
		e_Grey8,		// e_Luminance8 (R8)
		e_RGBA16,		// e_RGBA16UInt (R16G16B16A16_UNORM)
		e_RGBA16F,		// e_RGBA16f
		e_RGBA32F,		// e_RGBA32f
		e_Grey16F,		// e_Float16 (R16_FLOAT)
		e_Grey32F,		// e_Float32 (R32_FLOAT)
		e_RG32F,		// e_GR32f (R32G32_FLOAT): DDS only
	};

	const int c_Bytes[] = { 0, 4, 4, 4, 1, 8, 8, 16, 2, 4, 8 };

	Source source_of(const g2dPFD& i_PFD)
	{
		switch (i_PFD.GetPixelFormat())
		{
			case g2dPFD::e_Color:
				if (i_PFD.BitsPerPixel() != 32)
					return e_None;
				return (i_PFD.FirstRedBit() == 16 && i_PFD.FirstBlueBit() == 0) ? e_BGRA8 : e_RGBA8;
			case g2dPFD::e_RColor:		return i_PFD.BitsPerPixel() == 32 ? e_BGRX8 : e_None;
			case g2dPFD::e_Luminance8:	return e_Grey8;
			case g2dPFD::e_RGBA16UInt:	return e_RGBA16;
			case g2dPFD::e_RGBA16f:		return e_RGBA16F;
			case g2dPFD::e_RGBA32f:		return e_RGBA32F;
			case g2dPFD::e_Float16:		return e_Grey16F;
			case g2dPFD::e_Float32:		return e_Grey32F;
			case g2dPFD::e_GR32f:		return e_RG32F;
			default:					return e_None;
		}
	}

	//------------------------------------------------------------------------
	//	An image's rows as stored, tightly packed, top row first
	//------------------------------------------------------------------------
	struct Raw
	{
		int		m_Width = 0;
		int		m_Height = 0;
		Source	m_Source = e_None;
		Bytes	m_Bytes;
	};

	//	i_bAnySource: e_RG32F too (DDS); otherwise it throws g2dImageSaveX
	//	as for a format it can't read
	Raw read_raw(const g2dImage* i_pImage, bool i_bAnySource)
	{
		if (!i_pImage)
		{
			DBG_TRACE("g2dImageSaveUtil: no image");
			throw g2dImageSaveX();
		}
		//	Lock isn't const
		g2dImage* image = const_cast<g2dImage*>(i_pImage);
		Raw out;
		out.m_Source = source_of(image->GetPixelFormat());
		out.m_Width = image->GetWidth();
		out.m_Height = image->GetHeight();
		if (out.m_Source == e_None || (out.m_Source == e_RG32F && !i_bAnySource) || out.m_Width <= 0 || out.m_Height <= 0)
		{
			DBG_TRACE("g2dImageSaveUtil: can't write pixel format " << image->GetPixelFormat().GetPixelFormat() << " ("
					  << out.m_Width << "x" << out.m_Height << ")");
			throw g2dImageSaveX();
		}
		const uint8_t* base = static_cast<const uint8_t*>(image->Lock());
		if (!base)
		{
			DBG_TRACE("g2dImageSaveUtil: can't lock the image");
			throw g2dImageSaveX();
		}
		//	the stride is valid only while the image is locked
		const size_t stride = (size_t)image->GetStride();
		const size_t row = (size_t)out.m_Width * c_Bytes[out.m_Source];
		out.m_Bytes.resize(row * out.m_Height);
		for (int y = 0; y < out.m_Height; y++)
			memcpy(&out.m_Bytes[y * row], base + y * stride, row);
		image->Release();
		return out;
	}

	//------------------------------------------------------------------------
	//	An image's pixels as RGBA floats, top row first (grey: the grey in R,
	//	G and B; no alpha: 1)
	//------------------------------------------------------------------------
	struct Pixels
	{
		int					m_Width = 0;
		int					m_Height = 0;
		Source				m_Source = e_None;
		int					m_Channels = 4;		// in the source: 1 (grey), 3 or 4
		bool				m_bFloat = false;
		std::vector<float>	m_Rgba;
	};

	void read_pixel(Source i_Source, const uint8_t* i_pPixel, float* o_pRgba)
	{
		const uint16_t* p16 = reinterpret_cast<const uint16_t*>(i_pPixel);
		const float* p32 = reinterpret_cast<const float*>(i_pPixel);
		switch (i_Source)
		{
			case e_RGBA8:
				for (int c = 0; c < 4; c++)
					o_pRgba[c] = i_pPixel[c] / 255.0f;
				break;
			case e_BGRA8:
			case e_BGRX8:
				o_pRgba[0] = i_pPixel[2] / 255.0f;
				o_pRgba[1] = i_pPixel[1] / 255.0f;
				o_pRgba[2] = i_pPixel[0] / 255.0f;
				o_pRgba[3] = (i_Source == e_BGRX8) ? 1.0f : i_pPixel[3] / 255.0f;
				break;
			case e_Grey8:
				o_pRgba[0] = o_pRgba[1] = o_pRgba[2] = i_pPixel[0] / 255.0f;
				o_pRgba[3] = 1.0f;
				break;
			case e_RGBA16:
				for (int c = 0; c < 4; c++)
					o_pRgba[c] = p16[c] / 65535.0f;
				break;
			case e_RGBA16F:
				for (int c = 0; c < 4; c++)
					o_pRgba[c] = maFunctions::HalfToFloat(p16[c]);
				break;
			case e_RGBA32F:
				for (int c = 0; c < 4; c++)
					o_pRgba[c] = p32[c];
				break;
			case e_Grey16F:
				o_pRgba[0] = o_pRgba[1] = o_pRgba[2] = maFunctions::HalfToFloat(p16[0]);
				o_pRgba[3] = 1.0f;
				break;
			case e_Grey32F:
				o_pRgba[0] = o_pRgba[1] = o_pRgba[2] = p32[0];
				o_pRgba[3] = 1.0f;
				break;
			case e_RG32F:
			case e_None:
				break;
		}
	}

	Pixels read_pixels(const g2dImage* i_pImage)
	{
		const Raw raw = read_raw(i_pImage, false);
		Pixels out;
		out.m_Width = raw.m_Width;
		out.m_Height = raw.m_Height;
		out.m_Source = raw.m_Source;
		out.m_Channels = (raw.m_Source == e_Grey8 || raw.m_Source == e_Grey16F || raw.m_Source == e_Grey32F) ? 1
						 : (raw.m_Source == e_BGRX8 ? 3 : 4);
		out.m_bFloat = (raw.m_Source == e_RGBA16F || raw.m_Source == e_RGBA32F || raw.m_Source == e_Grey16F || raw.m_Source == e_Grey32F);
		const size_t count = (size_t)out.m_Width * out.m_Height;
		out.m_Rgba.resize(count * 4);
		for (size_t i = 0; i < count; i++)
			read_pixel(raw.m_Source, &raw.m_Bytes[i * c_Bytes[raw.m_Source]], &out.m_Rgba[i * 4]);
		return out;
	}

	//------------------------------------------------------------------------
	//	Conversions to what the writers take, top row first, tightly packed
	//------------------------------------------------------------------------
	float clamp01(float i_V)
	{
		return i_V < 0.0f ? 0.0f : (i_V > 1.0f ? 1.0f : i_V);
	}

	//	Float sources are linear; 8-bit files are sRGB (as WIC converted them)
	float to_srgb(float i_Linear)
	{
		const float v = clamp01(i_Linear);
		return v <= 0.0031308f ? v * 12.92f : 1.055f * std::pow(v, 1.0f / 2.4f) - 0.055f;
	}

	//	i_Channels: 1 (grey, from R), 3 (RGB) or 4 (RGBA)
	Bytes to_bytes(const Pixels& i_Pixels, int i_Channels)
	{
		const size_t count = (size_t)i_Pixels.m_Width * i_Pixels.m_Height;
		Bytes out(count * i_Channels);
		for (size_t i = 0; i < count; i++)
		{
			for (int c = 0; c < i_Channels; c++)
			{
				float v = i_Pixels.m_Rgba[i * 4 + c];
				if (i_Pixels.m_bFloat && c < 3)
					v = to_srgb(v);
				out[i * i_Channels + c] = (uint8_t)std::floor(clamp01(v) * 255.0f + 0.5f);
			}
		}
		return out;
	}

	//	16-bit samples: RGBA16 as stored; floats (linear) through the sRGB
	//	curve, alpha linear, as WIC converted them for TIFF
	std::vector<uint16_t> to_words(const Pixels& i_Pixels, int i_Channels)
	{
		const size_t count = (size_t)i_Pixels.m_Width * i_Pixels.m_Height;
		std::vector<uint16_t> out(count * i_Channels);
		for (size_t i = 0; i < count; i++)
		{
			for (int c = 0; c < i_Channels; c++)
			{
				double v = i_Pixels.m_Rgba[i * 4 + c];
				if (i_Pixels.m_bFloat && c < 3)
				{
					v = std::min(1.0, std::max(0.0, v));
					v = v <= 0.0031308 ? v * 12.92 : 1.055 * std::pow(v, 1.0 / 2.4) - 0.055;
				}
				out[i * i_Channels + c] = (uint16_t)std::floor(std::min(1.0, std::max(0.0, v)) * 65535.0 + 0.5);
			}
		}
		return out;
	}

	std::vector<float> to_floats(const Pixels& i_Pixels, int i_Channels)
	{
		const size_t count = (size_t)i_Pixels.m_Width * i_Pixels.m_Height;
		std::vector<float> out(count * i_Channels);
		for (size_t i = 0; i < count; i++)
		{
			for (int c = 0; c < i_Channels; c++)
				out[i * i_Channels + c] = i_Pixels.m_Rgba[i * 4 + c];
		}
		return out;
	}

	//------------------------------------------------------------------------
	//	Output
	//------------------------------------------------------------------------
	void append(void* i_pContext, void* i_pData, int i_Size)
	{
		Bytes& bytes = *static_cast<Bytes*>(i_pContext);
		bytes.insert(bytes.end(), static_cast<const uint8_t*>(i_pData), static_cast<const uint8_t*>(i_pData) + i_Size);
	}

	void append(Bytes& io_Out, const std::string& i_Text)
	{
		io_Out.insert(io_Out.end(), i_Text.begin(), i_Text.end());
	}

	void put32(Bytes& io_Out, uint32_t i_V)
	{
		for (int i = 0; i < 4; i++)
			io_Out.push_back((uint8_t)(i_V >> (8 * i)));
	}

	void check_encoded(int i_Result, const char* i_pFormat)
	{
		if (!i_Result)
		{
			DBG_TRACE("g2dImageSaveUtil: stb_image_write couldn't write " << i_pFormat);
			throw g2dImageSaveX();
		}
	}

	//	Writes the bytes to the file (replacing it); throws g2dImageSaveX
	void write_file(const fsLocator& i_FileName, const Bytes& i_Bytes)
	{
		std::string filename;
		fsFileUtil::LocatorToUnicodeString(i_FileName, filename);
		std::ofstream file(std::filesystem::u8path(filename), std::ios::binary | std::ios::trunc);
		file.write(reinterpret_cast<const char*>(i_Bytes.data()), (std::streamsize)i_Bytes.size());
		if (!file)
		{
			DBG_TRACE("g2dImageSaveUtil: couldn't write " << filename);
			throw g2dImageSaveX();
		}
	}

	//	RGB for colour sources, grey for grey ones; alpha where the source
	//	has it and i_bAlpha
	int channels_for(const Pixels& i_Pixels, bool i_bAlpha)
	{
		if (i_Pixels.m_Channels == 1)
			return 1;
		return (i_bAlpha && i_Pixels.m_Channels == 4) ? 4 : 3;
	}

	//------------------------------------------------------------------------
	//	TIFF in memory (libtiff's client I/O)
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

	int tiff_close(thandle_t)					{ return 0; }
	toff_t tiff_size(thandle_t i_Handle)		{ return static_cast<TiffMemory*>(i_Handle)->m_Bytes.size(); }
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
	//	DDS (DirectXTex's DDS.h and EncodeDDSHeader, MIT licence, Copyright
	//	(c) Microsoft Corporation)
	//------------------------------------------------------------------------
	const uint32_t DDS_MAGIC = 0x20534444;		// "DDS "
	const uint32_t DDS_FOURCC = 0x4, DDS_RGB = 0x40, DDS_RGBA = 0x41, DDS_LUMINANCE = 0x20000;
	const uint32_t DDS_HEADER_FLAGS_TEXTURE = 0x00001007;	// CAPS | HEIGHT | WIDTH | PIXELFORMAT
	const uint32_t DDS_HEADER_FLAGS_MIPMAP = 0x00020000;
	const uint32_t DDS_HEADER_FLAGS_PITCH = 0x00000008;
	const uint32_t DDS_SURFACE_FLAGS_TEXTURE = 0x00001000;

	struct DdsPixelFormat
	{
		uint32_t m_Flags, m_FourCC, m_Bits, m_R, m_G, m_B, m_A;
	};

	//	EncodeDDSHeader's legacy table (DDS_FLAGS_NONE), for the sources
	DdsPixelFormat dds_pixel_format(Source i_Source)
	{
		switch (i_Source)
		{
			case e_RGBA8:	return { DDS_RGBA, 0, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000 };	// A8B8G8R8
			case e_BGRA8:	return { DDS_RGBA, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000 };	// A8R8G8B8
			case e_BGRX8:	return { DDS_RGB, 0, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0 };			// X8R8G8B8
			case e_Grey8:	return { DDS_LUMINANCE, 0, 8, 0xff, 0, 0, 0 };								// L8
			//	Legacy D3DX formats using D3DFMT enum value as FourCC
			case e_RGBA16:	return { DDS_FOURCC, 36, 0, 0, 0, 0, 0 };	// D3DFMT_A16B16G16R16
			case e_RGBA16F:	return { DDS_FOURCC, 113, 0, 0, 0, 0, 0 };	// D3DFMT_A16B16G16R16F
			case e_RGBA32F:	return { DDS_FOURCC, 116, 0, 0, 0, 0, 0 };	// D3DFMT_A32B32G32R32F
			case e_Grey16F:	return { DDS_FOURCC, 111, 0, 0, 0, 0, 0 };	// D3DFMT_R16F
			case e_Grey32F:	return { DDS_FOURCC, 114, 0, 0, 0, 0, 0 };	// D3DFMT_R32F
			case e_RG32F:	return { DDS_FOURCC, 115, 0, 0, 0, 0, 0 };	// D3DFMT_G32R32F
			case e_None:	break;
		}
		return { 0, 0, 0, 0, 0, 0, 0 };
	}
}


//============================================================================
//============================================================================
namespace g2dImageSaveUtil
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
FileType PickFormat(const fsLocator& i_FileName)
{
	std::string fname = i_FileName.GetLastName();
	std::string ext;
	envString::GetExtension(fname, ext);

	const std::string& e = envString::ToLower(ext);

	if (e == "bmp")						return e_BMP;
	if (e == "jpg" || e == "jpeg")		return e_JPG;
	if (e == "tga")						return e_TGA;
	if (e == "png")						return e_PNG;
	if (e == "dds")						return e_DDS;
	if (e == "ppm")						return e_PPM;
	if (e == "dib")						return e_DIB;
	if (e == "hdr")						return e_HDR;
	if (e == "pfm")						return e_PFM;
	if (e == "exr")						return e_EXR;
	if (e == "tif" || e == "tiff")		return e_TIFF;
	throw g2dUnknownImageFileTypeX();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Write(const fsLocator& i_FileName, const g2dImage* i_pImage, FileType i_Type)
{
	switch (i_Type)
	{
		case e_PNG:		WritePNG(i_FileName, i_pImage); return;
		case e_BMP:		WriteBMP(i_FileName, i_pImage); return;
		case e_JPG:		WriteJPG(i_FileName, i_pImage); return;
		case e_TGA:		WriteTGA(i_FileName, i_pImage); return;
		case e_HDR:		WriteHDR(i_FileName, i_pImage); return;
		case e_PPM:		WritePPM(i_FileName, i_pImage); return;
		case e_PFM:		WritePFM(i_FileName, i_pImage); return;
		case e_EXR:		WriteEXR(i_FileName, i_pImage); return;
		case e_TIFF:	WriteTIFF(i_FileName, i_pImage); return;
		case e_DDS:		WriteDDS(i_FileName, i_pImage); return;
		case e_DIB:
			break;
	}
	throw g2dUnknownImageFileTypeX();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void Write(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	Write(i_FileName, i_pImage, PickFormat(i_FileName));
}

//----------------------------------------------------------------------------
//	PNG: 8-bit grey, RGB or RGBA
//----------------------------------------------------------------------------
void WritePNG(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, true);
	const Bytes data = to_bytes(pixels, channels);
	Bytes out;
	check_encoded(stbi_write_png_to_func(append, &out, pixels.m_Width, pixels.m_Height, channels, data.data(),
										 pixels.m_Width * channels), "PNG");
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	BMP: 24-bit, or 32-bit (a V4 header) with alpha
//----------------------------------------------------------------------------
void WriteBMP(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, true);
	const Bytes data = to_bytes(pixels, channels);
	Bytes out;
	check_encoded(stbi_write_bmp_to_func(append, &out, pixels.m_Width, pixels.m_Height, channels, data.data()), "BMP");
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	JPG: quality 90 (WIC's default), no alpha
//----------------------------------------------------------------------------
void WriteJPG(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, false);
	const Bytes data = to_bytes(pixels, channels);
	Bytes out;
	check_encoded(stbi_write_jpg_to_func(append, &out, pixels.m_Width, pixels.m_Height, channels, data.data(), 90), "JPG");
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	TGA: uncompressed 8-bit grey, 24-bit or 32-bit
//----------------------------------------------------------------------------
void WriteTGA(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, true);
	const Bytes data = to_bytes(pixels, channels);
	Bytes out;
	//	file-static (STB_IMAGE_WRITE_STATIC): only this file's writer sees it
	stbi_write_tga_with_rle = 0;
	check_encoded(stbi_write_tga_to_func(append, &out, pixels.m_Width, pixels.m_Height, channels, data.data()), "TGA");
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	HDR: Radiance RGBE, run-length encoded; negative values become 0 (RGBE
//	has no sign, and stb_image_write would wrap them)
//----------------------------------------------------------------------------
void WriteHDR(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, false);
	std::vector<float> data = to_floats(pixels, channels);
	for (float& v : data)
		v = v < 0.0f ? 0.0f : v;
	Bytes out;
	check_encoded(stbi_write_hdr_to_func(append, &out, pixels.m_Width, pixels.m_Height, channels, data.data()), "HDR");
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	PPM: binary, 8-bit; P5 (grey) or P6 (RGB)
//----------------------------------------------------------------------------
void WritePPM(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, false);
	const Bytes data = to_bytes(pixels, channels);
	Bytes out;
	append(out, std::string(channels == 1 ? "P5\n" : "P6\n") + std::to_string(pixels.m_Width) + " " +
				std::to_string(pixels.m_Height) + "\n255\n");
	out.insert(out.end(), data.begin(), data.end());
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	PFM: 32-bit floats, little-endian (a negative scale), bottom row first;
//	Pf (grey) or PF (RGB)
//----------------------------------------------------------------------------
void WritePFM(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const int channels = channels_for(pixels, false);
	const std::vector<float> data = to_floats(pixels, channels);
	Bytes out;
	append(out, std::string(channels == 1 ? "Pf\n" : "PF\n") + std::to_string(pixels.m_Width) + " " +
				std::to_string(pixels.m_Height) + "\n-1.0\n");
	const size_t row_floats = (size_t)pixels.m_Width * channels;
	for (int y = pixels.m_Height - 1; y >= 0; y--)
	{
		const uint8_t* row = reinterpret_cast<const uint8_t*>(data.data() + y * row_floats);
		out.insert(out.end(), row, row + row_floats * sizeof(float));
	}
	write_file(i_FileName, out);
}

//----------------------------------------------------------------------------
//	EXR: g2dScanlineOpenEXRUtil, which writes 8-bit RGBA, half and float
//	RGBA and single-channel images (the others: g2dImageSaveX)
//----------------------------------------------------------------------------
void WriteEXR(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Source source = i_pImage ? source_of(i_pImage->GetPixelFormat()) : e_None;
	if (source != e_RGBA8 && source != e_RGBA16F && source != e_RGBA32F && source != e_Grey16F && source != e_Grey32F)
	{
		DBG_TRACE("g2dImageSaveUtil: can't write this pixel format to EXR");
		throw g2dImageSaveX();
	}
	g2dScanlineOpenEXRUtil::WriteToScanlineEXR(i_FileName, i_pImage);
}

//----------------------------------------------------------------------------
//	TIFF: 8-bit RGBA, RGB or grey for 8-bit sources, 16-bit RGBA or grey
//	for the others; LZW (horizontal prediction for 8-bit samples),
//	unassociated alpha, 72 dpi, strips of about 12 KB (WIC's layout)
//----------------------------------------------------------------------------
void WriteTIFF(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Pixels pixels = read_pixels(i_pImage);
	const bool eight = (pixels.m_Source == e_RGBA8 || pixels.m_Source == e_BGRA8 || pixels.m_Source == e_BGRX8 ||
						pixels.m_Source == e_Grey8);
	const int channels = channels_for(pixels, true);
	const int bits = eight ? 8 : 16;
	Bytes rows;
	if (eight)
	{
		rows = to_bytes(pixels, channels);
	}
	else
	{
		const std::vector<uint16_t> words = to_words(pixels, channels);
		rows.resize(words.size() * 2);
		memcpy(rows.data(), words.data(), rows.size());		// little-endian, as the file ("wl")
	}
	const size_t row_bytes = (size_t)pixels.m_Width * channels * bits / 8;

	TiffMemory memory;
	TIFFOpenOptions* options = TIFFOpenOptionsAlloc();
	TIFFOpenOptionsSetErrorHandlerExtR(options, tiff_error, &memory.m_Error);
	TIFFOpenOptionsSetWarningHandlerExtR(options, tiff_warning, nullptr);
	TIFF* tif = TIFFClientOpenExt("memory", "wl", &memory, tiff_read, tiff_write, tiff_seek, tiff_close, tiff_size,
								  tiff_map, tiff_unmap, options);
	TIFFOpenOptionsFree(options);
	if (!tif)
	{
		DBG_TRACE("g2dImageSaveUtil: " << memory.m_Error.c_str());
		throw g2dImageSaveX();
	}
	const uint32_t rows_per_strip = (uint32_t)std::max<size_t>(1, std::min<size_t>(pixels.m_Height, 12288 / row_bytes));
	TIFFSetField(tif, TIFFTAG_SUBFILETYPE, 0);
	TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, (uint32_t)pixels.m_Width);
	TIFFSetField(tif, TIFFTAG_IMAGELENGTH, (uint32_t)pixels.m_Height);
	TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, (uint16_t)bits);
	TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, (uint16_t)channels);
	TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, channels == 1 ? PHOTOMETRIC_MINISBLACK : PHOTOMETRIC_RGB);
	TIFFSetField(tif, TIFFTAG_COMPRESSION, COMPRESSION_LZW);
	TIFFSetField(tif, TIFFTAG_PREDICTOR, bits == 8 ? PREDICTOR_HORIZONTAL : PREDICTOR_NONE);
	TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tif, TIFFTAG_ROWSPERSTRIP, rows_per_strip);
	TIFFSetField(tif, TIFFTAG_XRESOLUTION, 72.0f);
	TIFFSetField(tif, TIFFTAG_YRESOLUTION, 72.0f);
	TIFFSetField(tif, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
	if (channels == 4)
	{
		const uint16_t extra = EXTRASAMPLE_UNASSALPHA;
		TIFFSetField(tif, TIFFTAG_EXTRASAMPLES, 1, &extra);
	}
	bool written = true;
	Bytes line(row_bytes);
	for (int y = 0; y < pixels.m_Height && written; y++)
	{
		//	TIFFWriteScanline may change the buffer (the predictor)
		memcpy(line.data(), rows.data() + y * row_bytes, row_bytes);
		written = TIFFWriteScanline(tif, line.data(), (uint32_t)y, 0) >= 0;
	}
	TIFFClose(tif);
	if (!written || !memory.m_Error.empty())
	{
		DBG_TRACE("g2dImageSaveUtil: " << memory.m_Error.c_str());
		throw g2dImageSaveX();
	}
	write_file(i_FileName, memory.m_Bytes);
}

//----------------------------------------------------------------------------
//	DDS: the pixels as stored, under DirectXTex's EncodeDDSHeader for one 2D
//	image (one mip, one layer), DDS_FLAGS_NONE
//----------------------------------------------------------------------------
void WriteDDS(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	const Raw raw = read_raw(i_pImage, true);
	const DdsPixelFormat pf = dds_pixel_format(raw.m_Source);
	const size_t row_pitch = (size_t)raw.m_Width * c_Bytes[raw.m_Source];

	Bytes out;
	put32(out, DDS_MAGIC);
	put32(out, 124);
	put32(out, DDS_HEADER_FLAGS_TEXTURE | DDS_HEADER_FLAGS_MIPMAP | DDS_HEADER_FLAGS_PITCH);
	put32(out, (uint32_t)raw.m_Height);
	put32(out, (uint32_t)raw.m_Width);
	put32(out, (uint32_t)row_pitch);
	put32(out, 1);						// depth
	put32(out, 1);						// mipMapCount
	out.insert(out.end(), 11 * 4, 0);	// reserved1
	put32(out, 32);
	put32(out, pf.m_Flags);
	put32(out, pf.m_FourCC);
	put32(out, pf.m_Bits);
	put32(out, pf.m_R);
	put32(out, pf.m_G);
	put32(out, pf.m_B);
	put32(out, pf.m_A);
	put32(out, DDS_SURFACE_FLAGS_TEXTURE);
	out.insert(out.end(), 4 * 4, 0);	// caps2, caps3, caps4, reserved2
	out.insert(out.end(), raw.m_Bytes.begin(), raw.m_Bytes.end());
	write_file(i_FileName, out);
}

}
