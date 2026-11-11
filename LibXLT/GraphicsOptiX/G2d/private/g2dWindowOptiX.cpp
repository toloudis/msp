/*****************************************************************************
**  g2dWindowOptiX.cpp
**
**      See g2dWindowOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G2DWINDOWOPTIX_HPP
#include "GraphicsOptiX/G2d/g2dWindowOptiX.hpp"
#endif
#ifndef GOX_G2DWINDOWPAC_HPP
#include "GraphicsOptiX/G2d/private/g2dWindowPAC.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/G2d/g2dPFD.hpp"
#endif
#ifndef G2D_RGBCOLOR_HPP
#include "Graphics/G2d/g2dRGBColor.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/Ma/maFloatRGBA.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>

namespace gox
{
	namespace
	{
		const int c_DefaultWidth = 640;
		const int c_DefaultHeight = 480;

		unsigned char to_byte(float i_Value)
		{
			return static_cast<unsigned char>(std::min(std::max(i_Value, 0.0f), 1.0f) * 255.0f + 0.5f);
		}
	}

	g2dWindowOptiX::g2dWindowOptiX(void* i_hWnd, int i_Width, int i_Height, bool i_bHeadless)
		: m_hWnd(i_hWnd)
		, m_bHeadless(i_bHeadless)
		, m_bPresented(false)
	{
		m_ClearColor[0] = m_ClearColor[1] = m_ClearColor[2] = 0;

		int width = c_DefaultWidth;
		int height = c_DefaultHeight;
		if (i_Width > 0 && i_Height > 0)
		{
			width = i_Width;
			height = i_Height;
		}
		else
		{
			int client_width = 0;
			int client_height = 0;
			if (g2dWindowPAC::GetClientSize(m_hWnd, client_width, client_height) && client_width > 0 && client_height > 0)
			{
				width = client_width;
				height = client_height;
			}
		}
		set_size(width, height);
	}

	g2dWindowOptiX::~g2dWindowOptiX()
	{
	}

	//	R8G8B8A8 (red in the low byte), as the other backends' back buffers
	void g2dWindowOptiX::set_size(int i_Width, int i_Height)
	{
		g2dPFD pfd;
		pfd.Set(0, 8, 8, 8, 16, 8, 24, 8, 32);
		g2dWindow::SetWindowProperties(true, pfd, pfd, i_Width, i_Height, 32);
	}

	void g2dWindowOptiX::BeginScene()
	{
	}

	void g2dWindowOptiX::MakeCurrent()
	{
	}

	void g2dWindowOptiX::MakeDepthCurrent()
	{
	}

	void g2dWindowOptiX::EndScene()
	{
	}

	void g2dWindowOptiX::Clear(const g2dRGBColor& i_Color)
	{
		m_ClearColor[0] = i_Color.GetRed();
		m_ClearColor[1] = i_Color.GetGreen();
		m_ClearColor[2] = i_Color.GetBlue();
	}

	void g2dWindowOptiX::Clear(const maFloatRGBA& i_Color, bool /*i_ClearDepth*/, float /*i_Depth*/, bool /*i_ClearStencil*/, unsigned int /*i_Stencil*/)
	{
		m_ClearColor[0] = to_byte(i_Color.GetRed());
		m_ClearColor[1] = to_byte(i_Color.GetGreen());
		m_ClearColor[2] = to_byte(i_Color.GetBlue());
	}

	void g2dWindowOptiX::ClearDepthStencil(float /*i_Depth*/, bool /*i_ClearStencil*/, unsigned int /*i_Stencil*/)
	{
	}

	bool g2dWindowOptiX::GetHasDepthBuffer() const
	{
		return false;
	}

	shared_ptr<g2dDepthStencilBuffer> g2dWindowOptiX::GetDepthStencilBuffer() const
	{
		return shared_ptr<g2dDepthStencilBuffer>();
	}

	bool g2dWindowOptiX::IsOpen() const
	{
		return true;
	}

	void* g2dWindowOptiX::GetHandle()
	{
		return m_hWnd;
	}

	void g2dWindowOptiX::EnableDebugOverlay(bool /*i_bEnabled*/)
	{
	}

	void g2dWindowOptiX::SetDebugInfo(int /*i_Line*/, const char* /*i_Text*/)
	{
	}

	void g2dWindowOptiX::DrawRect(int /*i_X1*/, int /*i_Y1*/, int /*i_X2*/, int /*i_Y2*/, const g2dRGBColor& /*i_Color*/)
	{
	}

	void g2dWindowOptiX::DrawText(int /*i_X*/, int /*i_Y*/, g2dFontHandle /*i_Font*/, const std::string& /*i_Text*/, const g2dRGBColor& /*i_Color*/)
	{
	}

	void g2dWindowOptiX::DrawImage(int /*i_DX1*/, int /*i_DY1*/, const g2dImage& /*i_Image*/)
	{
	}

	void g2dWindowOptiX::DrawImage(int /*i_DX1*/, int /*i_DY1*/, int /*i_SX1*/, int /*i_SY1*/, int /*i_SX2*/, int /*i_SY2*/, const g2dImage& /*i_Image*/)
	{
	}

	void g2dWindowOptiX::ResizeWindow(int i_Width, int i_Height)
	{
		if (i_Width > 0 && i_Height > 0)
		{
			set_size(i_Width, i_Height);
		}
	}

	void g2dWindowOptiX::Present()
	{
		if (!m_bPresented)
		{
			m_bPresented = true;
			DBG_LOG("GraphicsOptiX: first frame presented to a " << m_Width << "x" << m_Height
					<< (m_bHeadless ? " headless" : "") << " window");
		}
		if (!m_bHeadless)
		{
			g2dWindowPAC::FillClient(m_hWnd, m_ClearColor[0], m_ClearColor[1], m_ClearColor[2]);
		}
	}

	void g2dWindowOptiX::SetTitle(const std::string& /*i_Title*/)
	{
	}

	float g2dWindowOptiX::GetFrameRateAverage()
	{
		return 0.0f;
	}

	float g2dWindowOptiX::GetFrameRate()
	{
		return 0.0f;
	}
}
