/*****************************************************************************
**  g2dWindowVk.cpp
**
**      See g2dWindowVk.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GVK_G2DWINDOWVK_HPP
#include "GraphicsVulkan/G2d/g2dWindowVk.hpp"
#endif
#ifndef GVK_G2DDEPTHSTENCILBUFFERVK_HPP
#include "GraphicsVulkan/G2d/g2dDepthStencilBufferVk.hpp"
#endif
#ifndef GVK_VKDDEVICE_HPP
#include "GraphicsVulkan/vkd/vkdDevice.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef GVK_VKDCHECK_HPP
#include "GraphicsVulkan/vkd/vkdCheck.hpp"
#endif
#ifndef GVK_VKDINFO_HPP
#include "GraphicsVulkan/vkd/vkdInfo.hpp"
#endif
#ifndef GVK_VKDSURFACE_HPP
#include "GraphicsVulkan/vkd/vkdSurface.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/It/itString.hpp"
#endif
#ifndef APP_APPLICATION_HPP
#include "Core/App/appApplication.hpp"
#endif
#ifndef APP_TIME_HPP
#include "Core/App/appTime.hpp"
#endif

#include <mutex>

namespace gvk
{
	namespace
	{
		std::string window_name(void* i_Hwnd)
		{
			char text[64];
			snprintf(text, sizeof(text), "window %p", i_Hwnd);
			return text;
		}
	}

	g2dWindowVk::g2dWindowVk(vkdDevice& i_Device, void* i_Hwnd, bool i_bOwnHwnd, int i_Width, int i_Height)
		: m_Device(i_Device)
		, m_Hwnd(i_Hwnd)
		, m_bOwnHwnd(i_bOwnHwnd)
		, m_Swapchain(i_Device, i_Hwnd, window_name(i_Hwnd))
		, m_FPSAverage(600)
	{
		int width = i_Width;
		int height = i_Height;
		if (width < 0 || height < 0)
		{
			vkdWindowClientSize(i_Hwnd, width, height);
		}
		set_size(width, height);
	}

	g2dWindowVk::~g2dWindowVk()
	{
		m_Device.WaitIdle();
		m_Depth.reset();
		if (m_bOwnHwnd)
		{
			appApplication::DestroySubWindow(m_Hwnd);
		}
	}

	//	The swap chain's format and the depth buffer for a new size.
	void g2dWindowVk::set_size(int i_Width, int i_Height)
	{
		m_Depth.reset();
		if (i_Width > 0 && i_Height > 0)
		{
			m_Depth.reset(new g2dDepthStencilBufferVk(&m_Device));
			m_Depth->Make(i_Width, i_Height, g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT, VK_SAMPLE_COUNT_1_BIT, g2dResourceCounterVk::eWindow);
		}
		//	R8G8B8A8 (red in the low byte); B8G8R8A8 only if the surface lacks it
		g2dPFD pfd;
		if (m_Swapchain.Format() == VK_FORMAT_B8G8R8A8_UNORM)
		{
			pfd.Set(16, 8, 8, 8, 0, 8, 24, 8, 32);
		}
		else
		{
			pfd.Set(0, 8, 8, 8, 16, 8, 24, 8, 32);
		}
		g2dWindow::SetWindowProperties(true, pfd, pfd, i_Width, i_Height, 32);
	}

	//------------------------------------------------------------------------
	//	g2dRenderTarget
	//------------------------------------------------------------------------
	void g2dWindowVk::BeginScene()
	{
		m_Swapchain.Acquire();
	}

	void g2dWindowVk::MakeCurrent()
	{
		m_Swapchain.Acquire();
	}

	void g2dWindowVk::MakeDepthCurrent()
	{
	}

	void g2dWindowVk::EndScene()
	{
		mark_frame();
		m_FPSAverage.Push(m_FrameRate);
	}

	void g2dWindowVk::clear_color(float r, float g, float b, float a)
	{
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		if (!m_Swapchain.Acquire())
		{
			return;
		}
		VkCommandBuffer commands = m_Device.Commands();
		vkdFullBarrier(commands);
		VkClearColorValue color = {};
		color.float32[0] = r; color.float32[1] = g; color.float32[2] = b; color.float32[3] = a;
		VkImageSubresourceRange range = vkdRange();
		vkCmdClearColorImage(commands, m_Swapchain.Image(), VK_IMAGE_LAYOUT_GENERAL, &color, 1, &range);
	}

	void g2dWindowVk::clear_depth(bool i_bDepth, float i_Depth, bool i_bStencil, unsigned int i_Stencil)
	{
		if (!m_Depth || (!i_bDepth && !i_bStencil))
		{
			return;
		}
		std::lock_guard<std::recursive_mutex> lock(m_Device.StreamLock());
		VkCommandBuffer commands = m_Device.Commands();
		vkdFullBarrier(commands);
		VkClearDepthStencilValue value = {i_Depth, i_Stencil};
		VkImageSubresourceRange range = vkdRange((i_bDepth ? VK_IMAGE_ASPECT_DEPTH_BIT : 0) |
												 (i_bStencil ? VK_IMAGE_ASPECT_STENCIL_BIT : 0));
		vkCmdClearDepthStencilImage(commands, m_Depth->GetDepthView()->Image(), VK_IMAGE_LAYOUT_GENERAL, &value, 1, &range);
	}

	//	As DX11: opaque colour, and the depth buffer cleared to 1.
	void g2dWindowVk::Clear(const g2dRGBColor& i_Color)
	{
		clear_color(i_Color.GetRed() / 255.0f, i_Color.GetGreen() / 255.0f, i_Color.GetBlue() / 255.0f, 1.0f);
		clear_depth(true, 1.0f, false, 0);
	}

	void g2dWindowVk::Clear(const maFloatRGBA& i_Color, bool i_ClearDepth, float i_Depth,
							bool i_ClearStencil, unsigned int i_Stencil)
	{
		clear_color(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha());
		if (i_ClearDepth)
		{
			clear_depth(true, i_Depth, i_ClearStencil, i_Stencil);
		}
	}

	void g2dWindowVk::ClearDepthStencil(float i_Depth, bool i_ClearStencil, unsigned int i_Stencil)
	{
		clear_depth(true, i_Depth, i_ClearStencil, i_Stencil);
	}

	bool g2dWindowVk::GetHasDepthBuffer() const
	{
		return m_Depth != NULL;
	}

	shared_ptr<g2dDepthStencilBuffer> g2dWindowVk::GetDepthStencilBuffer() const
	{
		return m_Depth;
	}

	//------------------------------------------------------------------------
	//	g2dWindow
	//------------------------------------------------------------------------
	bool g2dWindowVk::IsOpen() const
	{
		return vkdWindowIsOpen(m_Hwnd);
	}

	void* g2dWindowVk::GetHandle()
	{
		return m_Hwnd;
	}

	void g2dWindowVk::EnableDebugOverlay(bool i_bEnabled)
	{
		m_bDebugOverlay = i_bEnabled;
	}

	void g2dWindowVk::SetDebugInfo(int i_Line, const char* i_Text)
	{
		if (i_Text)
		{
			m_DebugLines[i_Line] = i_Text;
		}
		else
		{
			m_DebugLines.erase(i_Line);
		}
	}

	void g2dWindowVk::DrawRect(int, int, int, int, const g2dRGBColor&)
	{
		GVK_NOT_YET("g2dWindowVk::DrawRect");
	}

	void g2dWindowVk::DrawText(int, int, g2dFontHandle, const itString&, const g2dRGBColor&)
	{
		GVK_NOT_YET("g2dWindowVk::DrawText");
	}

	void g2dWindowVk::DrawImage(int, int, const g2dImage&)
	{
		GVK_NOT_YET("g2dWindowVk::DrawImage");
	}

	void g2dWindowVk::DrawImage(int, int, int, int, int, int, const g2dImage&)
	{
		GVK_NOT_YET("g2dWindowVk::DrawImage");
	}

	void g2dWindowVk::ResizeWindow(int i_Width, int i_Height)
	{
		m_Swapchain.Invalidate();
		set_size(i_Width, i_Height);
	}

	void g2dWindowVk::SetRenderResolution(int i_Width, int i_Height)
	{
		if (i_Width > 0 && i_Height > 0)
		{
			GVK_NOT_YET("g2dWindowVk::SetRenderResolution");		// renders at the window's size for now
		}
	}

	void g2dWindowVk::Present()
	{
		if (m_bDebugOverlay && !m_DebugLines.empty())
		{
			GVK_NOT_YET("g2dWindowVk debug overlay");
		}
		m_Swapchain.Present();
	}

	void g2dWindowVk::SetTitle(const itString& i_Title)
	{
		vkdSetWindowTitle(m_Hwnd, i_Title.GetString());
	}

	float g2dWindowVk::GetFrameRateAverage()
	{
		return m_FPSAverage.GetAverage();
	}

	float g2dWindowVk::GetFrameRate()
	{
		return m_FrameRate;
	}

	//	As g2dWindowDX11::mark_frame: frames per second over 10 frames.
	void g2dWindowVk::mark_frame()
	{
		const int c_FramesToCount = 10;
		if (++m_Frames == c_FramesToCount)
		{
			const float end_time = appTime::GetTime();
			const float elapsed = end_time - m_StartTime;
			m_FrameRate = elapsed > 0 ? float(c_FramesToCount) / elapsed : 0.0f;
			m_StartTime = end_time;
			m_Frames = 0;
		}
	}
}
