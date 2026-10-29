/*****************************************************************************
**  g2dImageSaveVkTest.cpp
**
**      Image and texture saving (g2dImageSave, matTextureMgr's
**      SaveTextureToFile, SaveTextureToRgbaTiff and SaveTextureToRgbaPNG)
**      and MergeTransparentTextures.
**
**      Both backends save through Graphics' g2dImageSaveUtil (an image as
**      it is; a window or a texture read back into an image by
**      g2dScreenCaptureUtil first), so what DX11 and Vulkan write for the
**      same pixels must be byte-identical: that checks that the backends
**      hand the writers the same pixels. SaveUtilDecodes checks what the
**      files hold against the source, on each backend.
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
#define STBI_ONLY_BMP
#define STBI_ONLY_TGA
#define STBI_ONLY_HDR
#define STBI_ONLY_PNM
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

	const float c_JpgMeanErr = 6 / 255.0f;		// a JPG against its source

	const int c_W = 37, c_H = 23;	// odd: BMP rows are padded

	struct Format
	{
		const char*	m_pName;
		g2dPFD		m_PFD;
		int			m_Channels;
		int			m_Bits;			// per channel
		bool		m_bFloat;
		bool		m_bDdsOnly = false;	// g2dImageSaveUtil writes it to DDS only
	};

	const Format& rgba8()
	{
		static const Format f = { "RGBA8", g2dPFD(0, 8, 8, 8, 16, 8, 24, 8, 32), 4, 8, false };
		return f;
	}

	//	The formats a capture or a texture can have
	std::vector<Format> other_formats()
	{
		return
		{
			{ "BGRX8",	 g2dPFD(g2dPFD::e_RColor, 32), 4, 8, false },
			{ "RGBA16F", g2dPFD(g2dPFD::e_RGBA16f, 64), 4, 16, true },
			{ "RGBA16U", g2dPFD(g2dPFD::e_RGBA16UInt, 64), 4, 16, false },
			{ "RGBA32F", g2dPFD(g2dPFD::e_RGBA32f, 128), 4, 32, true },
			{ "RG32F",	 g2dPFD(g2dPFD::e_GR32f, 64), 2, 32, true, true },
			{ "R16F",	 g2dPFD(g2dPFD::e_Float16, 16), 1, 16, true },
			{ "R32F",	 g2dPFD(g2dPFD::e_Float32, 32), 1, 32, true },
			{ "L8",		 g2dPFD(g2dPFD::e_Luminance8, 8), 1, 8, false },
		};
	}

	std::vector<Format> all_formats()
	{
		std::vector<Format> formats = other_formats();
		formats.insert(formats.begin(), rgba8());
		return formats;
	}

	//	The types g2dImageSaveUtil writes
	const std::vector<const char*>& extensions()
	{
		static const std::vector<const char*> c_Extensions = { "bmp", "png", "jpg", "tga", "hdr", "ppm", "pfm", "exr", "tif", "dds" };
		return c_Extensions;
	}

	//	Whether g2dImageSaveUtil writes the format to the type: everything but
	//	RG32F (DDS only); EXR only 8-bit RGBA and the half and float formats
	bool writes(const Format& i_Format, const std::string& i_Ext)
	{
		if (i_Format.m_bDdsOnly)
			return i_Ext == "dds";
		if (i_Ext == "exr")
			return i_Format.m_bFloat || std::string(i_Format.m_pName) == "RGBA8";
		return true;
	}

	//	Smooth content in [0, 1] (JPEG is kindest to smooth images)
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
	//	what is checked); i_bOpaqueX sets BGRX8's X byte to 255 (textures:
	//	Vulkan has no X8 formats, its B8G8R8A8 image keeps alpha 255)
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
					if (i_Format.m_Bits == 8)
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
	};
	typedef std::map<std::string, Saved> Results;

	Saved saved(const std::string& i_Outcome, const std::filesystem::path& i_Path)
	{
		Saved s;
		s.m_Outcome = i_Outcome;
		std::ifstream file(i_Path, std::ios::binary);
		s.m_bExists = (bool)file;
		if (file)
			s.m_File.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		return s;
	}

	fsLocator locator_of(const std::filesystem::path& i_Path)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(i_Path.u8string(), locator);
		return locator;
	}

	//------------------------------------------------------------------------
	//	What g2dImageSaveUtil should read from a test image (pixels()): RGBA
	//	floats, top row first (grey in R, G and B; BGRX's alpha 1)
	//------------------------------------------------------------------------
	std::vector<float> source_rgba(const Format& i_Format)
	{
		const Bytes data = pixels(i_Format);
		const int bytes = i_Format.m_Channels * i_Format.m_Bits / 8;
		const bool bgrx = i_Format.m_PFD.GetPixelFormat() == g2dPFD::e_RColor;
		std::vector<float> out((size_t)c_W * c_H * 4);
		for (size_t i = 0; i < (size_t)c_W * c_H; i++)
		{
			const uint8_t* p = data.data() + i * bytes;
			float v[4] = { 0, 0, 0, 1 };
			for (int c = 0; c < i_Format.m_Channels; c++)
			{
				if (i_Format.m_Bits == 8)
					v[c] = p[c] / 255.0f;
				else if (!i_Format.m_bFloat)
					v[c] = reinterpret_cast<const uint16_t*>(p)[c] / 65535.0f;
				else if (i_Format.m_Bits == 16)
					v[c] = maFunctions::HalfToFloat(reinterpret_cast<const uint16_t*>(p)[c]);
				else
					v[c] = reinterpret_cast<const float*>(p)[c];
			}
			if (i_Format.m_Channels == 1)
				v[1] = v[2] = v[0];
			if (bgrx)
			{
				std::swap(v[0], v[2]);
				v[3] = 1.0f;
			}
			for (int c = 0; c < 4; c++)
				out[i * 4 + c] = v[c];
		}
		return out;
	}

	//	An integer file's sample (i_Max 255 or 65535): floats (linear)
	//	through the sRGB curve, alpha linear
	int expected_sample(float i_V, bool i_bFloatSource, int i_Channel, double i_Max)
	{
		double v = i_V;
		if (i_bFloatSource && i_Channel < 3)
		{
			v = std::min(1.0, std::max(0.0, v));
			v = v <= 0.0031308 ? v * 12.92 : 1.055 * std::pow(v, 1.0 / 2.4) - 0.055;
		}
		return (int)std::floor(std::min(1.0, std::max(0.0, v)) * i_Max + 0.5);
	}

	//	A PFM file's floats (RGB or grey), top row first; empty if it isn't one
	std::vector<float> read_pfm(const Bytes& i_File, int& o_Channels)
	{
		const std::string text(i_File.begin(), i_File.begin() + std::min<size_t>(i_File.size(), 64));
		const std::string header = std::string(text[1] == 'f' ? "Pf\n" : "PF\n") + std::to_string(c_W) + " " + std::to_string(c_H) + "\n-1.0\n";
		o_Channels = text[1] == 'f' ? 1 : 3;
		const size_t row = (size_t)c_W * o_Channels;
		if (text.compare(0, header.size(), header) != 0 || i_File.size() != header.size() + row * c_H * sizeof(float))
			return std::vector<float>();
		std::vector<float> out(row * c_H);
		for (int y = 0; y < c_H; y++)
			memcpy(&out[y * row], &i_File[header.size() + (c_H - 1 - y) * row * sizeof(float)], row * sizeof(float));
		return out;
	}

	//	A TIFF's samples as stored (8 or 16 bits), read by libtiff
	struct TiffRead
	{
		int					m_Width = 0, m_Height = 0, m_Channels = 0, m_Bits = 0;
		std::vector<int>	m_Samples;
	};

	TiffRead read_tiff(const std::filesystem::path& i_Path)
	{
		TiffRead out;
		TIFF* tif = TIFFOpenW(i_Path.wstring().c_str(), "r");
		if (!tif)
			return out;
		uint32_t width = 0, height = 0;
		uint16_t bps = 0, spp = 0;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &width);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &height);
		TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
		TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
		out.m_Width = (int)width;
		out.m_Height = (int)height;
		out.m_Channels = spp;
		out.m_Bits = bps;
		std::vector<uint8_t> line(TIFFScanlineSize(tif));
		for (uint32_t y = 0; y < height; y++)
		{
			if (TIFFReadScanline(tif, line.data(), y) < 0)
			{
				out.m_Samples.clear();
				break;
			}
			for (uint32_t i = 0; i < width * spp; i++)
				out.m_Samples.push_back(bps == 16 ? reinterpret_cast<const uint16_t*>(line.data())[i] : line[i]);
		}
		TIFFClose(tif);
		return out;
	}

	//------------------------------------------------------------------------
	//	Decodes what g2dImageSaveUtil wrote for i_Format as i_Ext (at
	//	i_Path) and checks it against the source
	//------------------------------------------------------------------------
	void expect_decodes(const Format& i_Format, const std::string& i_Ext, const std::filesystem::path& i_Path)
	{
		SCOPED_TRACE(std::string(i_Format.m_pName) + "." + i_Ext);
		const Saved file = saved("ok", i_Path);
		ASSERT_TRUE(file.m_bExists) << "nothing written";
		const Bytes& bytes = file.m_File;
		const std::vector<float> source = source_rgba(i_Format);
		const bool alpha = i_Format.m_Channels == 4 && i_Format.m_PFD.GetPixelFormat() != g2dPFD::e_RColor;
		const size_t count = (size_t)c_W * c_H;
		if (i_Ext == "dds")
		{
			//	the pixels as stored, after the 128-byte header
			const Bytes data = pixels(i_Format);
			ASSERT_EQ(128 + data.size(), bytes.size());
			EXPECT_EQ(0, memcmp(bytes.data(), "DDS ", 4));
			EXPECT_EQ((uint32_t)c_H, *reinterpret_cast<const uint32_t*>(&bytes[12]));
			EXPECT_EQ((uint32_t)c_W, *reinterpret_cast<const uint32_t*>(&bytes[16]));
			EXPECT_TRUE(std::equal(data.begin(), data.end(), bytes.begin() + 128)) << "pixels differ";
			return;
		}
		if (i_Ext == "tif")
		{
			const TiffRead got = read_tiff(i_Path);
			const int channels = i_Format.m_Channels == 1 ? 1 : (alpha ? 4 : 3);
			const bool eight = i_Format.m_Bits == 8;
			EXPECT_EQ(c_W, got.m_Width);
			EXPECT_EQ(c_H, got.m_Height);
			EXPECT_EQ(channels, got.m_Channels);
			EXPECT_EQ(eight ? 8 : 16, got.m_Bits);
			ASSERT_EQ(count * channels, got.m_Samples.size()) << "libtiff can't read it";
			int worst = 0;
			for (size_t i = 0; i < count; i++)
				for (int c = 0; c < channels; c++)
					worst = std::max(worst, std::abs(got.m_Samples[i * channels + c] -
													 expected_sample(source[i * 4 + c], i_Format.m_bFloat, c, eight ? 255.0 : 65535.0)));
			EXPECT_LE(worst, eight ? 0 : 1) << "largest difference";
			return;
		}
		if (i_Ext == "pfm")
		{
			int channels = 0;
			const std::vector<float> got = read_pfm(bytes, channels);
			ASSERT_FALSE(got.empty()) << "not a " << c_W << "x" << c_H << " PFM";
			EXPECT_EQ(channels, i_Format.m_Channels == 1 ? 1 : 3);
			int wrong = 0;
			for (size_t i = 0; i < count; i++)
				for (int c = 0; c < channels; c++)
					wrong += got[i * channels + c] != source[i * 4 + c];
			EXPECT_EQ(0, wrong) << "samples differ";
			return;
		}
		if (i_Ext == "hdr")
		{
			int w = 0, h = 0, n = 0;
			float* got = stbi_loadf_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &n, 3);
			ASSERT_TRUE(got != NULL) << "stb_image can't read it";
			EXPECT_EQ(c_W, w);
			EXPECT_EQ(c_H, h);
			//	RGBE: 8-bit mantissas sharing the largest channel's exponent
			float worst = 0;
			for (size_t i = 0; i < count && w == c_W && h == c_H; i++)
			{
				const float s[3] = { std::max(0.0f, source[i * 4]), std::max(0.0f, source[i * 4 + 1]), std::max(0.0f, source[i * 4 + 2]) };
				const float largest = std::max(s[0], std::max(s[1], s[2]));
				for (int c = 0; c < 3; c++)
					worst = std::max(worst, std::abs(got[i * 3 + c] - s[c]) / std::max(largest, 1e-6f));
			}
			EXPECT_LE(worst, 1 / 128.0f) << "largest error, relative to the pixel's largest channel";
			stbi_image_free(got);
			return;
		}
		int w = 0, h = 0, n = 0;
		stbi_uc* got = stbi_load_from_memory(bytes.data(), (int)bytes.size(), &w, &h, &n, 4);
		ASSERT_TRUE(got != NULL) << "stb_image can't read it";
		EXPECT_EQ(c_W, w);
		EXPECT_EQ(c_H, h);
		const bool file_alpha = alpha && (i_Ext == "png" || i_Ext == "bmp" || i_Ext == "tga");
		std::vector<uint8_t> expected(count * 4);
		for (size_t i = 0; i < expected.size(); i++)
			expected[i] = (uint8_t)((i % 4 == 3 && !file_alpha) ? 255 : expected_sample(source[i], i_Format.m_bFloat, (int)(i % 4), 255.0));
		if (w == c_W && h == c_H)
		{
			if (i_Ext == "jpg")
			{
				//	Lossy (4:2:0, quality 90) on a small image: the mean error
				//	only (a swapped channel or flipped image is far above it;
				//	measured up to 4.7/255, on RGBA16F)
				const gtCompareResult r = gtCompareImages(expected.data(), got, w, h);
				EXPECT_LE(r.m_MeanErr, c_JpgMeanErr) << r.Describe();
			}
			else
			{
				//	8-bit sources exactly; others within 1 (the writer's float
				//	arithmetic against this double one)
				const int tolerance = (i_Format.m_Bits == 8) ? 0 : 1;
				int worst = 0;
				for (size_t i = 0; i < expected.size(); i++)
					worst = std::max(worst, std::abs((int)expected[i] - (int)got[i]));
				EXPECT_LE(worst, tolerance) << "largest difference " << worst << "/255";
			}
		}
		stbi_image_free(got);
	}

	//------------------------------------------------------------------------
	//	DX11 against Vulkan: the same outcome, and the same file
	//------------------------------------------------------------------------
	void expect_same_file(const std::string& i_Name, const Saved& i_Dx11, const Saved& i_Vulkan)
	{
		SCOPED_TRACE(i_Name);
		EXPECT_EQ(i_Dx11.m_Outcome, i_Vulkan.m_Outcome);
		EXPECT_EQ(i_Dx11.m_bExists, i_Vulkan.m_bExists) << "a file was written on one backend only";
		if (i_Dx11.m_File != i_Vulkan.m_File)
		{
			size_t first = 0;
			while (first < i_Dx11.m_File.size() && first < i_Vulkan.m_File.size() && i_Dx11.m_File[first] == i_Vulkan.m_File[first])
				first++;
			ADD_FAILURE() << "not byte-identical: DX11 " << i_Dx11.m_File.size() << " bytes, Vulkan " << i_Vulkan.m_File.size()
						  << ", first difference at byte " << first;
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
			expect_same_file(entry.first, entry.second, found->second);
		}
	}

	//	i_bKnownType: an extension g2dImageSaveUtil writes (checks the save
	//	writes a file exactly when it writes the format to it)
	void save_image(const Format& i_Format, const char* i_Ext, const std::filesystem::path& i_Dir, Results& o_Results,
					bool i_bKnownType = true)
	{
		std::unique_ptr<g2dImage> image = make_image(i_Format);
		const std::string name = std::string(i_Format.m_pName) + "." + i_Ext;
		const std::filesystem::path path = i_Dir / name;
		o_Results[name] = saved(outcome([&]() { g2dImageSave::Save(locator_of(path), image.get()); }), path);
		if (!i_bKnownType)
			return;
		EXPECT_EQ(writes(i_Format, i_Ext), o_Results[name].m_bExists) << name;
		EXPECT_EQ(writes(i_Format, i_Ext) ? "ok" : "g2dImageSaveX", o_Results[name].m_Outcome) << name;
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

	//	Saves the texture to every type, and with SaveTextureToRgbaTiff and
	//	SaveTextureToRgbaPNG. i_Captured: the format it reads back as (B8G8R8A8
	//	and B8G8R8X8 become RGBA8); i_bDecode: check the files hold the
	//	pixels i_Captured's source has (an RGBA8 texture's do)
	void save_texture(matTexture* i_pTexture, const std::string& i_Name, const Format& i_Captured,
					  const std::filesystem::path& i_Dir, Results& o_Results, bool i_bDecode = false)
	{
		for (const char* ext : extensions())
		{
			const std::string name = i_Name + "." + ext;
			const std::filesystem::path path = i_Dir / name;
			o_Results[name] = saved(outcome([&]() { matTextureMgr::SaveTextureToFile(i_pTexture, locator_of(path)); }), path);
			EXPECT_EQ("ok", o_Results[name].m_Outcome) << name;
			EXPECT_EQ(writes(i_Captured, ext), o_Results[name].m_bExists) << name;
			if (i_bDecode && std::string(ext) != "exr")
				expect_decodes(i_Captured, ext, path);
		}
		const std::string tiff = i_Name + "_rgba.tif", png = i_Name + "_rgba.png";
		o_Results[tiff] = saved(outcome([&]() { matTextureMgr::SaveTextureToRgbaTiff(i_pTexture, locator_of(i_Dir / tiff)); }), i_Dir / tiff);
		o_Results[png] = saved(outcome([&]() { matTextureMgr::SaveTextureToRgbaPNG(i_pTexture, locator_of(i_Dir / png)); }), i_Dir / png);
		EXPECT_EQ(!i_Captured.m_bDdsOnly, o_Results[tiff].m_bExists) << tiff;
		EXPECT_EQ(!i_Captured.m_bDdsOnly, o_Results[png].m_bExists) << png;
	}
}

//----------------------------------------------------------------------------
//	Images of each format to each type
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveImageMatchesDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const Format& format : all_formats())
			for (const char* ext : extensions())
				save_image(format, ext, i_Dir, o_Results);
	});
}

//----------------------------------------------------------------------------
//	g2dImageSaveUtil's files hold the source's pixels: 8-bit files exactly
//	(floats through the sRGB curve; JPG within c_JpgMeanErr), 16-bit TIFF
//	within 1, HDR within RGBE's precision, PFM and DDS exactly; on each
//	backend (EXR: g2dScanlineOpenEXRUtil, not decoded here)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveUtilDecodes)
{
	for (gtBackend::Kind kind : { gtBackend::e_DX11, gtBackend::e_Vulkan })
	{
		SCOPED_TRACE(kind == gtBackend::e_DX11 ? "DX11" : "Vulkan");
		gtTempDir dir;
		const bool ran = gtBackend::Run(kind, [&]()
		{
			for (const Format& format : all_formats())
			{
				std::unique_ptr<g2dImage> image = make_image(format);
				for (const char* ext : extensions())
				{
					if (std::string(ext) == "exr" || (format.m_bDdsOnly && std::string(ext) != "dds"))
						continue;
					const std::filesystem::path path = dir.GetPath() / (std::string(format.m_pName) + "." + ext);
					EXPECT_EQ("ok", outcome([&]() { g2dImageSave::Save(locator_of(path), image.get()); })) << path;
					expect_decodes(format, ext, path);
				}
			}
		});
		if (!ran)
			return;
	}
}

//----------------------------------------------------------------------------
//	A format g2dImageSaveUtil writes to DDS only (RG32F): an image save
//	throws g2dImageSaveX and writes nothing; a texture save writes nothing
//	and throws nothing (a file that can't be written is traced)
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveDdsOnlyFormat)
{
	for (gtBackend::Kind kind : { gtBackend::e_DX11, gtBackend::e_Vulkan })
	{
		SCOPED_TRACE(kind == gtBackend::e_DX11 ? "DX11" : "Vulkan");
		gtTempDir dir;
		const bool ran = gtBackend::Run(kind, [&]()
		{
			const Format format = other_formats()[4];
			ASSERT_TRUE(format.m_bDdsOnly);
			std::unique_ptr<g2dImage> image = make_image(format);
			matTexture* texture = make_texture(format);
			ASSERT_TRUE(texture != NULL);
			for (const char* ext : extensions())
			{
				const bool dds = std::string(ext) == "dds";
				const std::filesystem::path image_path = dir.GetPath() / (std::string("image.") + ext);
				EXPECT_EQ(dds ? "ok" : "g2dImageSaveX", outcome([&]() { g2dImageSave::Save(locator_of(image_path), image.get()); })) << ext;
				EXPECT_EQ(dds, std::filesystem::exists(image_path)) << ext;
				const std::filesystem::path texture_path = dir.GetPath() / (std::string("texture.") + ext);
				EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToFile(texture, locator_of(texture_path)); })) << ext;
				EXPECT_EQ(dds, std::filesystem::exists(texture_path)) << ext;
			}
			const std::filesystem::path tiff = dir.GetPath() / "rgba.tif", png = dir.GetPath() / "rgba.png";
			EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToRgbaTiff(texture, locator_of(tiff)); }));
			EXPECT_EQ("ok", outcome([&]() { matTextureMgr::SaveTextureToRgbaPNG(texture, locator_of(png)); }));
			EXPECT_FALSE(std::filesystem::exists(tiff));
			EXPECT_FALSE(std::filesystem::exists(png));
			matTextureMgr::ReleaseTexture(texture);
		});
		if (!ran)
			return;
	}
}

//----------------------------------------------------------------------------
//	Extensions g2dImageSave doesn't write
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveThrowsAsDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		for (const char* ext : { "dib", "xyz" })
		{
			save_image(rgba8(), ext, i_Dir, o_Results, false);
			EXPECT_FALSE(o_Results[std::string("RGBA8.") + ext].m_bExists) << ext;
			EXPECT_EQ("g2dUnknownImageFileTypeX", o_Results[std::string("RGBA8.") + ext].m_Outcome) << ext;
		}
	});
}

//----------------------------------------------------------------------------
//	A window's back buffer, mid-frame: captured to an image and saved as
//	one, so to every type an image is saved to
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
		for (const char* ext : extensions())
		{
			const std::string name = std::string("window.") + ext;
			const std::filesystem::path path = i_Dir / name;
			o_Results[name] = saved(outcome([&]() { g2dImageSave::Save(locator_of(path), &w); }), path);
			EXPECT_EQ("ok", o_Results[name].m_Outcome) << name;
		}
		window.Present();
	});
}

//----------------------------------------------------------------------------
//	Textures (plain ones of each format, a B8G8R8A8 one loaded from a DDS
//	file, and a render target) through SaveTextureToFile for each type,
//	SaveTextureToRgbaTiff and SaveTextureToRgbaPNG (mip 0 of layer 0). The
//	RGBA8 ones' files are decoded too.
//----------------------------------------------------------------------------
TEST(g2dImageSaveVkTest, SaveTextureMatchesDX11)
{
	expect_same([](const std::filesystem::path& i_Dir, Results& o_Results)
	{
		const std::vector<Format> formats = all_formats();
		for (const Format& format : formats)
		{
			matTexture* texture = make_texture(format);
			ASSERT_TRUE(texture != NULL) << format.m_pName;
			const bool rgba = &format == &formats.front();
			const bool bgrx = format.m_PFD.GetPixelFormat() == g2dPFD::e_RColor;
			save_texture(texture, std::string("tex_") + format.m_pName, bgrx ? rgba8() : format, i_Dir, o_Results, rgba);
			if (rgba)
			{
				matTexture* target = matTextureMgr::CreateRenderTargetTexture(texture, false, matTextureMgr::e_SceneTexture);
				ASSERT_TRUE(target != NULL);
				save_texture(target, "rt_RGBA8", rgba8(), i_Dir, o_Results, true);
				matTextureMgr::ReleaseTexture(target);
			}
			matTextureMgr::ReleaseTexture(texture);
		}
		matTexture* bgra = load_bgra8_texture(i_Dir);
		ASSERT_TRUE(bgra != NULL);
		save_texture(bgra, "tex_BGRA8", rgba8(), i_Dir, o_Results);
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
