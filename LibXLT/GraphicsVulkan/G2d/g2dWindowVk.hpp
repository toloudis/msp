/*****************************************************************************
**  g2dWindowVk.hpp
**
**      g2dWindowVk is a g2dWindow drawn with Vulkan: a swap chain on the
**	native window (vkdSwapchain) plus a D24S8 depth buffer, matching the
**	DX11 window (g2dWindowDX11 and g2dWindowPrimaryDX11).
**
**	- BeginScene acquires the frame's swap-chain image (once per frame);
**	  MakeCurrent and the clears make it and the depth buffer vkdContext's
**	  targets, as DX11's configure_device; Present submits and presents.
**	- While the window has no area (or no image can be acquired) the frame
**	  is skipped: no targets are bound, so draws and clears do nothing.
**	- The swap-chain image is unbound from vkdContext at Present (it can't
**	  be drawn to until the next frame acquires one), and the window's
**	  targets when they are resized or destroyed.
**	- SetRenderResolution renders into an RGBA8 image of that size and, in
**	  Present, draws it scaled into the window (GetPresentRect), with black
**	  bars, as g2dWindowPrimaryDX11.
**
**	Not yet ported (GVK_NOT_YET): the debug overlay and DrawText.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GVK_G2DWINDOWVK_HPP
#error g2dWindowVk.hpp multiply included
#endif
#define GVK_G2DWINDOWVK_HPP

#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif
#ifndef MA_RUNNINGAVERAGE_HPP
#include "Core/ma/maRunningAverage.hpp"
#endif
#ifndef GVK_VKDSWAPCHAIN_HPP
#include "GraphicsVulkan/vkd/vkdSwapchain.hpp"
#endif

#include <map>
#include <memory>
#include <string>

namespace gvk
{
	class vkdDevice;
	class vkdImage;
	class g2dDepthStencilBufferVk;

	class g2dWindowVk : public g2dWindow
	{
		public:
			//	i_bOwnHwnd: the window was made for this object (destroyed with it).
			//	i_Width/i_Height: -1 means the window's client size.
			g2dWindowVk(vkdDevice& i_Device, void* i_Hwnd, bool i_bOwnHwnd, int i_Width = -1, int i_Height = -1);
			virtual ~g2dWindowVk();

			//	g2dRenderTarget
			virtual void BeginScene();
			virtual void MakeCurrent();
			virtual void MakeDepthCurrent();
			virtual void EndScene();
			virtual void Clear(const g2dRGBColor& i_Color);
			virtual void Clear(const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1,
							   bool i_ClearStencil = true, unsigned int i_Stencil = 0);
			virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);
			virtual bool GetHasDepthBuffer() const;
			virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

			//	g2dWindow
			virtual bool IsOpen() const;
			virtual void* GetHandle();
			virtual void EnableDebugOverlay(bool i_bEnabled);
			virtual void SetDebugInfo(int i_Line, const char* i_Text);
			virtual void DrawRect(int i_X1, int i_Y1, int i_X2, int i_Y2, const g2dRGBColor& i_Color);
			virtual void DrawText(int i_X, int i_Y, g2dFontHandle i_Font, const itString& i_Text, const g2dRGBColor& i_Color);
			virtual void DrawImage(int i_DX1, int i_DY1, const g2dImage& i_Image);
			virtual void DrawImage(int i_DX1, int i_DY1, int i_SX1, int i_SY1, int i_SX2, int i_SY2, const g2dImage& i_Image);
			virtual void ResizeWindow(int i_Width, int i_Height);
			virtual void SetRenderResolution(int i_Width, int i_Height);
			virtual void GetPresentRect(int& o_X, int& o_Y, int& o_Width, int& o_Height) const;
			virtual void Present();
			virtual void SetTitle(const itString& i_Title);
			virtual float GetFrameRateAverage();
			virtual float GetFrameRate();

			//	What is drawn to (g2dWindowDX11::GetBackBuffer): the offscreen
			//	image, else the frame's swap-chain image (acquired now if need
			//	be); NULL when the frame is skipped
			vkdImage* GetBackBuffer() { return back_buffer(); }

			//	Testing
			VkFormat SwapchainFormat() const { return m_Swapchain.Format(); }
			void DebugReadBackNextFrame() { m_Swapchain.DebugReadBackNextFrame(); }
			const std::vector<uint8_t>& DebugPixels() const { return m_Swapchain.DebugPixels(); }

		private:
			void set_size(int i_Width, int i_Height);
			bool create_offscreen_buffer(int i_Width, int i_Height);
			bool is_offscreen() const { return m_pOffscreen != NULL; }
			//	What is drawn to: the offscreen image, else the frame's
			//	swap-chain image (acquired now if need be); NULL to skip
			vkdImage* back_buffer();
			//	Our targets on vkdContext, as DX11's; false (nothing bound)
			//	when the frame is skipped
			bool configure_device();
			//	Takes the window's images off vkdContext if they are on it
			void release_targets(bool i_bSwapchainOnly);
			void mark_frame();

			vkdDevice&		m_Device;
			void*			m_Hwnd;
			bool			m_bOwnHwnd;
			vkdSwapchain	m_Swapchain;
			shared_ptr<g2dDepthStencilBufferVk>	m_Depth;

			//	size of the swap chain (the window's client area)
			int				m_WindowWidth = 0;
			int				m_WindowHeight = 0;
			//	requested render resolution, 0 when rendering at the window size
			int				m_RenderWidth = 0;
			int				m_RenderHeight = 0;
			//	the image rendered into at the render resolution
			std::unique_ptr<vkdImage>	m_pOffscreen;

			maRunningAverage	m_FPSAverage;
			float				m_StartTime = 0;
			float				m_FrameRate = 0;
			int					m_Frames = 0;
			bool				m_bDebugOverlay = false;
			std::map<int, std::string>	m_DebugLines;
	};
}
