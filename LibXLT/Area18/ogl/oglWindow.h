#pragma once

#include "Graphics/g2d/g2dWindow.hpp"

#include "Graphics/g2d/g2dPFD.hpp"

#include "Area18/ogl/oglTypes.hpp"

class oglContext;

class oglWindow :
	public g2dWindow
{
public:
	oglWindow(HWND i_Hwnd, bool i_bOwnHwnd = false, 
		int i_Width = -1, int i_Height = -1);
	virtual ~oglWindow(void);

	//------------------------------------------------------------------------
	//	IsOpen returns true if the window is still open
	//------------------------------------------------------------------------
	virtual bool IsOpen() const;

	//------------------------------------------------------------------------
	// Access to HWND
	//------------------------------------------------------------------------
	virtual void* GetHandle();

	//------------------------------------------------------------------------
	//	EnableDebugOverlay switches the state of the debug text overlay.
	//------------------------------------------------------------------------
	virtual void EnableDebugOverlay(bool i_bEnabled);

	//------------------------------------------------------------------------
	//	SetDebugInfo causes the the given text to displayed at the given line
	//	of the debug text overlay.  The debug overlay is toggled on and off
	//	by the user; currently, this is done with the tilde key.
	//	To remove a debug info, call the function with i_Text == NULL.
	//------------------------------------------------------------------------
	virtual void SetDebugInfo(int i_Line, const char* i_Text);

	//------------------------------------------------------------------------
	//	DrawRect draws a rectangle of the given color between the two locations
	//	given (in client coordinates).  The rectangle will include the
	//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
	//	for spacing adjacent rectangles.
	//	The rectangle will be silently clipped or rejected by the current
	//	screen viewing area.
	//------------------------------------------------------------------------
	virtual void DrawRect(	int i_X1,
							int i_Y1,
							int i_X2,
							int i_Y2,
							const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	DrawText draws the given text such that it's upper left corner is
	//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
	//	current viewing area.
	//------------------------------------------------------------------------
//this is needed since WinUser.h #defines this as DrawTextA or DrawTextW
#undef DrawText
	virtual void DrawText(	int i_X,
							int i_Y,
							g2dFontHandle i_Font,
							const itString& i_Text,
							const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	This DrawImage blits the given image to the screen so that it's upper left
	//	corner will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	virtual void DrawImage(	int i_DX1,
							int i_DY1,
							const g2dImage& i_Image);

	//------------------------------------------------------------------------
	//	This DrawImage blits a rectangular section of the given image to the
	//	screen.  The rectangle of the source image is defined by
	//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
	//	located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	virtual void DrawImage(	int i_DX1,
							int i_DY1,
							int i_SX1,
							int i_SY1,
							int i_SX2,
							int i_SY2,
							const g2dImage& i_Image);


	//------------------------------------------------------------------------
	// Resize window to new size
	//------------------------------------------------------------------------
	virtual void ResizeWindow(int i_Width, int i_Height);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Present();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SetTitle(const itString& i_Title);

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	virtual void BeginScene();

	//------------------------------------------------------------------------
	// similar to BeginScene, call this to make this the current target of all
	// subsequent rendering calls.  Need not be followed by EndScene.
	//------------------------------------------------------------------------
	virtual void MakeCurrent();

	//------------------------------------------------------------------------
	// call this to make this depth buffer the current depth buffer for all
	// subsequent rendering calls.  
	//------------------------------------------------------------------------
	virtual void MakeDepthCurrent();

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.  It will cause whatever drawing you have
	//	requested to be visible on the screen.
	//------------------------------------------------------------------------
	virtual void EndScene();

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color.
	//------------------------------------------------------------------------
	virtual void Clear(const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	Clear fills the screen with the given color and sets depth and stencil
	//------------------------------------------------------------------------
	virtual void Clear( const maFloatRGBA& i_Color, bool i_ClearDepth = false, float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0);

	//------------------------------------------------------------------------
	//	Clear fills the depthstencil buffer with the given values.
	//	If there is no depthstencil, then this does nothing.
	//------------------------------------------------------------------------
	virtual void ClearDepthStencil(float i_Depth = 1, bool i_ClearStencil = true, unsigned int i_Stencil = 0); 

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the render target.
	//------------------------------------------------------------------------
	virtual const g2dPFD& GetPixelFormat() const;

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the render target.
	//------------------------------------------------------------------------
	virtual void GetDimensions(int& o_Width, int& o_Height) const;

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	virtual bool GetHasDepthBuffer() const;

	virtual boost::shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

	virtual float GetFrameRateAverage() { return 0; }
	virtual float GetFrameRate() { return 0; }

	static void SetupPixelFormat(int i_nPixelFormat, const PIXELFORMATDESCRIPTOR& i_Pfd);

private:
	HWND m_Hwnd;
	// window must have CS_OWNDC
	HDC m_hDC;
	bool m_bOwnHwnd;
	g2dPFD m_PFD;
	oglContext* mContext;

	GLsizei mW, mH;

	static int l_nPixelFormat;
	static PIXELFORMATDESCRIPTOR l_PixelFormat;
};
