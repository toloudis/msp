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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G2DDEBUGDISPLAY_HPP
#include "GraphicsVulkan/G2d/g2dDebugDisplay.hpp"
#endif
#ifndef G2D_RESOURCECOUNTER_HPP
#include "Graphics/G2d/g2dResourceCounter.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef GVK_G2DWINDOWDRAWUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dWindowDrawUtilVk.hpp"
#endif
#ifndef GVK_G2DFONTUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dFontUtilVk.hpp"
#endif
#ifndef GVK_G3DSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dStateMgr.hpp"
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
#ifndef APP_APPLICATION_HPP
#include "Core/App/appApplication.hpp"
#endif
#ifndef APP_TIME_HPP
#include "Core/App/appTime.hpp"
#endif

#include <algorithm>
#include <iomanip>
#include <mutex>
#include <sstream>
#include <string>

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

	g2dWindowVk::g2dWindowVk(vkdDevice& i_Device, void* i_Hwnd, bool i_bOwnHwnd, int i_Width, int i_Height,
							 bool i_bHeadless)
		: m_Device(i_Device)
		, m_Hwnd(i_Hwnd)
		, m_bOwnHwnd(i_bOwnHwnd)
		, m_FPSAverage(600)
	{
		if (!i_bHeadless)
		{
			m_pSwapchain.reset(new vkdSwapchain(i_Device, i_Hwnd, window_name(i_Hwnd)));
		}
		m_pDebugDisplay.reset(new g2dDebugDisplay(*this));

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
		release_targets(false);
		m_Device.WaitIdle();
		m_pOffscreen.reset();
		m_Depth.reset();
		m_pDebugDisplay.reset();
		m_pSwapchain.reset();
		if (m_bOwnHwnd)
		{
			appApplication::DestroySubWindow(m_Hwnd);
		}
	}

	//	The depth buffer, and the offscreen image if there is a render
	//	resolution, for a new window size (g2dWindowPrimaryDX11::ResizeWindow)
	void g2dWindowVk::set_size(int i_Width, int i_Height)
	{
		release_targets(false);
		m_WindowWidth = i_Width;
		m_WindowHeight = i_Height;
		m_Depth.reset();
		m_pOffscreen.reset();

		// Render offscreen at the render resolution if one is set. Also when
		// the native window's client area isn't the size given: DX11's back
		// buffer is the size given whatever the window (ResizeBuffers), but a
		// swap chain is the client area's size (a capture window bigger than
		// the screen gets a smaller one), so render offscreen at the size
		// given and stretch it over the window in Present, as DXGI does.
		// Headless, there is only the offscreen image.
		int width = i_Width;
		int height = i_Height;
		if (i_Width > 0 && i_Height > 0)
		{
			int client_width = 0, client_height = 0;
			if (m_pSwapchain)
			{
				vkdWindowClientSize(m_Hwnd, client_width, client_height);
			}
			const bool render_resolution = m_RenderWidth > 0 && m_RenderHeight > 0;
			const int target_width = render_resolution ? m_RenderWidth : i_Width;
			const int target_height = render_resolution ? m_RenderHeight : i_Height;
			if ((!m_pSwapchain || render_resolution || client_width != i_Width || client_height != i_Height) &&
				create_offscreen_buffer(target_width, target_height))
			{
				width = target_width;
				height = target_height;
			}
			m_Depth.reset(new g2dDepthStencilBufferVk(&m_Device));
			m_Depth->Make(width, height, g2dVkGlobal::e_DXGI_D24_UNORM_S8_UINT, VK_SAMPLE_COUNT_1_BIT, g2dResourceCounterVk::eWindow);
		}
		//	R8G8B8A8 (red in the low byte); B8G8R8A8 only if the surface lacks
		//	it (the offscreen image is always R8G8B8A8, as DX11's back buffer)
		g2dPFD pfd;
		if (!is_offscreen() && SwapchainFormat() == VK_FORMAT_B8G8R8A8_UNORM)
		{
			pfd.Set(16, 8, 8, 8, 0, 8, 24, 8, 32);
		}
		else
		{
			pfd.Set(0, 8, 8, 8, 16, 8, 24, 8, 32);
		}
		g2dWindow::SetWindowProperties(true, pfd, pfd, width, height, 32);
	}

	//	g2dWindowPrimaryDX11::create_offscreen_buffer: the image rendered
	//	into, drawn into the swap chain's in Present
	bool g2dWindowVk::create_offscreen_buffer(int i_Width, int i_Height)
	{
		m_pOffscreen = vkdImage::Create2D(m_Device, (uint32_t)i_Width, (uint32_t)i_Height, VK_FORMAT_R8G8B8A8_UNORM,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT |
			VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT, "g2dWindowVk offscreen");
		return m_pOffscreen != NULL;
	}

	vkdImage* g2dWindowVk::back_buffer()
	{
		if (m_pOffscreen)
		{
			return m_pOffscreen.get();
		}
		return m_pSwapchain && m_pSwapchain->Acquire() ? m_pSwapchain->Target() : NULL;
	}

	VkFormat g2dWindowVk::SwapchainFormat() const
	{
		return m_pSwapchain ? m_pSwapchain->Format() : VK_FORMAT_R8G8B8A8_UNORM;
	}

	void g2dWindowVk::DebugReadBackNextFrame()
	{
		if (m_pSwapchain)
		{
			m_pSwapchain->DebugReadBackNextFrame();
		}
	}

	const std::vector<uint8_t>& g2dWindowVk::DebugPixels() const
	{
		static const std::vector<uint8_t> c_None;
		return m_pSwapchain ? m_pSwapchain->DebugPixels() : c_None;
	}

	//	g2dWindowDX11::configure_device: our back buffer and depth buffer as
	//	the targets, and a viewport over the back buffer
	bool g2dWindowVk::configure_device()
	{
		vkdImage* target = m_Depth ? back_buffer() : NULL;
		if (!target)
		{
			//	the frame is skipped: nothing to draw into
			release_targets(false);
			return false;
		}
		g2dVkGlobal::SetRenderTargets(target, m_Depth->GetDepthView());
		g2dVkGlobal::SetViewport((int)target->Width(), (int)target->Height());
		return true;
	}

	void g2dWindowVk::release_targets(bool i_bSwapchainOnly)
	{
		if (!vkdGlobal::IsSet())
		{
			return;
		}
		vkdImage* color = g2dVkGlobal::GetColorTarget();
		vkdImage* depth = g2dVkGlobal::GetDepthTarget();
		const bool swapchain_color = color && m_pSwapchain && color == m_pSwapchain->Target();
		const bool our_color = swapchain_color || (color && color == m_pOffscreen.get());
		const bool our_depth = depth && m_Depth && depth == m_Depth->GetDepthView();
		if (i_bSwapchainOnly)
		{
			if (swapchain_color)
			{
				g2dVkGlobal::SetColorTarget(NULL);
			}
		}
		else if (our_color || our_depth)
		{
			g2dVkGlobal::SetRenderTargets(our_color ? NULL : color, our_depth ? NULL : depth);
			vkdGlobal::Context().SetViewports(0, NULL);
		}
	}

	//------------------------------------------------------------------------
	//	g2dRenderTarget
	//------------------------------------------------------------------------
	void g2dWindowVk::BeginScene()
	{
		g3dStateMgrVk::SetDefault();

		// the frame's swap-chain image
		if (m_pSwapchain)
		{
			m_pSwapchain->Acquire();
		}
	}

	void g2dWindowVk::MakeCurrent()
	{
		configure_device();
	}

	void g2dWindowVk::MakeDepthCurrent()
	{
		if (m_Depth)
		{
			g2dVkGlobal::SetDepthTarget(m_Depth->GetDepthView());
		}
		else
		{
			DBG_TRACE("MakeDepthCurrent called with no depth buffer.");
		}
	}

	//	g2dWindowDX11::EndScene: the frame rate, and the debug overlay
	void g2dWindowVk::EndScene()
	{
		mark_frame();

		// we want the average fps to be kept track of even if we are not viewing it
		m_FPSAverage.Push(m_FrameRate);

		std::string line;
		std::ostringstream oss;
		oss.setf(std::ios::fixed, std::ios::floatfield);

		oss << "app fps:     "<<std::setprecision(2)<<m_FrameRate<<" "<<std::setprecision(1)<<std::setw(3)<<1000.0f/m_FrameRate<<" ms";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(0, line.c_str());

		oss.str("");
		oss << "app avg fps: "<<std::setprecision(2)<<m_FPSAverage.GetAverage()<<" "<<std::setprecision(1)<<std::setw(3)<<1000.0f/m_FPSAverage.GetAverage()<<" ms";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(1, line.c_str());

		float ge = g2dResourceCounter::GetTotalVertexBufferMemory() + g2dResourceCounter::GetTotalIndexBufferMemory();
		oss.str("");
		oss << "Geometry: "<<std::setprecision(2)<<std::setw(6)<<ge/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(2, line.c_str());

		float tx = g2dResourceCounter::GetTotalSceneTextureMemory();
		oss.str("");
		oss << "Textures: "<<std::setprecision(2)<<std::setw(6)<<tx/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(3, line.c_str());

		float sh = g2dResourceCounter::GetTotalShadowMapMemory();
		oss.str("");
		oss << "Shadows: "<<std::setprecision(2)<<std::setw(6)<<sh/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(4, line.c_str());

		float fb = g2dResourceCounter::GetTotalFramebufferMemory();
		oss.str("");
		oss << "Framebuffers: "<<std::setprecision(2)<<std::setw(6)<<fb/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(5, line.c_str());

		m_pDebugDisplay->Render();
	}

	//	As DX11: opaque colour, and the depth buffer cleared to 1.
	void g2dWindowVk::Clear(const g2dRGBColor& i_Color)
	{
		if (!configure_device())
		{
			return;
		}
		const float rgba[4] = { i_Color.GetRed() / 255.0f, i_Color.GetGreen() / 255.0f, i_Color.GetBlue() / 255.0f, 1.0f };
		g2dVkGlobal::ClearCurrent(rgba, true, 1.0f, false, 0);
	}

	void g2dWindowVk::Clear(const maFloatRGBA& i_Color, bool i_ClearDepth, float i_Depth,
							bool i_ClearStencil, unsigned int i_Stencil)
	{
		if (!configure_device())
		{
			return;
		}
		const float rgba[4] = { i_Color.m_Red, i_Color.m_Green, i_Color.m_Blue, i_Color.m_Alpha };
		g2dVkGlobal::ClearCurrent(rgba, i_ClearDepth, i_Depth, i_ClearStencil, i_Stencil);
	}

	void g2dWindowVk::ClearDepthStencil(float i_Depth, bool i_ClearStencil, unsigned int i_Stencil)
	{
		if (m_Depth)
		{
			vkdGlobal::Context().ClearDepthStencil(m_Depth->GetDepthView(), true, i_Depth, i_ClearStencil, (uint8_t)i_Stencil);
		}
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
		m_pDebugDisplay->EnableDebugOverlay(i_bEnabled);
	}

	void g2dWindowVk::SetDebugInfo(int i_Line, const char* i_Text)
	{
		m_pDebugDisplay->SetDebugInfo(i_Line, i_Text);
	}

	void g2dWindowVk::DrawRect(int i_X1, int i_Y1, int i_X2, int i_Y2, const g2dRGBColor& i_Color)
	{
		configure_device();
		g2dWindowDrawUtilVk::DrawRect(i_X1, i_Y1, i_X2, i_Y2, i_Color);
	}

	//------------------------------------------------------------------------
	//	DrawText draws the given text such that it's upper left corner is
	//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
	//	current viewing area.
	//------------------------------------------------------------------------
	void g2dWindowVk::DrawText(int i_X, int i_Y, g2dFontHandle i_Font, const std::string& i_Text, const g2dRGBColor& i_Color)
	{
		configure_device();

		g2dFontObjectVk* font = reinterpret_cast<g2dFontObjectVk*>(i_Font);
		if (font)
			font->DrawText(i_Text, maFloatRGBA(i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 1.0f), i_X, i_Y);
	}

	void g2dWindowVk::DrawImage(int i_DX1, int i_DY1, const g2dImage& i_Image)
	{
		int w, h;
		this->GetDimensions(w, h);
		g2dWindowDrawUtilVk::DrawImage(back_buffer(), w, h, i_DX1, i_DY1, i_Image);
	}

	void g2dWindowVk::DrawImage(int i_DX1, int i_DY1, int i_SX1, int i_SY1, int i_SX2, int i_SY2, const g2dImage& i_Image)
	{
		int w, h;
		this->GetDimensions(w, h);
		g2dWindowDrawUtilVk::DrawImage(back_buffer(), w, h, i_DX1, i_DY1, i_SX1, i_SY1, i_SX2, i_SY2, i_Image);
	}

	void g2dWindowVk::ResizeWindow(int i_Width, int i_Height)
	{
		//	An image acquired at the old size can't be given back unpresented:
		//	present it now (DX11's ResizeBuffers drops the frame being drawn),
		//	so the next frame acquires one of the new size
		if (m_pSwapchain)
		{
			if (m_pSwapchain->IsAcquired())
			{
				release_targets(true);
				m_pSwapchain->Present();
			}
			m_pSwapchain->Invalidate();
		}
		set_size(i_Width, i_Height);
	}

	//	Render offscreen at the given size and scale the image to fit the
	//	window, centered with black bars, in Present.  0, 0 turns this off.
	void g2dWindowVk::SetRenderResolution(int i_Width, int i_Height)
	{
		if (i_Width <= 0 || i_Height <= 0)
		{
			i_Width = 0;
			i_Height = 0;
		}
		if (i_Width == m_RenderWidth && i_Height == m_RenderHeight)
		{
			return;
		}
		m_RenderWidth = i_Width;
		m_RenderHeight = i_Height;
		if (m_WindowWidth > 0 && m_WindowHeight > 0)
		{
			set_size(m_WindowWidth, m_WindowHeight);
		}
	}

	//	g2dWindowPrimaryDX11::GetPresentRect: where the rendered image is
	//	drawn in the window, in window pixels
	void g2dWindowVk::GetPresentRect(int& o_X, int& o_Y, int& o_Width, int& o_Height) const
	{
		if (!is_offscreen() || m_Width <= 0 || m_Height <= 0)
		{
			g2dWindow::GetPresentRect(o_X, o_Y, o_Width, o_Height);
			return;
		}

		// largest rect with the render aspect ratio that fits in the window
		if (m_WindowWidth * m_Height <= m_WindowHeight * m_Width)
		{
			o_Width = m_WindowWidth;
			o_Height = (m_WindowWidth * m_Height + m_Width / 2) / m_Width;
		}
		else
		{
			o_Height = m_WindowHeight;
			o_Width = (m_WindowHeight * m_Width + m_Height / 2) / m_Height;
		}
		// the panel is shaped to the image, so a gap of a pixel or two is only
		// rounding: fill the window rather than leave a sliver of border
		if (m_WindowWidth - o_Width <= 2 && m_WindowHeight - o_Height <= 2)
		{
			o_Width = m_WindowWidth;
			o_Height = m_WindowHeight;
		}
		o_Width = (std::max)(o_Width, 1);
		o_Height = (std::max)(o_Height, 1);
		o_X = (m_WindowWidth - o_Width) / 2;
		o_Y = (m_WindowHeight - o_Height) / 2;
	}

	void g2dWindowVk::Present()
	{
		// Headless: nothing to show; the frame is submitted
		if (!m_pSwapchain)
		{
			vkdGlobal::Context().Flush();
			return;
		}

		// When rendering offscreen, draw the image scaled to fit the window,
		// centered, with black bars where the aspect ratios differ
		if (is_offscreen() && m_pSwapchain->Acquire())
		{
			g2dVkGlobal::SetRenderTargets(m_pSwapchain->Target(), NULL);
			const float black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			g2dVkGlobal::ClearCurrent(black);

			int x, y, width, height;
			GetPresentRect(x, y, width, height);
			if (m_RenderWidth <= 0 || m_RenderHeight <= 0)
			{
				// offscreen only because the client area isn't the size
				// given: stretched over the whole window, as DXGI presents
				x = 0;
				y = 0;
				width = (int)m_pSwapchain->Target()->Width();
				height = (int)m_pSwapchain->Target()->Height();
			}
			g2dFullscreenQuad::DrawTexturedQuad(m_pOffscreen.get(),
				(uint32_t)width, (uint32_t)height, (float)x, (float)y);

			// put back the default states and our offscreen target
			g3dStateMgrVk::SetDefault();
			g2dVkGlobal::SetRenderTargets(m_pOffscreen.get(),
				m_Depth ? m_Depth->GetDepthView() : NULL);
		}

		// the image can't be drawn to once presented
		release_targets(true);
		m_pSwapchain->Present();
	}

	void g2dWindowVk::SetTitle(const std::string& i_Title)
	{
		vkdSetWindowTitle(m_Hwnd, i_Title);
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
