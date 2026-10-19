/*****************************************************************************
**  g2dCaptureVkTest.cpp
**
**      Capturing frames, as MachStudio's renders do (cptrRenderUtil):
**      g2dScreenCaptureUtil::CaptureWindowToImage of a window mid-frame
**      (its swap-chain image, or its offscreen image at another render
**      resolution) and of a render target, the image's pixels as floats
**      (GetPixels, for the accumulation buffer), and the image saved as
**      a PNG (g2dImageSave), each against DX11 with both layers up
**      (gtBackend; each DX11 half in a child process, gtDx11Child).
**      Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtBackend.hpp"
#include "gtDx11Child.hpp"
#include "gtImage.hpp"
#include "gtSceneWindow.hpp"
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
#ifndef G2D_SCREENCAPTUREUTIL_HPP
#include "Graphics/G2d/g2dScreenCaptureUtil.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/Mat/matTextureMgr.hpp"
#endif
#ifndef MAT_TEXTURE_HPP
#include "Graphics/Mat/matTexture.hpp"
#endif
#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/G2d/g2dRenderTarget.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include <stb_image.h>

#include <gtest/gtest.h>

#include <cstring>
#include <memory>
#include <vector>

namespace
{
	//	What a capture gave: the image's size and pixel format, and its
	//	pixels as GetPixels' floats (bottom row first, as DX11's)
	struct Capture
	{
		int						m_Width = 0;
		int						m_Height = 0;
		int						m_BitsPerPixel = 0;
		int						m_FirstRedBit = -1;
		std::vector<float>		m_Rgba;
		std::vector<uint8_t>	m_Png;		// the saved PNG, decoded (RGBA, top row first)
		int						m_PngWidth = 0;
		int						m_PngHeight = 0;
		gtPicture				m_Presented;
	};

	//	A Capture to and from bytes, for the DX11 child process
	template <typename T> void put(std::vector<uint8_t>& io_Bytes, const std::vector<T>& i_Values)
	{
		const uint64_t n = i_Values.size();
		io_Bytes.insert(io_Bytes.end(), (const uint8_t*)&n, (const uint8_t*)&n + sizeof(n));
		io_Bytes.insert(io_Bytes.end(), (const uint8_t*)i_Values.data(), (const uint8_t*)(i_Values.data() + i_Values.size()));
	}

	template <typename T> bool get(const std::vector<uint8_t>& i_Bytes, size_t& io_At, std::vector<T>& o_Values)
	{
		uint64_t n = 0;
		if (io_At + sizeof(n) > i_Bytes.size())
			return false;
		memcpy(&n, &i_Bytes[io_At], sizeof(n));
		io_At += sizeof(n);
		if (io_At + n * sizeof(T) > i_Bytes.size())
			return false;
		o_Values.resize((size_t)n);
		memcpy(o_Values.data(), &i_Bytes[io_At], (size_t)n * sizeof(T));
		io_At += (size_t)n * sizeof(T);
		return true;
	}

	std::vector<uint8_t> to_bytes(const Capture& i_Capture)
	{
		std::vector<uint8_t> bytes;
		put(bytes, std::vector<int>{ i_Capture.m_Width, i_Capture.m_Height, i_Capture.m_BitsPerPixel, i_Capture.m_FirstRedBit,
									 i_Capture.m_PngWidth, i_Capture.m_PngHeight });
		put(bytes, i_Capture.m_Rgba);
		put(bytes, i_Capture.m_Png);
		return bytes;
	}

	Capture from_bytes(const std::vector<uint8_t>& i_Bytes)
	{
		Capture capture;
		size_t at = 0;
		std::vector<int> ints;
		if (!get(i_Bytes, at, ints) || ints.size() != 6 || !get(i_Bytes, at, capture.m_Rgba) || !get(i_Bytes, at, capture.m_Png))
		{
			ADD_FAILURE() << "no result from the DX11 half";
			return Capture();
		}
		capture.m_Width = ints[0];
		capture.m_Height = ints[1];
		capture.m_BitsPerPixel = ints[2];
		capture.m_FirstRedBit = ints[3];
		capture.m_PngWidth = ints[4];
		capture.m_PngHeight = ints[5];
		return capture;
	}

	fsLocator locator_of(const std::filesystem::path& i_Path)
	{
		fsLocator locator;
		fsFileUtil::UnicodeStringToLocator(itString(i_Path.wstring().c_str()), locator);
		return locator;
	}

	//	A 40x30 image, every pixel different, alpha too
	std::unique_ptr<g2dImage> pattern_image()
	{
		const g2dPFD rgba8(0, 8, 8, 8, 16, 8, 24, 8, 32);
		std::unique_ptr<g2dImage> image(g2dImageCreate::Make(40, 30, rgba8, g2dImage::e_VideoMemory));
		uint8_t* pixels = static_cast<uint8_t*>(image->Lock());
		const int stride = image->GetStride();
		for (int y = 0; y < 30; y++)
		{
			for (int x = 0; x < 40; x++)
			{
				uint8_t* p = pixels + y * stride + x * 4;
				p[0] = (uint8_t)(x * 6);
				p[1] = (uint8_t)(y * 8);
				p[2] = (uint8_t)((x + y) % 2 ? 255 : 40);
				p[3] = (uint8_t)(100 + x + y);
			}
		}
		image->Release();
		return image;
	}

	//	The image's size, format and pixels; and saved as a PNG, read back
	void record(g2dImage* i_pImage, const std::filesystem::path& i_Png, Capture& o_Capture)
	{
		ASSERT_TRUE(i_pImage != NULL);
		o_Capture.m_Width = i_pImage->GetWidth();
		o_Capture.m_Height = i_pImage->GetHeight();
		o_Capture.m_BitsPerPixel = i_pImage->GetPixelFormat().BitsPerPixel();
		o_Capture.m_FirstRedBit = i_pImage->GetPixelFormat().FirstRedBit();
		const size_t count = (size_t)o_Capture.m_Width * o_Capture.m_Height;
		std::vector<g2dPixelRGBA32F> pixels(count);
		i_pImage->GetPixels(pixels.data(), o_Capture.m_Width, o_Capture.m_Height);
		o_Capture.m_Rgba.resize(count * 4);
		memcpy(o_Capture.m_Rgba.data(), pixels.data(), count * sizeof(g2dPixelRGBA32F));

		g2dImageSave::Save(locator_of(i_Png), i_pImage);
		int channels = 0;
		stbi_uc* png = stbi_load(i_Png.string().c_str(), &o_Capture.m_PngWidth, &o_Capture.m_PngHeight, &channels, 4);
		ASSERT_TRUE(png != NULL) << "no PNG at " << i_Png.string();
		o_Capture.m_Png.assign(png, png + (size_t)o_Capture.m_PngWidth * o_Capture.m_PngHeight * 4);
		stbi_image_free(png);
	}

	//	A window drawn mid-frame (grey, the pattern image in it), captured
	//	before Present, as cptrRenderThread does; i_RenderW/H > 0 renders
	//	offscreen at that resolution; i_ResizeW/H > 0 resizes the g2dWindow
	//	only, not the native window (as a capture window bigger than the
	//	screen is: Windows makes its client area smaller)
	Capture capture_window(gtBackend::Kind i_Kind, const std::filesystem::path& i_Dir, int i_RenderW, int i_RenderH,
						   int i_ResizeW = 0, int i_ResizeH = 0)
	{
		Capture capture;
		gtSceneWindow window(i_Kind, 160, 120);
		g2dWindow& w = window.Window();
		if (i_ResizeW > 0)
		{
			w.ResizeWindow(i_ResizeW, i_ResizeH);
		}
		if (i_RenderW > 0)
		{
			w.SetRenderResolution(i_RenderW, i_RenderH);
		}
		std::unique_ptr<g2dImage> image = pattern_image();
		w.BeginScene();
		w.MakeCurrent();
		w.Clear(g2dRGBColor(60, 70, 80));
		w.DrawImage(10, 10, *image);
		w.EndScene();
		g2dImage* captured = NULL;
		g2dScreenCaptureUtil::CaptureWindowToImage(w, captured);
		record(captured, i_Dir / (std::string(gtBackend::Name(i_Kind)) + ".png"), capture);
		delete captured;
		capture.m_Presented = window.Present();
		return capture;
	}

	void expect_same(const Capture& i_Dx11, const Capture& i_Vulkan)
	{
		EXPECT_EQ(i_Dx11.m_Width, i_Vulkan.m_Width);
		EXPECT_EQ(i_Dx11.m_Height, i_Vulkan.m_Height);
		EXPECT_EQ(i_Dx11.m_BitsPerPixel, i_Vulkan.m_BitsPerPixel);
		EXPECT_EQ(i_Dx11.m_FirstRedBit, i_Vulkan.m_FirstRedBit);
		ASSERT_EQ(i_Dx11.m_Rgba.size(), i_Vulkan.m_Rgba.size());
		size_t differ = 0;
		for (size_t i = 0; i < i_Dx11.m_Rgba.size(); i++)
			differ += i_Dx11.m_Rgba[i] != i_Vulkan.m_Rgba[i];
		EXPECT_EQ(0u, differ) << "GetPixels floats differ";
		EXPECT_EQ(i_Dx11.m_PngWidth, i_Vulkan.m_PngWidth);
		EXPECT_EQ(i_Dx11.m_PngHeight, i_Vulkan.m_PngHeight);
		EXPECT_TRUE(i_Dx11.m_Png == i_Vulkan.m_Png) << "saved PNGs differ";
	}
}

//	The window's own frame, captured mid-frame then presented: the capture
//	is what the window shows
TEST(g2dCaptureVk, CaptureWindowToImageMatchesDX11)
{
	gtTempDir dir;
	Capture vulkan;
	const Capture dx11 = from_bytes(gtInDx11([&]() { return to_bytes(capture_window(gtBackend::e_DX11, dir.GetPath(), 0, 0)); }));
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = capture_window(gtBackend::e_Vulkan, dir.GetPath(), 0, 0); }))
		return;
	EXPECT_EQ(160, vulkan.m_Width);
	EXPECT_EQ(120, vulkan.m_Height);
	expect_same(dx11, vulkan);
	//	the capture is the presented frame: GetPixels' rows bottom first, the PNG's top first
	ASSERT_TRUE(vulkan.m_Presented.Valid());
	ASSERT_EQ(vulkan.m_Presented.m_Rgba.size(), vulkan.m_Png.size());
	size_t differ = 0;
	for (size_t i = 0; i < vulkan.m_Png.size(); i += 4)
		differ += memcmp(&vulkan.m_Png[i], &vulkan.m_Presented.m_Rgba[i], 3) != 0;
	EXPECT_EQ(0u, differ) << "the PNG isn't what the window showed";
	const size_t top_left_of_image = ((size_t)(120 - 1 - 10) * 160 + 10) * 4;	// bottom row first
	EXPECT_FLOAT_EQ(40 / 255.0f, vulkan.m_Rgba[top_left_of_image + 2]);
	EXPECT_FLOAT_EQ(100 / 255.0f, vulkan.m_Rgba[top_left_of_image + 3]);
}

//	Rendering offscreen at another resolution: the capture is the render
//	resolution, not the window's
TEST(g2dCaptureVk, CaptureOffscreenWindowMatchesDX11)
{
	gtTempDir dir;
	Capture vulkan;
	const Capture dx11 = from_bytes(gtInDx11([&]() { return to_bytes(capture_window(gtBackend::e_DX11, dir.GetPath(), 96, 64)); }));
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = capture_window(gtBackend::e_Vulkan, dir.GetPath(), 96, 64); }))
		return;
	EXPECT_EQ(96, vulkan.m_Width);
	EXPECT_EQ(64, vulkan.m_Height);
	expect_same(dx11, vulkan);
}

//	A window whose native client area isn't the size it was given (a capture
//	window bigger than the screen): DX11's back buffer is the size given
//	(ResizeBuffers), and so is what Vulkan renders and captures
TEST(g2dCaptureVk, CaptureWindowAtGivenSizeMatchesDX11)
{
	gtTempDir dir;
	Capture vulkan;
	const Capture dx11 = from_bytes(gtInDx11([&]() { return to_bytes(capture_window(gtBackend::e_DX11, dir.GetPath(), 0, 0, 200, 150)); }));
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = capture_window(gtBackend::e_Vulkan, dir.GetPath(), 0, 0, 200, 150); }))
		return;
	EXPECT_EQ(200, dx11.m_Width);
	EXPECT_EQ(150, dx11.m_Height);
	expect_same(dx11, vulkan);
}

//	A render target texture (the raw-float capture path), cleared to a colour
TEST(g2dCaptureVk, CaptureRenderTargetToImageMatchesDX11)
{
	gtTempDir dir;
	auto run = [&](gtBackend::Kind i_Kind)
	{
		Capture capture;
		matTexture* target = matTextureMgr::CreateRenderTargetTexture(48, 32, false, NULL, false, true);
		EXPECT_TRUE(target != NULL);
		if (!target)
			return capture;
		target->GetRenderTargetAPI()->Clear(maFloatRGBA(0.2f, 0.4f, 0.6f, 0.8f), true);
		g2dImage* captured = NULL;
		g2dScreenCaptureUtil::CaptureRenderTargetToImage(*target->GetRenderTargetAPI(), captured);
		record(captured, dir.GetPath() / (std::string(gtBackend::Name(i_Kind)) + "_rt.png"), capture);
		delete captured;
		matTextureMgr::ReleaseTexture(target);
		return capture;
	};
	Capture vulkan;
	const Capture dx11 = from_bytes(gtInDx11([&]() { return to_bytes(run(gtBackend::e_DX11)); }));
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = run(gtBackend::e_Vulkan); }))
		return;
	EXPECT_EQ(48, vulkan.m_Width);
	expect_same(dx11, vulkan);
}
