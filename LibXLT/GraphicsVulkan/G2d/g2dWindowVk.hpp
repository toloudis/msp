/*****************************************************************************
**  g2dWindowVk.hpp
**
**      g2dWindowVk is a g2dWindow drawn with Vulkan: a swap chain on the
**	native window (vkdSwapchain) plus a D24S8 depth buffer, matching the
**	DX11 window (g2dWindowPrimaryDX11). Clears record into the device's
**	command stream; Present submits and presents.
**
**	Not yet ported (GVK_NOT_YET): the debug overlay, DrawText/DrawRect/
**	DrawImage, and rendering at a resolution other than the window's.
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
			virtual void Present();
			virtual void SetTitle(const itString& i_Title);
			virtual float GetFrameRateAverage();
			virtual float GetFrameRate();

			//	Testing
			VkFormat SwapchainFormat() const { return m_Swapchain.Format(); }
			void DebugReadBackNextFrame() { m_Swapchain.DebugReadBackNextFrame(); }
			const std::vector<uint8_t>& DebugPixels() const { return m_Swapchain.DebugPixels(); }

		private:
			void set_size(int i_Width, int i_Height);
			void clear_color(float r, float g, float b, float a);
			void clear_depth(bool i_bDepth, float i_Depth, bool i_bStencil, unsigned int i_Stencil);
			void mark_frame();

			vkdDevice&		m_Device;
			void*			m_Hwnd;
			bool			m_bOwnHwnd;
			vkdSwapchain	m_Swapchain;
			shared_ptr<g2dDepthStencilBufferVk>	m_Depth;

			maRunningAverage	m_FPSAverage;
			float				m_StartTime = 0;
			float				m_FrameRate = 0;
			int					m_Frames = 0;
			bool				m_bDebugOverlay = false;
			std::map<int, std::string>	m_DebugLines;
	};
}
