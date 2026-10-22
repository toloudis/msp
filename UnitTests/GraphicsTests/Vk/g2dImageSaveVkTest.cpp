/*****************************************************************************
**  g2dImageSaveVkTest.cpp
**
**      Image and texture saving (g2dImageSave, matTextureMgr's
**      SaveTextureToFile, SaveTextureToRgbaTiff and SaveTextureToRgbaPNG)
**      and MergeTransparentTextures. GraphicsDX11 writes BMP, JPG, PNG and
**      TIFF through DirectXTex and WIC, and DDS through DirectXTex;
**      GraphicsVulkan has neither, and writes the same layouts itself
**      (Plan 6 decision 3; the layouts were measured in Task 2 Step 1 and
**      are in the plan's ledger). Each test saves the same pixels through
**      both backends, checks the headers, and decodes both files with the
**      same decoder (stb_image, libtiff); the pixels must be equal.
**      BMP and DDS files must be byte-identical.
**
**      Exceptions, measured (decision 3):
**      - JPG: two JPEG encoders (WIC's, stb_image_write's) at quality 90
**        never agree byte for byte; the decoded images are compared with
**        c_JpgMaxErr and c_JpgPctOver. A grey source gives a one-component
**        JPEG on DX11 and a three-component one on Vulkan
**        (stb_image_write writes only YCbCr).
**      - Float sources (RGBA16F, RGBA32F, R16F, R32F) in 16-bit PNG and
**        TIFF: WIC's linear-to-sRGB conversion is an approximation of the
**        sRGB curve that GraphicsVulkan computes exactly; they agree within
**        c_SrgbMax16 (of 65535).
**      - RGBA16 (UNORM) sources in BMP (64-bit fixed point, linear): WIC's
**        sRGB-to-linear conversion is an approximation too; its colour
**        values are up to c_FixedMax (of 8192) above GraphicsVulkan's
**        exact ones (measured: 155 of 2553 samples 1 higher). The header
**        is byte-identical.
**      - PNG text chunk: DX11's files say Software=DirectXTex (a tEXt
**        chunk), and its TIFFs carry Software and an XMP packet naming
**        DirectXTex; Vulkan's don't.
**      Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtSceneWindow.hpp"
#include "gtTarget.hpp"
#include "gtTempDir.hpp"

#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif
#ifndef G2D_IMAGESAVE_HPP
#include "Graphics/G2d/g2dImageSave.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G2D_EXCEPTIONX_HPP
#include "Graphics/G2d/g2dExceptionX.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/Ma/maFunctions.hpp"
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_ONLY_JPEG
#include <stb_image.h>

#include <tiffio.h>

#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace
{
	typedef std::vector<uint8_t> Bytes;

	//	The exceptions to "decoded pixels equal" (see the top of the file)
	const float c_JpgMaxErr = 6 / 255.0f;
	const float c_JpgPctOver = 37.0f;
	const int c_SrgbMax16 = 2;
	const int c_FixedMax = 1;

	const int c_W = 37, c_H = 23;	// odd: BMP rows are padded

	struct Format
	{
		const char*	m_pName;
		g2dPFD		m_PFD;
		int			m_Channels;
		int			m_Bits;			// per channel
		bool		m_bFloat;
		bool		m_bTens = false;	// greys 250, 240, ... 0 (see pixels)
		bool		m_bWicFails = false;	// DirectXTex has no WIC format for it
	};

	const Format& rgba8()
	{
		static const Format f = { "RGBA8", g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32), 4, 8, false };
		return f;
	}

	//	The formats a capture or a texture can have (Step 1's rows)
	std::vector<Format> other_formats()
	{
		return
		{
			{ "BGRX8",	 g2dPFD(g2dPFD::e_RColor, 32), 4, 8, false },
			{ "RGBA16F", g2dPFD(g2dPFD::e_RGBA16f, 64), 4, 16, true },
			{ "RGBA16U", g2dPFD(g2dPFD::e_RGBA16UInt, 64), 4, 16, false },
			{ "RGBA32F", g2dPFD(g2dPFD::e_RGBA32f, 128), 4, 32, true },
			{ "RG32F",	 g2dPFD(g2dPFD::e_GR32f, 64), 2, 32, true, false, true },
			{ "R16F",	 g2dPFD(g2dPFD::e_Float16, 16), 1, 16, true },
			{ "R32F",	 g2dPFD(g2dPFD::e_Float32, 32), 1, 32, true },
			{ "L8",		 g2dPFD(g2dPFD::e_Luminance8, 8), 1, 8, false },
			{ "L8tens",	 g2dPFD(g2dPFD::e_Luminance8, 8), 1, 8, false, true },
		};
	}

	//	Smooth content in [0, 1] (JPEG encoders differ least on smooth images)
	float smooth(int i_X, int i_Y, int i_Channel)
	{
		const float u = (float)i_X / (c_W - 1), v = (float)i_Y / (c_H - 1);
		switch (i_Channel)
		{
			case 0:		return u;
			case 1:		return v;
			case 2:		return 0.5f + 0.5f * std::sin(3.0f * u + 2.0f * v);
			default:	return 1.0f - 0.6f * u * v;
		}
	}

	//	Tightly packed pixels: floats in [-0.25, 1.5] (clamping is part of
	//	what is compared); i_bOpaqueX sets BGRX8's X byte to 255 (textures:
	//	Vulkan has no X8 formats, its B8G8R8A8 image keeps alpha 255).
	//	m_bTens: a few greys (multiples of 10, the highest first), so that
	//	WIC's palette lookup for an 8-bit grey BMP meets two entries as near
	//	(80 and 90 for 85)
	Bytes pixels(const Format& i_Format, bool i_bOpaqueX = false)
	{
		const int bytes = i_Format.m_Channels * i_Format.m_Bits / 8;
		Bytes out((size_t)c_W * c_H * bytes);
		for (int y = 0; y < c_H; y++)
		{
			for (int x = 0; x < c_W; x++)
			{
				uint8_t* p = out.data() + ((size_t)y * c_W + x) * bytes;
				for (int c = 0; c < i_Format.m_Channels; c++)
				{
					const float t = smooth(x, y, i_Format.m_Channels == 1 ? 2 : c);
					if (i_Format.m_bTens)
						p[c] = (uint8_t)(250 - ((x + y) % 26) * 10);
					else if (i_Format.m_Bits == 8)
						p[c] = (uint8_t)std::lround(t * 255.0f);
					else if (!i_Format.m_bFloat)
						reinterpret_cast<uint16_t*>(p)[c] = (uint16_t)std::lround(t * 65535.0f);
					else if (i_Format.m_Bits == 16)
						reinterpret_cast<uint16_t*>(p)[c] = maFunctions::FloatToHalf(t * 1.75f - 0.25f);
					else
						reinterpret_cast<float*>(p)[c] = t * 1.75f - 0.25f;
				}
				if (i_bOpaqueX && i_Format.m_PFD.GetPixelFormat() == g2dPFD::e_RColor)
					p[3] = 255;
			}
		}
		return out;
	}

	std::unique_ptr<g2dImage> make_image(const Format& i_Format)
	{
		std::unique_ptr<g2dImage> image(g2dImageCreate::Make(c_W, c_H, i_Format.m_PFD, g2dImage::e_SystemMemory));
		const Bytes data = pixels(i_Format);
		const size_t row = data.size() / c_H;
		uint8_t* base = static_cast<uint8_t*>(image->Lock());
		for (int y = 0; y < c_H; y++)
			memcpy(base + (size_t)y * image->GetStride(), data.data() + y * row, row);
		image->Release();
		return image;
	}

	//	What an exception (or none) was: a name to compare
	template <class F>
	std::string outcome(F i_Function)
	{
		try
		{
			i_Function();
			return "ok";
		}
		catch (const g2dUnknownImageFileTypeX&)	{ return "g2dUnknownImageFileTypeX"; }
		catch (const g2dImageSaveX&)			{ return "g2dImageSaveX"; }
		catch (const envExceptionX&)			{ return "envExceptionX"; }
	}

	//	One save: what it threw, and the file (if one was written)
	struct Saved
	{
		std::string	m_Outcome;
		bool		m_bExists = false;
		Bytes		m_File;
		bool		m_bGreySource = false;
		bool		m_bFloatSource = false;
		bool		m_bUnorm16Source = false;
	};
	typedef std::map<std::string, Saved> Results;

	Saved saved(const std::string& i_Outcome, const std::filesystem::path& i_Path, const Format* i_pFormat)
	{
		Saved s;
		s.m_Outcome = i_Outcome;
		std::ifstream file(i_Path, std::ios::binary);
		s.m_bExists = (bool)file;
		if (file)
			s.m_File.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		if (i_pFormat)
		{
			s.m_bGreySource = i_pFormat->m_Channels == 1;
			s.m_bFloatSource = i_pFormat->m_bFloat;
			s.m_bUnorm16Source = !i_pFormat->m_bFloat && i_pFormat->m_Bits == 16;
		}
		return s;
	}

	fsLocator locator_of(const std::filesystem::path& i_Path)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(i_Path.wstring().c_str()), locator);
		return locator;
	}

	std::string extension(const std::string& i_Name)
	{
		return i_Name.substr(i_Name.find_last_of('.') + 1);
	}

	//------------------------------------------------------------------------
	//	PNG: IHDR, gAMA and pHYs as DX11's; the other chunks in the same
	//	order (DX11's tEXt aside); pixels decoded by stb_image
	//------------------------------------------------------------------------
	struct PngChunks
	{
		std::vector<std::string>				m_Order;
		std::map<std::string, Bytes>			m_Data;
	};

	PngChunks png_chunks(const Bytes& i_File)
	{
		PngChunks out;
		size_t p = 8;
		while (p + 12 <= i_File.size())
		{
			const uint32_t length = ((uint32_t)i_File[p] << 24) | ((uint32_t)i_File[p + 1] << 16) | ((uint32_t)i_File[p + 2] << 8) | i_File[p + 3];
			const std::string type(reinterpret_cast<const char*>(&i_File[p + 4]), 4);
			if (type != "tEXt")
				out.m_Order.push_back(type);
			if (type != "IDAT")
				out.m_Data[type] = Bytes(i_File.begin() + p + 8, i_File.begin() + std::min(i_File.size(), p + 8 + length));
			p += 12 + length;
		}
		return out;
	}

	struct Decoded16
	{
		int					m_Width = 0, m_Height = 0, m_Channels = 0;
		std::vector<int>	m_Samples;		// 16-bit scale
	};

	Decoded16 decode_png(const Bytes& i_File)
	{
		Decoded16 d;
		stbi_us* p = stbi_load_16_from_memory(i_File.data(), (int)i_File.size(), &d.m_Width, &d.m_Height, &d.m_Channels, 0);
		if (p)
		{
			d.m_Samples.assign(p, p + (size_t)d.m_Width * d.m_Height * d.m_Channels);
			stbi_image_free(p);
		}
		return d;
	}

	//	Largest difference between two decodes (-1 if their shapes differ)
	int max_difference(const Decoded16& i_A, const Decoded16& i_B)
	{
		if (i_A.m_Width != i_B.m_Width || i_A.m_Height != i_B.m_Height || i_A.m_Channels != i_B.m_Channels ||
			i_A.m_Samples.size() != i_B.m_Samples.size() || i_A.m_Samples.empty())
			return -1;
		int worst = 0;
		for (size_t i = 0; i < i_A.m_Samples.size(); i++)
			worst = std::max(worst, std::abs(i_A.m_Samples[i] - i_B.m_Samples[i]));
		return worst;
	}

	void expect_same_png(const Saved& i_Dx11, const Saved& i_Vulkan)
	{
		const PngChunks a = png_chunks(i_Dx11.m_File), b = png_chunks(i_Vulkan.m_File);
		EXPECT_EQ(a.m_Order, b.m_Order) << "chunks";
		for (const char* chunk : { "IHDR", "gAMA", "pHYs" })
		{
			EXPECT_TRUE(a.m_Data.count(chunk) == 1) << "DX11 has no " << chunk;
			EXPECT_EQ(a.m_Data.count(chunk) ? a.m_Data.at(chunk) : Bytes(), b.m_Data.count(chunk) ? b.m_Data.at(chunk) : Bytes()) << chunk;
		}
		const int worst = max_difference(decode_png(i_Dx11.m_File), decode_png(i_Vulkan.m_File));
		EXPECT_LE(0, worst) << "decodes differ in size or failed";
		EXPECT_LE(worst, i_Dx11.m_bFloatSource ? c_SrgbMax16 : 0) << "largest difference " << worst << "/65535";
	}

	//------------------------------------------------------------------------
	//	TIFF: the tags DX11's file has (Software and XMP aside); the
	//	scanlines read by libtiff
	//------------------------------------------------------------------------
	struct TiffRead
	{
		std::map<std::string, std::string>	m_Tags;
		Decoded16							m_Pixels;
	};

	TiffRead read_tiff(const Bytes& i_File, const std::filesystem::path& i_Path)
	{
		TiffRead out;
		std::ofstream(i_Path, std::ios::binary | std::ios::trunc).write(reinterpret_cast<const char*>(i_File.data()), (std::streamsize)i_File.size());
		TIFF* tif = TIFFOpenW(i_Path.wstring().c_str(), "r");
		if (!tif)
			return out;
		uint32_t width = 0, height = 0, rows_per_strip = 0;
		uint16_t bps = 0, spp = 0, photometric = 0, compression = 0, predictor = 1, planar = 0, unit = 0, extra_count = 0;
		uint16_t* extra = NULL;
		float xres = 0, yres = 0;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
		TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression);
		TIFFGetField(tif, TIFFTAG_PREDICTOR, &predictor);
		TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rows_per_strip);
		TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &planar);
		TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &unit);
		TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
		TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres);
		std::string extras;
		if (TIFFGetField(tif, TIFFTAG_EXTRASAMPLES, &extra_count, &extra))
		{
			for (uint16_t i = 0; i < extra_count; i++)
				extras += std::to_string(extra[i]) + " ";
		}
		out.m_Tags = {
			{ "size", std::to_string(width) + "x" + std::to_string(height) },
			{ "BitsPerSample", std::to_string(bps) },
			{ "SamplesPerPixel", std::to_string(spp) },
			{ "Photometric", std::to_string(photometric) },
			{ "Compression", std::to_string(compression) },
			{ "Predictor", std::to_string(predictor) },
			{ "RowsPerStrip", std::to_string(rows_per_strip) },
			{ "PlanarConfig", std::to_string(planar) },
			{ "ResolutionUnit", std::to_string(unit) },
			{ "Resolution", std::to_string(xres) + "," + std::to_string(yres) },
			{ "ExtraSamples", extras },
			{ "strips", std::to_string(TIFFNumberOfStrips(tif)) },
		};
		out.m_Pixels.m_Width = (int)width;
		out.m_Pixels.m_Height = (int)height;
		out.m_Pixels.m_Channels = spp;
		std::vector<uint8_t> line(TIFFScanlineSize(tif));
		for (uint32_t y = 0; y < height; y++)
		{
			if (TIFFReadScanline(tif, line.data(), y) < 0)
			{
				out.m_Pixels.m_Samples.clear();
				break;
			}
			for (uint32_t i = 0; i < width * spp; i++)
				out.m_Pixels.m_Samples.push_back(bps == 16 ? reinterpret_cast<const uint16_t*>(line.data())[i] : line[i] * 257);
		}
		TIFFClose(tif);
		return out;
	}

	void expect_same_tiff(const Saved& i_Dx11, const Saved& i_Vulkan, const std::filesystem::path& i_Dir)
	{
		const TiffRead a = read_tiff(i_Dx11.m_File, i_Dir / "dx11.tif");
		const TiffRead b = read_tiff(i_Vulkan.m_File, i_Dir / "vulkan.tif");
		ASSERT_FALSE(a.m_Tags.empty()) << "libtiff can't read DX11's file";
		ASSERT_FALSE(b.m_Tags.empty()) << "libtiff can't read Vulkan's file";
		for (const auto& tag : a.m_Tags)
			EXPECT_EQ(tag.second, b.m_Tags.at(tag.first)) << tag.first;
		const int worst = max_difference(a.m_Pixels, b.m_Pixels);
		EXPECT_LE(0, worst) << "decodes differ in size or failed";
		EXPECT_LE(worst, i_Dx11.m_bFloatSource ? c_SrgbMax16 : 0) << "largest difference " << worst << "/65535";
	}

	//------------------------------------------------------------------------
	//	JPG: the frame size and components; the decoded images within the
	//	JPG exception's tolerance
	//------------------------------------------------------------------------
	int jpg_components(const Bytes& i_File)
	{
		for (size_t p = 2; p + 9 < i_File.size(); )
		{
			if (i_File[p] != 0xff)
				return -1;
			const uint8_t marker = i_File[p + 1];
			const size_t length = ((size_t)i_File[p + 2] << 8) | i_File[p + 3];
			if (marker == 0xc0 || marker == 0xc1 || marker == 0xc2)
				return i_File[p + 9];
			p += 2 + length;
		}
		return -1;
	}

	void expect_same_jpg(const std::string& i_Name, const Saved& i_Dx11, const Saved& i_Vulkan)
	{
		EXPECT_EQ(jpg_components(i_Dx11.m_File), i_Dx11.m_bGreySource ? 1 : 3);
		EXPECT_EQ(jpg_components(i_Vulkan.m_File), 3);
		int w[2] = {}, h[2] = {}, n[2] = {};
		stbi_uc* p[2] = {
			stbi_load_from_memory(i_Dx11.m_File.data(), (int)i_Dx11.m_File.size(), &w[0], &h[0], &n[0], 4),
			stbi_load_from_memory(i_Vulkan.m_File.data(), (int)i_Vulkan.m_File.size(), &w[1], &h[1], &n[1], 4) };
		ASSERT_TRUE(p[0] && p[1]) << "stb_image can't read a JPG";
		EXPECT_EQ(w[0], w[1]);
		EXPECT_EQ(h[0], h[1]);
		if (w[0] == w[1] && h[0] == h[1])
		{
			const gtCompareResult r = gtCompareImages(p[0], p[1], w[0], h[0], c_JpgMaxErr, 1 / 255.0f, c_JpgPctOver);
			EXPECT_TRUE(r.m_bPass) << r.Describe() << "; images in " << gtSaveComparison(i_Name, p[0], p[1], w[0], h[0]);
			std::printf("[ info ] %s: JPG %s\n", i_Name.c_str(), r.Describe().c_str());
		}
		stbi_image_free(p[0]);
		stbi_image_free(p[1]);
	}

	//	A 64-bit fixed-point BMP: the header byte-identical, the samples
	//	within c_FixedMax
	void expect_same_fixed_point_bmp(const Saved& i_Dx11, const Saved& i_Vulkan)
	{
		const size_t c_Header = 54;
		ASSERT_EQ(i_Dx11.m_File.size(), i_Vulkan.m_File.size());
		ASSERT_GT(i_Dx11.m_File.size(), c_Header);
		EXPECT_TRUE(std::equal(i_Dx11.m_File.begin(), i_Dx11.m_File.begin() + c_Header, i_Vulkan.m_File.begin())) << "headers differ";
		int worst = 0;
		for (size_t i = c_Header; i + 1 < i_Dx11.m_File.size(); i += 2)
		{
			const int16_t a = (int16_t)(i_Dx11.m_File[i] | (i_Dx11.m_File[i + 1] << 8));
			const int16_t b = (int16_t)(i_Vulkan.m_File[i] | (i_Vulkan.m_File[i + 1] << 8));
			worst = std::max(worst, std::abs(a - b));
		}
		EXPECT_LE(worst, c_FixedMax) << "largest difference " << worst << "/8192";
	}

	void expect_same_file(const std::string& i_Name, const Saved& i_Dx11, const Saved& i_Vulkan, const std::filesystem::path& i_Dir)
	{
		SCOPED_TRACE(i_Name);
		EXPECT_EQ(i_Dx11.m_Outcome, i_Vulkan.m_Outcome);
		EXPECT_EQ(i_Dx11.m_bExists, i_Vulkan.m_bExists) << "a file was written on one backend only";
		if (!i_Dx11.m_bExists || !i_Vulkan.m_bExists)
			return;
		const std::string ext = extension(i_Name);
		if (ext == "bmp" && i_Dx11.m_bUnorm16Source)
		{
			expect_same_fixed_point_bmp(i_Dx11, i_Vulkan);
		}
		else if (ext == "bmp" || ext == "dds")
		{
			if (i_Dx11.m_File != i_Vulkan.m_File)
			{
				size_t first = 0;
				while (first < i_Dx11.m_File.size() && first < i_Vulkan.m_File.size() && i_Dx11.m_File[first] == i_Vulkan.m_File[first])
					first++;
				ADD_FAILURE() << "not byte-identical: DX11 " << i_Dx11.m_File.size() << " bytes, Vulkan " << i_Vulkan.m_File.size()
							  << ", first difference at byte " << first;
			}
		}
		else if (ext == "png")
		{
			expect_same_png(i_Dx11, i_Vulkan);
		}
		else if (ext == "tif")
		{
			expect_same_tiff(i_Dx11, i_Vulkan, i_Dir);
		}
		else if (ext == "jpg")
		{
			expect_same_jpg(i_Name, i_Dx11, i_Vulkan);
		}
		else
		{
			ADD_FAILURE() << "no comparison for ." << ext;
		}
	}

	//	Runs i_Save on DX11, then on Vulkan, each in its own directory, and
	//	compares what they saved
	void expect_same(const std::function<void(const std::filesystem::path&, Results&)>& i_Save)
	{
		gtTempDir dir;
		const std::filesystem::path dx11_dir = dir.GetPath() / "dx11", vulkan_dir = dir.GetPath() / "vulkan";
		std::filesystem::create_directories(dx11_dir);
		std::filesystem::create_directories(vulkan_dir);
		Results dx11, vulkan;
		if (!gtBackend::Run(gtBackend::e_DX11, [&]() { i_Save(dx11_dir, dx11); }))
			return;
		if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { i_Save(vulkan_dir, vulkan); }))
			return;
		ASSERT_FALSE(dx11.empty());
		EXPECT_EQ(dx11.size(), vulkan.size());
		for (const auto& entry : dx11)
		{
			auto found = vulkan.find(entry.first);
			if (found == vulkan.end())
			{
				ADD_FAILURE() << entry.first << ": missing on Vulkan";
				continue;
			}
			expect_same_file(entry.first, entry.second, found->second, dir.GetPath());
		}
	}

	void save_image(const Format& i_Format, const char* i_Ext, const std::filesystem::path& i_Dir, Results& o_Results)
	{
		std::unique_ptr<g2dImage> image = make_image(i_Format);
		const std::string name = std::string(i_Format.m_pName) + "." + i_Ext;
		const std::filesystem::path path = i_Dir / name;
		const std::string result = outcome([&]() { g2dImageSave::Save(locator_of(path), image.get()); });
		o_Results[name] = saved(result, path, &i_Format);
	}

	matTexture* make_texture(const Format& i_Format)
	{
		Bytes data = pixels(i_Format, true);
		return matTextureMgr::CreateTexture(c_W, c_H, &i_Format.m_PFD, false, data.data());
	}

	//	A B8G8R8A8 texture: an A8R8G8B8 DDS file, loaded
	matTexture* load_bgra8_texture(const std::filesystem::path& i_Dir)
	{
		const Bytes data = pixels(rgba8());
		Bytes file(128 + data.size(), 0);
		uint32_t* h = reinterpret_cast<uint32_t*>(file.data());
		h[0] = 0x20534444;	h[1] = 124;			h[2] = 0x100f;		h[3] = c_H;		h[4] = c_W;		h[5] = c_W * 4;
		h[19] = 32;			h[20] = 0x41;		h[22] = 32;
		h[23] = 0x00ff0000;	h[24] = 0x0000ff00;	h[25] = 0x000000ff;	h[26] = 0xff000000;	h[27] = 0x1000;
		memcpy(file.data() + 128, data.data(), data.size());
		const std::filesystem::path path = i_Dir / "source_bgra8.dds";
		std::ofstream(path, std::ios::binary).write(reinterpret_cast<const char*>(file.data()), (std::streamsize)file.size());
		return matTextureMgr::LoadTexture(locator_of(path), TEXTURE_TYPE_2D, false);
	}

	void save_texture(matTexture* i_pTexture, const std::string& i_Name, const Format* i_pFormat,
					  const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const char* ext : { "bmp", "jpg", "png", "tif", "dds", "tga" })
		{
			if ((i_pFormat->m_bTens && std::string(ext) == "jpg") || (i_pFormat->m_bWicFails && std::string(ext) != "dds"))
				continue;
			const std::string name = i_Name + "." + ext;
			const std::filesystem::path path = i_Dir / name;
			const std::string result = outcome([&]() { matTextureMgr::SaveTextureToFile(i_pTexture, locator_of(path)); });
			Saved s = saved(result, path, i_pFormat);
			if (std::string(ext) == "tga")
			{
				//	DX11 has no branch for it: nothing is written
				EXPECT_FALSE(s.m_bExists) << name;
				continue;
			}
			o_Results[name] = s;
		}
		if (i_pFormat->m_bWicFails)
			return;
		const std::string tiff = i_Name + "_rgba.tif", png = i_Name + "_rgba.png";
		o_Results[tiff] = saved(outcome([&]() { matTextureMgr::SaveTextureToRgbaTiff(i_pTexture, locator_of(i_Dir / tiff)); }), i_Dir / tiff, i_pFormat);
		o_Results[png] = saved(outcome([&]() { matTextureMgr::SaveTextureToRgbaPNG(i_pTexture, locator_of(i_Dir / png)); }), i_Dir / png, i_pFormat);
	}
}

//----------------------------------------------------------------------------
//	An 8-bit RGBA image (what a capture of a window or an LDR render target
//	gives) to each format WIC writes
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveFormatsMatchDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const char* ext : { "bmp", "png", "tif", "jpg" })
			save_image(rgba8(), ext, i_Dir, o_Results);
	});
}

//----------------------------------------------------------------------------
//	Images that aren't 8-bit RGBA (Review Focus 2): HDR captures
//	(RGBA32F, RGBA16F), 16-bit, grey and float single-channel images, BGRX;
//	the layouts DX11's WIC path gives (RG32F, which DX11 can't write
//	through WIC: SaveWiclessFormatAsDX11)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveNonRGBA8MatchesDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const Format& format : other_formats())
			for (const char* ext : { "bmp", "png", "tif", "jpg" })
				if ((!format.m_bTens || std::string(ext) != "jpg") && !format.m_bWicFails)
					save_image(format, ext, i_Dir, o_Results);
	});
}

//----------------------------------------------------------------------------
//	A format DirectXTex has no WIC pixel format for (RG32F): g2dImageSave
//	throws g2dImageSaveX for BMP, JPG, PNG and TIFF and writes nothing;
//	texture saving writes nothing to those either and throws nothing (DX11
//	traces the failure), and writes the DDS (in SaveTextureMatchesDX11).
//	Vulkan only: DX11 gives the same (measured, Plan 6 Task 2 Step 1), but
//	its traces of the failures ("... Return Error result: ...") read as
//	errors to MSBuild, which fails the post-build test run.
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveWiclessFormatAsDX11)
{
	gtTempDir dir;
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		const Format format = other_formats()[4];
		ASSERT_TRUE(format.m_bWicFails);
		std::unique_ptr<g2dImage> image = make_image(format);
		matTexture* texture = make_texture(format);
		ASSERT_TRUE(texture != NULL);
		for (const char* ext : { "bmp", "png", "tif", "jpg" })
		{
			const std::filesystem::path image_path = dir.GetPath() / (std::string("image.") + ext);
			EXPECT_EQ("g2dImageSaveX", outcome([&]() { g2dImageSave::Save(locator_of(image_path), image.get()); })) << ext;
			EXPECT_FALSE(std::filesystem::exists(image_path)) << ext;
			const std::filesystem::path texture_path = dir.GetPath() / (std::string("texture.") + ext);
			EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToFile(texture, locator_of(texture_path)); })) << ext;
			EXPECT_FALSE(std::filesystem::exists(texture_path)) << ext;
		}
		const std::filesystem::path tiff = dir.GetPath() / "rgba.tif", png = dir.GetPath() / "rgba.png";
		EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToRgbaTiff(texture, locator_of(tiff)); }));
		EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToRgbaPNG(texture, locator_of(png)); }));
		EXPECT_FALSE(std::filesystem::exists(tiff));
		EXPECT_FALSE(std::filesystem::exists(png));
		matTextureMgr::ReleaseTexture(texture);
	});
}

//----------------------------------------------------------------------------
//	Extensions g2dImageSave doesn't write (DDS only through texture saving)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveThrowsAsDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const char* ext : { "dds", "ppm", "dib", "pfm", "xyz" })
		{
			save_image(rgba8(), ext, i_Dir, o_Results);
			EXPECT_EQ("g2dUnknownImageFileTypeX", o_Results[std::string("RGBA8.") + ext].m_Outcome) << ext;
		}
	});
}

//----------------------------------------------------------------------------
//	A window's back buffer, mid-frame: BMP and PNG as an image's; TGA, HDR
//	and EXR aren't written from a window on DX11 (only from an image)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveWindowMatchesDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		gtSceneWindow window(gtBackend::Current(), 64, 48);
		g2dWindow& w = window.Window();
		std::unique_ptr<g2dImage> image = make_image(rgba8());
		w.BeginScene();
		w.MakeCurrent();
		w.Clear(g2dRGBColor(60, 70, 80));
		w.DrawImage(5, 7, *image);
		w.EndScene();
		for (const char* ext : { "bmp", "png", "tga", "hdr", "exr", "dds" })
		{
			const std::string name = std::string("window.") + ext;
			const std::filesystem::path path = i_Dir / name;
			const std::string result = outcome([&]() { g2dImageSave::Save(locator_of(path), &w); });
			Saved s = saved(result, path, &rgba8());
			if (std::string(ext) == "bmp" || std::string(ext) == "png")
			{
				o_Results[name] = s;
			}
			else
			{
				EXPECT_EQ("g2dUnknownImageFileTypeX", result) << name;
				EXPECT_FALSE(s.m_bExists) << name;
			}
		}
		window.Present();
	});
}

//----------------------------------------------------------------------------
//	Textures (plain ones of each format, a B8G8R8A8 one loaded from a DDS
//	file, and a render target) through SaveTextureToFile for each
//	extension, SaveTextureToRgbaTiff and SaveTextureToRgbaPNG (mip 0 of
//	layer 0). DDS byte-identical (RG32F only to DDS: SaveWiclessFormatAsDX11).
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveTextureMatchesDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		std::vector<Format> formats = other_formats();
		formats.insert(formats.begin(), rgba8());
		for (const Format& format : formats)
		{
			matTexture* texture = make_texture(format);
			ASSERT_TRUE(texture != NULL) << format.m_pName;
			save_texture(texture, std::string("tex_") + format.m_pName, &format, i_Dir, o_Results);
			if (&format == &formats.front())
			{
				matTexture* target = matTextureMgr::CreateRenderTargetTexture(texture, false, matTextureMgr::e_SceneTexture);
				ASSERT_TRUE(target != NULL);
				save_texture(target, "rt_RGBA8", &format, i_Dir, o_Results);
				matTextureMgr::ReleaseTexture(target);
			}
			matTextureMgr::ReleaseTexture(texture);
		}
		matTexture* bgra = load_bgra8_texture(i_Dir);
		ASSERT_TRUE(bgra != NULL);
		save_texture(bgra, "tex_BGRA8", &rgba8(), i_Dir, o_Results);
		matTextureMgr::ReleaseTexture(bgra);
	});
}

//----------------------------------------------------------------------------
//	MergeTransparentTextures: one texture copied into a target, then a
//	second multiplied over it (DX11's two blend states)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, MergeTransparentTexturesMatchesDX11)
{
	const int w = 64, h = 48;
	auto soft = [&](int i_Seed)
	{
		Bytes data((size_t)w * h * 4);
		for (int y = 0; y < h; y++)
			for (int x = 0; x < w; x++)
				for (int c = 0; c < 4; c++)
				{
					const float t = 0.5f + 0.5f * std::sin(0.11f * x * (c + 1 + i_Seed) + 0.17f * y * (2 + c) + i_Seed);
					data[((size_t)y * w + x) * 4 + c] = (uint8_t)std::lround(t * 255.0f);
				}
		return data;
	};
	const Bytes first = soft(0), second = soft(1);
	const g2dPFD c_RGBA8(0, 8, 8, 8, 16, 8, 24, 8, 32);
	gtPicture pictures[2];
	for (gtBackend::Kind kind : { gtBackend::e_DX11, gtBackend::e_Vulkan })
	{
		const bool ran = gtBackend::Run(kind, [&]()
		{
			const float clear[4] = { 0.2f, 0.3f, 0.4f, 1.0f };
			gtTarget target(kind, w, h, clear);
			matTexture* o = matTextureMgr::CreateTexture(w, h, &c_RGBA8, false, (void*)first.data());
			matTexture* i = matTextureMgr::CreateTexture(w, h, &c_RGBA8, false, (void*)second.data());
			ASSERT_TRUE(o && i);
			matTextureMgr::MergeTransparentTextures(o, i, target.Target());
			pictures[kind] = target.Read();
			matTextureMgr::ReleaseTexture(o);
			matTextureMgr::ReleaseTexture(i);
		});
		if (!ran)
			return;
	}
	ASSERT_TRUE(pictures[0].Valid() && pictures[1].Valid());
	//	the merge changes the target on DX11 (a test can pass against a stub otherwise)
	size_t changed = 0;
	for (size_t p = 0; p < pictures[0].m_Rgba.size(); p += 4)
		changed += pictures[0].m_Rgba[p] != 51 || pictures[0].m_Rgba[p + 1] != 77;
	EXPECT_GT(changed, pictures[0].m_Rgba.size() / 8) << "DX11's merge left the target as it was";
	const gtCompareResult r = gtCompareImages(pictures[0].m_Rgba.data(), pictures[1].m_Rgba.data(), w, h);
	EXPECT_TRUE(r.m_bPass) << r.Describe() << "; images in "
						   << gtSaveComparison("MergeTransparentTexturesMatchesDX11", pictures[0].m_Rgba.data(), pictures[1].m_Rgba.data(), w, h);
}
