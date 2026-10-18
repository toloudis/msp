/*****************************************************************************
**  g2dWindowVkTest.cpp
**
**      g2dWindowVk: surface and swap chain on a (hidden) native window:
**      clear and present, resize including to nothing, two windows, and
**      the GPU timer for g3dProfiler. Skipped without a Vulkan device.
**
**	Extra Large Technology
\****************************************************************************/
#include "gtVulkan.hpp"
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
#include "gtTempDir.hpp"

#include <fstream>
#include <memory>
#include <string>

using namespace gvk;

namespace
{
	class gtWindow : public gtVulkan
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
				gtVulkan::TearDown();
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
