/*****************************************************************************
**  g2dWindowVkTest.cpp
**
**      g2dWindowVk: surface and swap chain on a (hidden) native window:
**      clear and present, resize including to nothing, two windows, and
**      the GPU timer for g3dProfiler. Then windows that render scenes,
**      with the whole layer up (gtBackend): a scene in a window equals the
**      same scene in a render-target texture, survives resizes and a 0x0
**      window, renders at another resolution (scaled into the window),
**      and DrawRect / DrawImage match DX11's window. Skipped without a
**      Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtVulkanContext.hpp"
#include "gtBackend.hpp"
#include "gtImage.hpp"
#include "gtScene.hpp"
#include "gtTarget.hpp"
#include "gtSceneWindow.hpp"
#include "gtPlatform.hpp"

#ifndef GVK_G2DWINDOWVK_HPP
#include "GraphicsVulkan/G2d/g2dWindowVk.hpp"
#endif
#ifndef GVK_G3DPROFILERGPUVK_HPP
#include "GraphicsVulkan/G3d/g3dProfilerGpuVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif
#ifndef G2D_SYSTEM_HPP
#include "Graphics/G2d/g2dSystem.hpp"
#endif
#ifndef G2D_IMAGE_HPP
#include "Graphics/G2d/g2dImage.hpp"
#endif
#ifndef G2D_IMAGECREATE_HPP
#include "Graphics/G2d/g2dImageCreate.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G3D_LIGHTMGR_HPP
#include "Graphics/G3d/g3dLightMgr.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef EFF_PHONGDATA_HPP
#include "Graphics/Eff/effPhongData.hpp"
#endif
#ifndef GRAPHICSDX11_LAYER_HPP
#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#endif
#ifndef G2D_WINDOWDX11_HPP
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#endif
#ifndef G2D_DX11GLOBALWIN_HPP
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#endif
#ifndef G3D_SCENERENDERERDX11_HPP
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#endif
#ifndef GVK_GRAPHICSVULKANLAYER_HPP
#include "GraphicsVulkan/GraphicsVulkanLayer.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERERVK_HPP
#include "GraphicsVulkan/G3d/g3dSceneRendererVk.hpp"
#endif
#ifndef G3D_SCENERENDERERCREATE_HPP
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#endif
#include "gtTempDir.hpp"

#include <cstring>
#include <fstream>
#include <memory>
#include <string>

using namespace gvk;

namespace
{
	class gtWindow : public gtVulkanContext
	{
		protected:
			void TearDown() override
			{
				m_Windows.clear();
				for (void* hwnd : m_Native)
				{
					gtPlatform::DestroyHiddenWindow(hwnd);
				}
				m_Native.clear();
				gtVulkanContext::TearDown();
			}

			g2dWindowVk& window(int i_Width, int i_Height)
			{
				void* hwnd = gtPlatform::CreateHiddenWindow(i_Width, i_Height);
				m_Native.push_back(hwnd);
				m_Windows.emplace_back(new g2dWindowVk(Device(), hwnd, false));
				return *m_Windows.back();
			}

			std::vector<void*> m_Native;
			std::vector<std::unique_ptr<g2dWindowVk>> m_Windows;
	};
}

TEST_F(gtWindow, ClearsAndPresents)
{
	g2dWindowVk& w = window(320, 240);
	int width = 0, height = 0;
	w.GetDimensions(width, height);
	EXPECT_EQ(320, width);
	EXPECT_EQ(240, height);
	EXPECT_TRUE(w.IsOpen());
	EXPECT_TRUE(w.GetHasDepthBuffer());

	for (int i = 0; i < 5; ++i)
	{
		if (i == 4)
		{
			w.DebugReadBackNextFrame();
		}
		w.BeginScene();
		w.Clear(g2dRGBColor(255, 0, 0));
		w.EndScene();
		w.Present();
	}
	const std::vector<uint8_t>& pixels = w.DebugPixels();
	ASSERT_EQ(320u * 240u * 4u, pixels.size());
	const bool bgra = w.SwapchainFormat() == VK_FORMAT_B8G8R8A8_UNORM;
	for (size_t i = 0; i < pixels.size(); i += 4)
	{
		ASSERT_EQ(255, pixels[i + (bgra ? 2 : 0)]) << "pixel " << i / 4;
		ASSERT_EQ(0, pixels[i + 1]);
		ASSERT_EQ(0, pixels[i + (bgra ? 0 : 2)]);
	}
}

TEST_F(gtWindow, ResizeAndZeroSizeAreClean)
{
	g2dWindowVk& w = window(320, 240);
	auto frame = [&w]()
	{
		w.BeginScene();
		w.Clear(g2dRGBColor(0, 128, 0));
		w.EndScene();
		w.Present();
	};
	frame();

	gtPlatform::ResizeHiddenWindow(w.GetHandle(), 640, 480);
	w.ResizeWindow(640, 480);
	frame();
	int width = 0, height = 0;
	w.GetDimensions(width, height);
	EXPECT_EQ(640, width);
	EXPECT_EQ(480, height);

	//	minimized: nothing to present into; must not fail
	gtPlatform::ResizeHiddenWindow(w.GetHandle(), 0, 0);
	w.ResizeWindow(0, 0);
	frame();
	frame();

	gtPlatform::ResizeHiddenWindow(w.GetHandle(), 200, 100);
	w.ResizeWindow(200, 100);
	w.DebugReadBackNextFrame();
	frame();
	EXPECT_EQ(200u * 100u * 4u, w.DebugPixels().size());
}

TEST_F(gtWindow, TwoWindowsInterleaved)
{
	g2dWindowVk& a = window(160, 120);
	g2dWindowVk& b = window(200, 150);
	for (int i = 0; i < 6; ++i)
	{
		g2dWindowVk& w = (i % 2) ? b : a;
		w.BeginScene();
		w.Clear(g2dRGBColor(0, 0, static_cast<uint8_t>(40 * i)));
		w.EndScene();
		w.Present();
	}
}

#ifdef MSP_GFX_PROFILING
TEST_F(gtVulkan, GpuTimerMeasuresClear)
{
	gtTempDir dir;
	const std::string csv = (dir.GetPath() / "p.csv").string();
	g3dProfilerGpuVk gpu(Device());
	g3dProfiler::SetGpu(&gpu);
	g3dProfiler::Start(csv, "vulkan");

	std::unique_ptr<vkdImage> image = vkdImage::Create2D(Device(), 2048, 2048, VK_FORMAT_R8G8B8A8_UNORM,
		VK_IMAGE_USAGE_TRANSFER_DST_BIT, "GpuTimer");
	{
		G3D_PROFILE_FRAME_SCOPE("Target", 0.0f);
		G3D_PROFILE_SCOPE("Clears");
		for (int i = 0; i < 16; ++i)
		{
			VkClearColorValue color = {};
			color.float32[0] = i / 16.0f;
			VkImageSubresourceRange range = vkdRange();
			vkdFullBarrier(Device().Commands());
			vkCmdClearColorImage(Device().Commands(), image->Image(), VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
		}
	}
	g3dProfiler::Stop();
	g3dProfiler::SetGpu(NULL);

	std::ifstream in(csv);
	std::string header, root, clears;
	std::getline(in, header);
	std::getline(in, root);
	std::getline(in, clears);
	//	backend,frame,sim_time,path,depth,cpu_ms,gpu_ms,draws
	auto field = [](const std::string& i_Line, int i_Index)
	{
		size_t start = 0;
		for (int i = 0; i < i_Index; ++i)
		{
			start = i_Line.find(',', start) + 1;
		}
		return i_Line.substr(start, i_Line.find(',', start) - start);
	};
	EXPECT_EQ("vulkan", field(clears, 0));
	EXPECT_EQ("Target/Clears", field(clears, 3));
	ASSERT_FALSE(field(clears, 6).empty()) << clears;
	EXPECT_GT(std::stod(field(clears, 6)), 0.0) << clears;
	EXPECT_GE(std::stod(field(root, 6)), std::stod(field(clears, 6)));
}
#endif

//============================================================================
//	Windows that render scenes, with the whole layer up (gtBackend)
//============================================================================
namespace
{
	//	Two lit Phong spheres
	void spheres(gtSceneBuilder& io_Builder)
	{
		g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.15f, 0.2f, 1));
		io_Builder.PointLight(maPoint3d(2, 3, 4), maFloatRGBA(0.9f, 0.8f, 0.7f, 1), 25.0f);
		matMaterial* phong = io_Builder.Keep(gtScene::DataMaterial("Phong.fx", [](effShaderData* i_pData)
		{
			gtScene::SetPhong(i_pData, gtScene::e_NonDefault);
			if (effPhongData* data = dynamic_cast<effPhongData*>(i_pData))
				data->m_Transparency = 1.0f;
		}));
		io_Builder.Add(gtScene::Sphere(16, 32), phong, gtScene::Place(-0.6f, 0.2f, 0, 0.8f));
		io_Builder.Add(gtScene::Sphere(16, 32), phong, gtScene::Place(0.7f, -0.3f, 0.5f, 0.6f));
	}

	//	One frame of the scene into io_rWindow: black with depth 1, then the
	//	forward renderer (which clears the colour again, as DX11's)
	void render_frame(g2dWindow& io_rWindow, g3dSceneRenderer& io_rRenderer, gtSceneBuilder& io_rBuilder)
	{
		io_rWindow.BeginScene();
		io_rWindow.MakeCurrent();
		io_rWindow.Clear(g2dRGBColor(0, 0, 0));
		const std::vector<g3dLayer*> no_layers;
		io_rRenderer.Render(&io_rWindow, io_rBuilder.Camera(), io_rBuilder.Scene(), no_layers, 1.25f);
		io_rWindow.EndScene();
	}

	//	How many pixels of i_rPicture aren't black
	int lit_pixels(const gtPicture& i_rPicture)
	{
		int lit = 0;
		for (size_t i = 0; i + 3 < i_rPicture.m_Rgba.size(); i += 4)
			lit += (i_rPicture.m_Rgba[i] | i_rPicture.m_Rgba[i + 1] | i_rPicture.m_Rgba[i + 2]) != 0;
		return lit;
	}

	bool same_rgb(const gtPicture& i_rPicture, int i_X, int i_Y, uint8_t i_R, uint8_t i_G, uint8_t i_B)
	{
		const uint8_t* p = &i_rPicture.m_Rgba[((size_t)i_Y * i_rPicture.m_Width + i_X) * 4];
		return p[0] == i_R && p[1] == i_G && p[2] == i_B;
	}
}

TEST(g2dWindowVkScene, SceneInWindowMatchesOffscreen)
{
	const int size = 256;
	gtPicture offscreen, windowed;
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		gtSceneBuilder builder;
		spheres(builder);
		gvk::g3dSceneRendererVk renderer;
		{
			const float black[4] = { 0, 0, 0, 1 };
			gtTarget target(gtBackend::e_Vulkan, size, size, black);
			const std::vector<g3dLayer*> no_layers;
			renderer.Render(target.Target(), builder.Camera(), builder.Scene(), no_layers, 1.25f);
			offscreen = target.Read();
		}
		gtSceneWindow window(gtBackend::e_Vulkan, size, size);
		render_frame(window.Window(), renderer, builder);
		windowed = window.Present();
		renderer.ReleaseResources();
	});
	if (::testing::Test::IsSkipped())
		return;
	ASSERT_TRUE(offscreen.Valid());
	ASSERT_TRUE(windowed.Valid());
	ASSERT_EQ(size, windowed.m_Width);
	EXPECT_GT(lit_pixels(offscreen), size * size / 20) << "the scene drew almost nothing";
	const gtCompareResult result = gtCompareImages(offscreen.m_Rgba.data(), windowed.m_Rgba.data(), size, size);
	std::cout << "  SceneInWindowMatchesOffscreen: " << result.Describe() << std::endl;
	if (!result.m_bPass)
	{
		ADD_FAILURE() << result.Describe() << " (images in "
			<< gtSaveComparison("SceneInWindowMatchesOffscreen", offscreen.m_Rgba.data(), windowed.m_Rgba.data(), size, size) << ")";
	}
}

//	MachStudio's viewport draws through the HDR renderer into the window's
//	swap-chain image, which the renderer also reads (DX11 makes its swap
//	chain with DXGI_USAGE_SHADER_INPUT)
TEST(g2dWindowVkScene, HDRSceneInWindowMatchesOffscreen)
{
	const int size = 256;
	gtPicture offscreen, windowed;
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		gtSceneBuilder builder;
		spheres(builder);
		std::unique_ptr<g3dSceneRenderer> renderer(g3dSceneRendererCreate::CreateHDRRenderer());
		{
			const float black[4] = { 0, 0, 0, 1 };
			gtTarget target(gtBackend::e_Vulkan, size, size, black);
			const std::vector<g3dLayer*> no_layers;
			renderer->Render(target.Target(), builder.Camera(), builder.Scene(), no_layers, 1.25f);
			offscreen = target.Read();
		}
		gtSceneWindow window(gtBackend::e_Vulkan, size, size);
		render_frame(window.Window(), *renderer, builder);
		windowed = window.Present();
		renderer->ReleaseResources();
	});
	if (::testing::Test::IsSkipped())
		return;
	ASSERT_TRUE(offscreen.Valid());
	ASSERT_TRUE(windowed.Valid());
	ASSERT_EQ(size, windowed.m_Width);
	EXPECT_GT(lit_pixels(offscreen), size * size / 20) << "the scene drew almost nothing";
	const gtCompareResult result = gtCompareImages(offscreen.m_Rgba.data(), windowed.m_Rgba.data(), size, size);
	std::cout << "  HDRSceneInWindowMatchesOffscreen: " << result.Describe() << std::endl;
	if (!result.m_bPass)
	{
		ADD_FAILURE() << result.Describe() << " (images in "
			<< gtSaveComparison("HDRSceneInWindowMatchesOffscreen", offscreen.m_Rgba.data(), windowed.m_Rgba.data(), size, size) << ")";
	}
}

TEST(g2dWindowVkScene, SceneInWindowResizes)
{
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		gtSceneBuilder builder;
		spheres(builder);
		gvk::g3dSceneRendererVk renderer;
		gtSceneWindow window(gtBackend::e_Vulkan, 200, 150);
		auto frame = [&]()
		{
			render_frame(window.Window(), renderer, builder);
			return window.Present();
		};
		gtPicture picture = frame();
		EXPECT_EQ(200u * 150u * 4u, picture.m_Rgba.size());
		EXPECT_GT(lit_pixels(picture), 0);

		window.Resize(320, 240);
		picture = frame();
		EXPECT_EQ(320u * 240u * 4u, picture.m_Rgba.size());
		EXPECT_GT(lit_pixels(picture), 0);

		//	minimized: nothing is drawn or presented, and nothing fails
		window.Resize(0, 0);
		frame();
		frame();

		window.Resize(160, 120);
		picture = frame();
		EXPECT_EQ(160u * 120u * 4u, picture.m_Rgba.size());
		EXPECT_GT(lit_pixels(picture), 0);

		//	resized between BeginScene and Present
		window.Window().BeginScene();
		window.Window().Clear(g2dRGBColor(0, 0, 0));
		window.Resize(200, 100);
		picture = frame();
		EXPECT_EQ(200u * 100u * 4u, picture.m_Rgba.size());
		EXPECT_GT(lit_pixels(picture), 0);
		renderer.ReleaseResources();
	});
}

TEST(g2dWindowVkScene, RenderResolutionScales)
{
	gtBackend::Run(gtBackend::e_Vulkan, [&]()
	{
		gtSceneWindow window(gtBackend::e_Vulkan, 200, 100);
		g2dWindow& w = window.Window();
		int width = 0, height = 0;

		//	50x50 rendered, shown 100x100 in the middle with black bars
		w.SetRenderResolution(50, 50);
		w.GetDimensions(width, height);
		EXPECT_EQ(50, width);
		EXPECT_EQ(50, height);
		int x = 0, y = 0;
		w.GetPresentRect(x, y, width, height);
		EXPECT_EQ(50, x);
		EXPECT_EQ(0, y);
		EXPECT_EQ(100, width);
		EXPECT_EQ(100, height);

		w.BeginScene();
		w.Clear(g2dRGBColor(255, 0, 0));
		w.EndScene();
		gtPicture picture = window.Present();
		ASSERT_EQ(200u * 100u * 4u, picture.m_Rgba.size());
		ASSERT_EQ(200, picture.m_Width);
		for (int py = 0; py < 100; py += 3)
		{
			for (int px = 0; px < 200; px += 3)
			{
				const bool inside = px >= 50 && px < 150;
				ASSERT_TRUE(inside ? same_rgb(picture, px, py, 255, 0, 0) : same_rgb(picture, px, py, 0, 0, 0))
					<< "pixel " << px << "," << py;
			}
		}

		//	a scene at 64x64 fills the window's middle 100x100, scaled up
		gtSceneBuilder builder;
		spheres(builder);
		gvk::g3dSceneRendererVk renderer;
		w.SetRenderResolution(64, 64);
		render_frame(w, renderer, builder);
		picture = window.Present();
		ASSERT_EQ(200u * 100u * 4u, picture.m_Rgba.size());
		int lit_middle = 0, lit_bars = 0;
		for (int py = 0; py < 100; py++)
		{
			for (int px = 0; px < 200; px++)
			{
				const int lit = same_rgb(picture, px, py, 0, 0, 0) ? 0 : 1;
				if (px >= 50 && px < 150)
					lit_middle += lit;
				else
					lit_bars += lit;
			}
		}
		EXPECT_GT(lit_middle, 100 * 100 / 20);
		EXPECT_EQ(0, lit_bars);

		//	back to the window's size
		w.SetRenderResolution(0, 0);
		w.GetDimensions(width, height);
		EXPECT_EQ(200, width);
		EXPECT_EQ(100, height);
		w.BeginScene();
		w.Clear(g2dRGBColor(0, 0, 255));
		w.EndScene();
		picture = window.Present();
		ASSERT_EQ(200u * 100u * 4u, picture.m_Rgba.size());
		EXPECT_TRUE(same_rgb(picture, 0, 0, 0, 0, 255));
		EXPECT_TRUE(same_rgb(picture, 199, 99, 0, 0, 255));
		renderer.ReleaseResources();
	});
}

TEST(g2dWindowVkScene, DrawRectAndImageMatchDX11)
{
	//	On each backend: a grey window, a 40x30 image drawn whole, part of
	//	it, clipped at each edge and wholly off, and a rectangle (DX11's
	//	DrawRect draws nothing: "not implemented")
	auto draw = [](gtBackend::Kind i_Kind)
	{
		gtSceneWindow window(i_Kind, 160, 120);
		g2dWindow& w = window.Window();
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
				p[3] = 255;
			}
		}
		image->Release();

		w.BeginScene();
		w.MakeCurrent();
		w.Clear(g2dRGBColor(60, 60, 60));
		w.DrawImage(10, 10, *image);
		w.DrawImage(70, 20, 5, 5, 25, 20, *image);
		w.DrawImage(-15, 80, *image);		// left edge
		w.DrawImage(140, -10, *image);		// top and right edges
		w.DrawImage(100, 100, *image);		// bottom edge
		w.DrawImage(-50, 50, *image);		// all off
		w.DrawRect(5, 5, 50, 50, g2dRGBColor(255, 255, 0));
		w.EndScene();
		return window.Present();
	};
	gtPicture dx11, vulkan;
	if (!gtBackend::Run(gtBackend::e_DX11, [&]() { dx11 = draw(gtBackend::e_DX11); }))
		return;
	if (!gtBackend::Run(gtBackend::e_Vulkan, [&]() { vulkan = draw(gtBackend::e_Vulkan); }))
		return;
	ASSERT_TRUE(dx11.Valid());
	ASSERT_TRUE(vulkan.Valid());
	ASSERT_EQ(dx11.m_Rgba.size(), vulkan.m_Rgba.size());
	//	the image is there (its corner pixels exactly)
	EXPECT_TRUE(same_rgb(dx11, 10, 10, 0, 0, 40));
	EXPECT_TRUE(same_rgb(dx11, 49, 39, 234, 232, 40));
	int differ = 0;
	for (size_t i = 0; i < dx11.m_Rgba.size(); i += 4)
		differ += memcmp(&dx11.m_Rgba[i], &vulkan.m_Rgba[i], 3) != 0;
	EXPECT_EQ(0, differ) << "pixels differ (images in "
		<< gtSaveComparison("DrawRectAndImageMatchDX11", dx11.m_Rgba.data(), vulkan.m_Rgba.data(), 160, 120) << ")";
}
