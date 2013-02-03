#include "oglWindow.h"

#include "Area18/Area18Layer.hpp"
#include "Area18/ogl/oglContext.h"

#include "Core/app/appApplication.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maFloatRGBA.hpp"

int oglWindow::l_nPixelFormat = 0;
PIXELFORMATDESCRIPTOR oglWindow::l_PixelFormat;
void oglWindow::SetupPixelFormat(int i_nPixelFormat, const PIXELFORMATDESCRIPTOR& i_Pfd)
{
	l_nPixelFormat = i_nPixelFormat;
	l_PixelFormat = i_Pfd;
}

oglWindow::oglWindow(HWND i_Hwnd, bool i_bOwnHwnd, 
					 int i_Width, int i_Height)
{
	m_bOwnHwnd = i_bOwnHwnd;
	m_Hwnd = i_Hwnd;

	// window should have CS_OWNDC so I can hold onto this dc:
	m_hDC = ::GetDC(m_Hwnd);
	mContext = new oglContext(m_hDC, Area18Layer::defaultContext());

	int width = i_Width;
	int height = i_Height;
	if (width == -1 || height == -1)
	{
		RECT rect;
		::GetClientRect(i_Hwnd, &rect);
		width = rect.right - rect.left;
		height = rect.bottom - rect.top;
	}
	mW = width;
	mH = height;
}

oglWindow::~oglWindow(void)
{
	// window should have CS_OWNDC so I can hold onto this dc:
	::ReleaseDC(m_Hwnd, m_hDC);

	delete mContext;

	if (m_bOwnHwnd)
		appApplication::DestroySubWindow(m_Hwnd);
}

//------------------------------------------------------------------------
//	IsOpen returns true if the window is still open
//------------------------------------------------------------------------
bool oglWindow::IsOpen() const
{
	return (IsWindow(m_Hwnd) != 0);
}

//------------------------------------------------------------------------
// Access to HWND
//------------------------------------------------------------------------
void* oglWindow::GetHandle()
{
	return m_Hwnd;
}

//------------------------------------------------------------------------
//	EnableDebugOverlay switches the state of the debug text overlay.
//------------------------------------------------------------------------
void oglWindow::EnableDebugOverlay(bool i_bEnabled)
{
}

//------------------------------------------------------------------------
//	SetDebugInfo causes the the given text to displayed at the given line
//	of the debug text overlay.  The debug overlay is toggled on and off
//	by the user; currently, this is done with the tilde key.
//	To remove a debug info, call the function with i_Text == NULL.
//------------------------------------------------------------------------
void oglWindow::SetDebugInfo(int i_Line, const char* i_Text)
{
}

//------------------------------------------------------------------------
//	DrawRect draws a rectangle of the given color between the two locations
//	given (in client coordinates).  The rectangle will include the
//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
//	for spacing adjacent rectangles.
//	The rectangle will be silently clipped or rejected by the current
//	screen viewing area.
//------------------------------------------------------------------------
void oglWindow::DrawRect(	int i_X1,
						int i_Y1,
						int i_X2,
						int i_Y2,
						const g2dRGBColor& i_Color)
{
}

//this is needed since WinUser.h #defines this as DrawTextA or DrawTextW
#undef DrawText
//------------------------------------------------------------------------
//	DrawText draws the given text such that it's upper left corner is
//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
//	current viewing area.
//------------------------------------------------------------------------
void oglWindow::DrawText(	int i_X,
						int i_Y,
						g2dFontHandle i_Font,
						const itString& i_Text,
						const g2dRGBColor& i_Color)
{
}

//------------------------------------------------------------------------
//	This DrawImage blits the given image to the screen so that it's upper left
//	corner will be located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void oglWindow::DrawImage(	int i_DX1,
						int i_DY1,
						const g2dImage& i_Image)
{
}

//------------------------------------------------------------------------
//	This DrawImage blits a rectangular section of the given image to the
//	screen.  The rectangle of the source image is defined by
//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
//	located at (i_DX1, i_DY1).
//------------------------------------------------------------------------
void oglWindow::DrawImage(	int i_DX1,
						int i_DY1,
						int i_SX1,
						int i_SY1,
						int i_SX2,
						int i_SY2,
						const g2dImage& i_Image)
{
}


//------------------------------------------------------------------------
// Resize window to new size
//------------------------------------------------------------------------
void oglWindow::ResizeWindow(int i_Width, int i_Height)
{
	mW = i_Width;
	mH = i_Height;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void oglWindow::Present()
{
	::SwapBuffers(m_hDC);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void oglWindow::SetTitle(const itString& i_Title)
{
}

//------------------------------------------------------------------------
// BeginScene must be called before rendering to this window
//------------------------------------------------------------------------
void oglWindow::BeginScene()
{
}

//------------------------------------------------------------------------
// similar to BeginScene, call this to make this the current target of all
// subsequent rendering calls.  Need not be followed by EndScene.
//------------------------------------------------------------------------
void oglWindow::MakeCurrent()
{
	mContext->makeCurrent(m_hDC);
}

//------------------------------------------------------------------------
// call this to make this depth buffer the current depth buffer for all
// subsequent rendering calls.  
//------------------------------------------------------------------------
void oglWindow::MakeDepthCurrent()
{
}

//------------------------------------------------------------------------
//	EndScene must be called when you are done with the drawing operations
//	on the current frame.  It will cause whatever drawing you have
//	requested to be visible on the screen.
//------------------------------------------------------------------------
void oglWindow::EndScene()
{
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color.
//------------------------------------------------------------------------
void oglWindow::Clear(const g2dRGBColor& i_Color)
{
	MakeCurrent();
	float r,g,b,a;
	r = i_Color.GetRed()/255.0f;
	g = i_Color.GetGreen()/255.0f;
	b = i_Color.GetBlue()/255.0f;
	a = 1;
	glClearColor(r,g,b,a);
	glClear(GL_COLOR_BUFFER_BIT);
}

//------------------------------------------------------------------------
//	Clear fills the screen with the given color and sets depth and stencil
//------------------------------------------------------------------------
void oglWindow::Clear(const maFloatRGBA& i_Color, bool i_ClearDepth /*= false*/, 
					  float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/)
{
	glClearColor(i_Color.GetRed(), i_Color.GetGreen(), i_Color.GetBlue(), i_Color.GetAlpha());
	GLbitfield mask = GL_COLOR_BUFFER_BIT;
	if (i_ClearDepth)
	{
		mask |= GL_DEPTH_BUFFER_BIT;
		glClearDepthf(i_Depth);
	}
	if (i_ClearStencil)
	{
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(i_Stencil);
	}
	glClear(mask);
}

//------------------------------------------------------------------------
//	Clear fills the depthstencil buffer with the given values.
//	If there is no depthstencil, then this does nothing.
//------------------------------------------------------------------------
void oglWindow::ClearDepthStencil(float i_Depth /*= 1*/, bool i_ClearStencil /*= true*/, unsigned int i_Stencil /*= 0*/) 
{
	GLbitfield mask = GL_DEPTH_BUFFER_BIT;
	glClearDepthf(i_Depth);
	if (i_ClearStencil)
	{
		mask |= GL_STENCIL_BUFFER_BIT;
		glClearStencil(i_Stencil);
	}
	glClear(mask);
}

//------------------------------------------------------------------------
//	GetPixelFormat returns the current pixel format of the render target.
//------------------------------------------------------------------------
const g2dPFD& oglWindow::GetPixelFormat() const
{
	return m_PFD;
}

//------------------------------------------------------------------------
//	GetDimensions returns the width and height of the render target.
//------------------------------------------------------------------------
void oglWindow::GetDimensions(int& o_Width, int& o_Height) const
{
	o_Width = mW;
	o_Height = mH;
}

//------------------------------------------------------------------------
//	Returns true if this object has allocated a depth buffer.
//------------------------------------------------------------------------
bool oglWindow::GetHasDepthBuffer() const
{
	return true;
}

boost::shared_ptr<g2dDepthStencilBuffer> oglWindow::GetDepthStencilBuffer() const
{
	return boost::shared_ptr<g2dDepthStencilBuffer>();
}

