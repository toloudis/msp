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
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
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

//	static: ImportExport (glTF) and GraphicsVulkan link their own
//	stb_image_write into MachStudio. Not STBI_WRITE_NO_STDIO: it leaves
//	out stbi_write_hdr_to_func too
#define STB_IMAGE_WRITE_STATIC
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cwctype>
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
	};

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
			default:					return e_None;
		}
	}

	//------------------------------------------------------------------------
	//	An image's pixels as RGBA floats, top row first (grey: the grey in R,
	//	G and B; no alpha: 1)
	//------------------------------------------------------------------------
	struct Pixels
	{
		int					m_Width = 0;
		int					m_Height = 0;
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
			case e_None:
				break;
		}
	}

	Pixels read_pixels(const g2dImage* i_pImage)
	{
		if (!i_pImage)
		{
			DBG_TRACE("g2dImageSaveUtil: no image");
			throw g2dImageSaveX();
		}
		//	Lock isn't const
		g2dImage* image = const_cast<g2dImage*>(i_pImage);
		const Source source = source_of(image->GetPixelFormat());
		Pixels out;
		out.m_Width = image->GetWidth();
		out.m_Height = image->GetHeight();
		if (source == e_None || out.m_Width <= 0 || out.m_Height <= 0)
		{
			DBG_TRACE("g2dImageSaveUtil: can't write pixel format " << image->GetPixelFormat().GetPixelFormat() << " ("
					  << out.m_Width << "x" << out.m_Height << ")");
			throw g2dImageSaveX();
		}
		static const int c_Bytes[] = { 0, 4, 4, 4, 1, 8, 8, 16, 2, 4 };
		out.m_Channels = (source == e_Grey8 || source == e_Grey16F || source == e_Grey32F) ? 1 : (source == e_BGRX8 ? 3 : 4);
		out.m_bFloat = (source == e_RGBA16F || source == e_RGBA32F || source == e_Grey16F || source == e_Grey32F);
		out.m_Rgba.resize((size_t)out.m_Width * out.m_Height * 4);

		const uint8_t* base = static_cast<const uint8_t*>(image->Lock());
		if (!base)
		{
			DBG_TRACE("g2dImageSaveUtil: can't lock the image");
			throw g2dImageSaveX();
		}
		//	the stride is valid only while the image is locked
		const size_t stride = (size_t)image->GetStride();
		for (int y = 0; y < out.m_Height; y++)
		{
			for (int x = 0; x < out.m_Width; x++)
			{
				read_pixel(source, base + y * stride + (size_t)x * c_Bytes[source], &out.m_Rgba[((size_t)y * out.m_Width + x) * 4]);
			}
		}
		image->Release();
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
		itString filename;
		fsFileUtil::LocatorToUnicodeString(i_FileName, filename);
		std::ofstream file(std::filesystem::path(filename.GetString()), std::ios::binary | std::ios::trunc);
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
}


//============================================================================
//============================================================================
namespace g2dImageSaveUtil
{

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
FileType PickFormat(const fsLocator& i_FileName)
{
	itString fname = i_FileName.GetLastName();
	itString ext;
	fname.GetExtension(ext);

	std::wstring e(ext.GetString());
	for (wchar_t& c : e)
		c = (wchar_t)std::towlower(c);

	if (e == L"bmp")						return e_BMP;
	if (e == L"jpg" || e == L"jpeg")		return e_JPG;
	if (e == L"tga")						return e_TGA;
	if (e == L"png")						return e_PNG;
	if (e == L"dds")						return e_DDS;
	if (e == L"ppm")						return e_PPM;
	if (e == L"dib")						return e_DIB;
	if (e == L"hdr")						return e_HDR;
	if (e == L"pfm")						return e_PFM;
	if (e == L"exr")						return e_EXR;
	if (e == L"tif" || e == L"tiff")		return e_TIFF;
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
		case e_TIFF:
		case e_DDS:
		case e_DIB:
			break;
	}
	throw g2dUnknownImageFileTypeX();
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
//	EXR: g2dScanlineOpenEXRUtil
//----------------------------------------------------------------------------
void WriteEXR(const fsLocator& i_FileName, const g2dImage* i_pImage)
{
	g2dScanlineOpenEXRUtil::WriteToScanlineEXR(i_FileName, i_pImage);
}

}
