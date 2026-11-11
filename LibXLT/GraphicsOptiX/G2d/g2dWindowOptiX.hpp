/*****************************************************************************
**  g2dWindowOptiX.hpp
**
**      g2dWindowOptiX is the OptiX backend's window: the render target a
**	view's scene renderer draws into, presented to the native window.
**
**	Stub: draws nothing and has no depth buffer. Present fills the native
**	window with the color it was last cleared to (black at first), so a
**	running window shows it works; headless windows present nothing. Its
**	size is the native window's client area (640x480 without one) unless
**	given, and follows ResizeWindow.
**
**	Extra Large Technology
\****************************************************************************/
#ifdef GOX_G2DWINDOWOPTIX_HPP
#error g2dWindowOptiX.hpp multiply included
#endif
#define GOX_G2DWINDOWOPTIX_HPP

#ifndef G2D_WINDOW_HPP
#include "Graphics/G2d/g2dWindow.hpp"
#endif

#include <string>

namespace gox
{
	class g2dWindowOptiX : public g2dWindow
	{
		public:
			//--------------------------------------------------------------------
			//	i_hWnd: the native window (may be NULL); a width or height of
			//	-1 means the native window's size. i_bHeadless: RenderNode,
			//	where nothing is ever shown.
			//--------------------------------------------------------------------
			g2dWindowOptiX(void* i_hWnd, int i_Width, int i_Height, bool i_bHeadless);
			virtual ~g2dWindowOptiX();

			// g2dRenderTarget
			virtual void BeginScene();
			virtual void MakeCurrent();
			virtual void MakeDepthCurrent();
			virtual void EndScene();
			virtual void Clear(const g2dRGBColor& i_Color);
			virtual void Clear(const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);
			virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);
			virtual bool GetHasDepthBuffer() const;
			virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

			// g2dWindow
			virtual bool IsOpen() const;
			virtual void* GetHandle();
			virtual void EnableDebugOverlay(bool i_bEnabled);
			virtual void SetDebugInfo(int i_Line, const char* i_Text);
			virtual void DrawRect(int i_X1, int i_Y1, int i_X2, int i_Y2, const g2dRGBColor& i_Color);
			virtual void DrawText(int i_X, int i_Y, g2dFontHandle i_Font, const std::string& i_Text, const g2dRGBColor& i_Color);
			virtual void DrawImage(int i_DX1, int i_DY1, const g2dImage& i_Image);
			virtual void DrawImage(int i_DX1, int i_DY1, int i_SX1, int i_SY1, int i_SX2, int i_SY2, const g2dImage& i_Image);
			virtual void ResizeWindow(int i_Width, int i_Height);
			virtual void Present();
			virtual void SetTitle(const std::string& i_Title);
			virtual float GetFrameRateAverage();
			virtual float GetFrameRate();

		private:
			void set_size(int i_Width, int i_Height);

			void*			m_hWnd;
			bool			m_bHeadless;
			bool			m_bPresented;		// logged the first Present
			unsigned char	m_ClearColor[3];	// red, green, blue: what Present fills with
	};
}
