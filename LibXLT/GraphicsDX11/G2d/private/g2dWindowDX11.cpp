/****************************************************************************\
**	g2dWindowDX11.cpp
**
**		g2dWindowDX11.cpp defines the g2d screen PAC for windows.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"

#include "GraphicsDX11/g2d/g2dDebugDisplay.hpp"
#include "GraphicsDX11/g2d/g2dDepthStencilBufferDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dFontUtilDX11.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dStateMgr.hpp"

#include "Core/app/appTime.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"

#include <iomanip>


//============================================================================
//	anonymous namespace for private data and functions
//============================================================================
namespace
{
	bool l_bDebugEnabled = true;

}	// end of namespace


//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dWindowDX11::g2dWindowDX11()
:	m_pDebugDisplay(NULL),
	m_FPSAverage(600), m_StartTime(0), m_FrameRate(0), m_Frames(0),
	m_pSwapChain(NULL), m_pBackBuffer(NULL),
	m_pBackBuffer2D(NULL), 	m_pSolidBrush2D(NULL)
{
	m_pDebugDisplay = new g2dDebugDisplay(*this);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
g2dWindowDX11::~g2dWindowDX11()
{
	FreeBuffers();
	delete m_pDebugDisplay;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dWindowDX11::FreeBuffers()
{
	m_DepthStencil.reset();
}

//------------------------------------------------------------------------
//	FrameRate information
//------------------------------------------------------------------------
//virtual 
float g2dWindowDX11::GetFrameRateAverage()
{
	return this->m_FPSAverage.GetAverage();
}

//virtual 
float g2dWindowDX11::GetFrameRate()
{
	return this->m_FrameRate;
}

//------------------------------------------------------------------------
//	SetDebugInfo causes the the given text to displayed at the given line
//	of the debug text overlay.  The debug overlay is toggled on and off
//	by the user; currently, this is done with the tilde key.
//	To remove a debug info, call the function with i_Text == NULL.
//------------------------------------------------------------------------
void g2dWindowDX11::SetDebugInfo(int i_Line, const char* i_Text)
{
	m_pDebugDisplay->SetDebugInfo(i_Line, i_Text);
}

//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
void g2dWindowDX11::BeginScene()
{
	// direct device to output to our window
	//configure_device();
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void g2dWindowDX11::MakeCurrent()
{
	// direct device to output to our window
	configure_device();
}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void g2dWindowDX11::MakeDepthCurrent()
{
	if (m_DepthStencil)
	{
		g2dDX11Global::SetDepthTarget(m_DepthStencil->GetDepthView());
	}
	else
	{
		DBG_TRACE("MakeDepthCurrent called with no depth buffer.");
	}
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
void g2dWindowDX11::EndScene()
{
	mark_frame();

	// we want the average fps to be kept track of even if we are not viewing it
	m_FPSAverage.Push(m_FrameRate);

	if( l_bDebugEnabled )
	{
		//char line[48];
		std::string line;
		std::ostringstream oss;
		//oss.setf(0, std::ios::floatfield);
		oss.setf(std::ios::fixed, std::ios::floatfield);

		//sprintf(line, "fps: %.2f %3.1f ms", m_FrameRate, 1000.0f/m_FrameRate);
		oss << "app fps:     "<<std::setprecision(2)<<m_FrameRate<<" "<<std::setprecision(1)<<std::setw(3)<<1000.0f/m_FrameRate<<" ms";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(0, line.c_str());
		
		//sprintf(line, " afps: %.2f %3.1f ms", m_FPSAverage.GetAverage(), 1000.0f/m_FPSAverage.GetAverage());
		oss.str("");
		oss << "app avg fps: "<<std::setprecision(2)<<m_FPSAverage.GetAverage()<<" "<<std::setprecision(1)<<std::setw(3)<<1000.0f/m_FPSAverage.GetAverage()<<" ms";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(1, line.c_str());

		float ge = g2dResourceCounter::GetTotalVertexBufferMemory() + g2dResourceCounter::GetTotalIndexBufferMemory();
		//sprintf(line, "Geometry: %6.2f KB", ge);
		oss.str("");
		oss << "Geometry: "<<std::setprecision(2)<<std::setw(6)<<ge/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(2, line.c_str());
		
		float tx = g2dResourceCounter::GetTotalSceneTextureMemory();
		//sprintf(line, "Textures: %6.2f KB", tx);
		oss.str("");
		oss << "Textures: "<<std::setprecision(2)<<std::setw(6)<<tx/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(3, line.c_str());

		float sh = g2dResourceCounter::GetTotalShadowMapMemory();
		//sprintf(line, "Shadows: %6.2f KB", sh);
		oss.str("");
		oss << "Shadows: "<<std::setprecision(2)<<std::setw(6)<<sh/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(4, line.c_str());

		float fb = g2dResourceCounter::GetTotalFramebufferMemory();
		//sprintf(line, "Framebuffers: %6.2f KB", fb);
		oss.str("");
		oss << "Framebuffers: "<<std::setprecision(2)<<std::setw(6)<<fb/1024.0f<<" MB";
		line = oss.str();
		m_pDebugDisplay->SetDebugInfo(5, line.c_str());

		m_pDebugDisplay->Render();
	}
}

//------------------------------------------------------------------------
// Toggle display of Debug text overlay. When you don't want to do it with
// the tilde key.
//------------------------------------------------------------------------
void g2dWindowDX11::EnableDebugOverlay(bool i_bEnabled)
{
	if( l_bDebugEnabled )
	{
		m_pDebugDisplay->EnableDebugOverlay(i_bEnabled);
	}
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void g2dWindowDX11::Clear(const g2dRGBColor& i_Color)
{
	configure_device();
	float c[4];
	c[0] = i_Color.GetRed()/255.0f;
	c[1] = i_Color.GetGreen()/255.0f;
	c[2] = i_Color.GetBlue()/255.0f;
	c[3] = 1;
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(this->m_pBackBuffer, c);

	// temp: does it work to clear here?
	g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), D3D11_CLEAR_DEPTH, 1, 0);
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
//virtual 
void g2dWindowDX11::Clear(const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
						  float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	configure_device();
	float c[4];
	c[0] = i_Color.m_Red;
	c[1] = i_Color.m_Green;
	c[2] = i_Color.m_Blue;
	c[3] = i_Color.m_Alpha;
	g2dDX11Global::g_pDeviceContext->ClearRenderTargetView(this->m_pBackBuffer, c);

	// temp: does it work to clear here?
	if (i_ClearDepth && m_DepthStencil)
	{
		g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
			D3D11_CLEAR_DEPTH | (i_ClearStencil ? D3D11_CLEAR_STENCIL : 0), i_Depth, i_Stencil);
	}
}

//------------------------------------------------------------------------
//	Clear fills the depthstencil buffer with the given values.
//	If there is no depthstencil, then this does nothing.
//------------------------------------------------------------------------
//virtual 
void g2dWindowDX11::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	// temp: does it work to clear here?
	g2dDX11Global::g_pDeviceContext->ClearDepthStencilView(this->m_DepthStencil->GetDepthView(), 
		i_ClearStencil ? D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL : D3D11_CLEAR_DEPTH, 
		i_Depth, i_Stencil);
}

//------------------------------------------------------------------------
//	DrawRect draws a rectangle of the given color between the two locations
//	given (in client coordinates).  The rectangle will include the
//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
//	for spacing adjacent rectangles.
//	The rectangle will be silently clipped or rejected by the current
//	screen viewing area.
//------------------------------------------------------------------------
void g2dWindowDX11::DrawRect(	int i_X1,
								int i_Y1,
								int i_X2,
								int i_Y2,
								const g2dRGBColor& i_Color)
{
	configure_device();
	g2dWindowDrawUtilDX11::DrawRect(i_X1, i_Y1, i_X2, i_Y2, i_Color);
}


//this is needed since WinUser.h #defines this as DrawTextA or DrawTextW
#undef DrawText
//------------------------------------------------------------------------
//	DrawText draws the given text such that it's upper left corner is
//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
//	current viewing area.
//------------------------------------------------------------------------
void g2dWindowDX11::DrawText(	int i_X,
								int i_Y,
								g2dFontHandle i_Font,
								const itString& i_Text,
								const g2dRGBColor& i_Color)
{
	/*if ((m_pBackBuffer2D == NULL) || (m_pSolidBrush2D == NULL))
		return;*/

	configure_device();

	g2dFontObjectDX11* font = reinterpret_cast<g2dFontObjectDX11*>(i_Font);
	if (font)
		font->DrawText(i_Text, maFloatRGBA(i_Color.GetRed()/255.0f, i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f, 1.0f), i_X, i_Y);

	//float dpiScaleX, dpiScaleY;
	//m_pBackBuffer2D->GetDpi(&dpiScaleX, &dpiScaleY);

//   // Create a D2D rect that is the same size as the window.
	//int width, height;
	//g2dFontUtil::GetTextSizeInPixels(i_Font, i_Text, width, height);
	//D2D1_RECT_F layoutRect = D2D1::RectF(
	//	static_cast<FLOAT>(i_X) / dpiScaleX,
	//	static_cast<FLOAT>(i_Y) / dpiScaleY,
	//	static_cast<FLOAT>(i_X + width) / dpiScaleX,
	//	static_cast<FLOAT>(i_Y + height) / dpiScaleY
	//	);

	//m_pSolidBrush2D->SetColor(D2D1::ColorF(i_Color.GetRed()/255.0f, 
	//	i_Color.GetGreen()/255.0f, i_Color.GetBlue()/255.0f));

//   // Use the DrawText method of the D2D render target interface to draw.

//   m_pBackBuffer2D->DrawText(
//       i_Text.GetString(),        // The string to render.
//       i_Text.GetLength(),    // The string's length.
//       reinterpret_cast<IDWriteTextFormat*>(i_Font),    // The text format.
//       layoutRect,       // The region of the window where the text will be rendered.
//       m_pSolidBrush2D     // The brush used to draw the text.
//       );
}

//------------------------------------------------------------------------
//	This DrawImage blits the given image to the screen so that it's upper left
//	corner will be located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void g2dWindowDX11::DrawImage(	int i_DX1,
								int i_DY1,
								const g2dImage& i_Image)
{
	g2dD3D11RenderTargetPtr bbuf = GetBackBuffer();
	int w,h;
	this->GetDimensions(w,h);
	g2dWindowDrawUtilDX11::DrawImage(bbuf, w, h, i_DX1, i_DY1, i_Image);
	bbuf->Release();
}

//------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the given image to the
//	screen.  The rectangle of the source image is defined by
//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
//	located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void g2dWindowDX11::DrawImage(	int i_DX1,
								int i_DY1,
								int i_SX1,
								int i_SY1,
								int i_SX2,
								int i_SY2,
								const g2dImage& i_Image)
{
	g2dD3D11RenderTargetPtr bbuf = GetBackBuffer();
	int w,h;
	this->GetDimensions(w,h);
	g2dWindowDrawUtilDX11::DrawImage(bbuf, w, h, i_DX1, i_DY1, i_SX1, i_SY1, i_SX2, i_SY2, i_Image);
	bbuf->Release();
}

//------------------------------------------------------------------------
// set up device to render to our window
//------------------------------------------------------------------------
void g2dWindowDX11::configure_device()
{
	g2dDX11Global::SetRenderTargets(this->m_pBackBuffer, this->m_DepthStencil->GetDepthView());

	D3D11_VIEWPORT vprt;
	vprt.TopLeftX = vprt.TopLeftY = 0;
	vprt.Width = (FLOAT)m_BackBufferDesc.Width;
	vprt.Height = (FLOAT)m_BackBufferDesc.Height;
	vprt.MinDepth = 0;
	vprt.MaxDepth = 1;
	g2dDX11Global::g_pDeviceContext->RSSetViewports(1, &vprt);
}


//--------------------------------------------------------------------
//	MarkFrame can be called when a frame is completed drawing
//--------------------------------------------------------------------
void g2dWindowDX11::mark_frame()
{
	const int c_FramesToCount = 10;
	m_Frames++;
	if( m_Frames == c_FramesToCount )
	{
		float end_time = appTime::GetTime();
		float elapsed = end_time - m_StartTime;

		m_FrameRate = float(c_FramesToCount) / elapsed;
		//DBG_LOG("avg sec/frame " << 1.0f/m_FrameRate << " fps=" << m_FrameRate);
		m_StartTime = end_time;
		m_Frames = 0;
	}
}

//------------------------------------------------------------------------
//	Returns true if this object has allocated a depth buffer.
//------------------------------------------------------------------------
bool g2dWindowDX11::GetHasDepthBuffer() const
{
	return m_DepthStencil != NULL;
}

shared_ptr<g2dDepthStencilBuffer> g2dWindowDX11::GetDepthStencilBuffer() const
{
	return m_DepthStencil;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void g2dWindowDX11::SetDepthBuffer(shared_ptr<g2dDepthStencilBuffer> i_DepthStencil)
{
	m_DepthStencil = std::dynamic_pointer_cast<g2dDepthStencilBufferDX11>(i_DepthStencil);
}

