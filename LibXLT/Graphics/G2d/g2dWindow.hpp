/****************************************************************************\
**  g2dWindow.hpp
**
**      g2dWindow.hpp defines the base class for a window in an application
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_WINDOW_HPP
#error g2dWindow.hpp multiply included
#endif
#define G2D_WINDOW_HPP

#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#ifndef G2D_PFD_HPP
#include "Graphics/g2d/g2dPFD.hpp"
#endif
#ifndef G2D_FONTHANDLE_HPP
#include "Graphics/g2d/g2dFontHandle.hpp"
#endif


//============================================================================
//============================================================================
class g2dImage;
class itString;


//============================================================================
//============================================================================
class g2dWindow : public g2dRenderTarget
{
protected:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dWindow();

public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dWindow();

	//------------------------------------------------------------------------
	//	IsWindowed returns true if we are in windowed mode
	//------------------------------------------------------------------------
	bool IsWindowed() const;

	//------------------------------------------------------------------------
	//	IsOpen returns true if the window is still open
	//------------------------------------------------------------------------
	virtual bool IsOpen() const = 0;

	//------------------------------------------------------------------------
	// Access to HWND
	//------------------------------------------------------------------------
	virtual void* GetHandle() = 0;

	//------------------------------------------------------------------------
	//	GetPixelFormat returns the current pixel format of the screen.
	//------------------------------------------------------------------------
	virtual const g2dPFD& GetPixelFormat() const;

	//------------------------------------------------------------------------
	//	Return the pixel format of the backbuffer surface.
	//------------------------------------------------------------------------
	virtual const g2dPFD& GetBackBufferPixelFormat() const;

	//------------------------------------------------------------------------
	//	GetDimensions returns the width and height of the screen.
	//------------------------------------------------------------------------
	virtual void GetDimensions(int& o_Width, int& o_Height) const;

	//------------------------------------------------------------------------
	//	GetBitDepth returns the BitDepth of the screen.
	//------------------------------------------------------------------------
	void GetBitDepth(int& o_BitDepth) const;

	//------------------------------------------------------------------------
	//	SetVirtualResolution sets the resolution that the gui scales itself to
	//------------------------------------------------------------------------
	void SetVirtualResolution( int i_nWidth, int i_nHeight );

	//------------------------------------------------------------------------
	//	GetVirtualResolution gets the resolution that the gui scales itself to
	//------------------------------------------------------------------------
	void GetVirtualResolution( int& o_nWidth, int& o_nHeight ) const;

	//------------------------------------------------------------------------
	//	EnableDebugOverlay switches the state of the debug text overlay.
	//------------------------------------------------------------------------
	virtual void EnableDebugOverlay(bool i_bEnabled) = 0;

	//------------------------------------------------------------------------
	//	SetDebugInfo causes the the given text to displayed at the given line
	//	of the debug text overlay.  The debug overlay is toggled on and off
	//	by the user; currently, this is done with the tilde key.
	//	To remove a debug info, call the function with i_Text == NULL.
	//------------------------------------------------------------------------
	virtual void SetDebugInfo(int i_Line, const char* i_Text) = 0;

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
							const g2dRGBColor& i_Color) = 0;

	//------------------------------------------------------------------------
	//	DrawText draws the given text such that it's upper left corner is
	//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
	//	current viewing area.
	//------------------------------------------------------------------------
	virtual void DrawText(	int i_X,
							int i_Y,
							g2dFontHandle i_Font,
							const itString& i_Text,
							const g2dRGBColor& i_Color) = 0;

	//------------------------------------------------------------------------
	//	This DrawImage blits the given image to the screen so that it's upper left
	//	corner will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	virtual void DrawImage(	int i_DX1,
							int i_DY1,
							const g2dImage& i_Image) = 0;

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
							const g2dImage& i_Image) = 0;

	//------------------------------------------------------------------------
	// Resize window to new size
	//------------------------------------------------------------------------
	virtual void ResizeWindow(int i_Width, int i_Height) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Present() = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SetTitle(const itString& i_Title) = 0;

	//------------------------------------------------------------------------
	//	FrameRate information
	//------------------------------------------------------------------------
	virtual float GetFrameRateAverage() = 0;
	virtual float GetFrameRate() = 0;

protected:
	//------------------------------------------------------------------------
	// Allows derived classes to set these properties based on implementation
	//------------------------------------------------------------------------
	void SetWindowProperties(bool i_Windowed, const g2dPFD &i_PFD, const g2dPFD& i_backPFD,
							int i_Width, int i_Height, int i_BitDepth);

protected:
	int m_Width;
	int m_Height;
	int m_BitDepth;

private:
	bool m_bIsWindowed;
	g2dPFD m_PFD;
	g2dPFD m_backBufPFD;

	int m_VirtualWidth; // = 800;
	int m_VirtualHeight; // = 600;
};

