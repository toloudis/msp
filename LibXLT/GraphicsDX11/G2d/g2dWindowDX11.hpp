/****************************************************************************\
**	g2dWindowDX11.hpp
**
**		g2dWindowDX11.hpp supplies the D3D implementation for g2dSystem
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef G2D_WINDOWDX11_HPP
#error g2dWindowDX11.hpp multiply included
#endif
#define G2D_WINDOWDX11_HPP

#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif 
#ifndef G2D_DX11TYPES_HPP
#include "GraphicsDX11/g2d/g2dDX11Types.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#ifndef MA_RUNNINGAVERAGE_HPP
#include "Core/ma/maRunningAverage.hpp"
#endif



//============================================================================
//============================================================================
class g2dDebugDisplay;
class g2dDepthStencilBuffer;
class g2dDepthStencilBufferDX11;


//============================================================================
//============================================================================
class g2dWindowDX11 : public g2dWindow
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~g2dWindowDX11();

	//------------------------------------------------------------------------
	//	SetDebugInfo causes the the given text to displayed at the given line
	//	of the debug text overlay.  The debug overlay is toggled on and off
	//	by the user; currently, this is done with the tilde key.
	//	To remove a debug info, call the function with i_Text == NULL.
	//------------------------------------------------------------------------
	void SetDebugInfo(int i_Line, const char* i_Text);

	//------------------------------------------------------------------------
	// Toggle display of Debug text overlay. When you don't want to do it with
	// the tilde key.
	//------------------------------------------------------------------------
	void EnableDebugOverlay(bool i_bEnabled);

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
	//	DrawRect draws a rectangle of the given color between the two locations
	//	given (in client coordinates).  The rectangle will include the
	//	rows at i_X1 and i_Y1, but not i_X2 and i_Y2.  This is convenient
	//	for spacing adjacent rectangles.
	//	The rectangle will be silently clipped or rejected by the current
	//	screen viewing area.
	//------------------------------------------------------------------------
	void DrawRect(	int i_X1,
					int i_Y1,
					int i_X2,
					int i_Y2,
					const g2dRGBColor& i_Color);

//this is needed since WinUser.h #defines this as DrawTextA or DrawTextW
#undef DrawText

	//------------------------------------------------------------------------
	//	DrawText draws the given text such that it's upper left corner is
	//	at (i_X1, i_Y1).  It will be silently clipped or rejected by the
	//	current viewing area.
	//------------------------------------------------------------------------
	void DrawText(	int i_X,
					int i_Y,
					g2dFontHandle i_Font,
					const itString& i_Text,
					const g2dRGBColor& i_Color);

	//------------------------------------------------------------------------
	//	This DrawImage blits the given image to the screen so that it's upper left
	//	corner will be located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void DrawImage(	int i_DX1,
					int i_DY1,
					const g2dImage& i_Image);

	//------------------------------------------------------------------------
	//	This DrawImage blits a rectangular section of the given image to the
	//	screen.  The rectangle of the source image is defined by
	//	(i_SX1, i_SY1) - (i_SX2, i_SY2).  The point (i_SX1, i_SY1) will be
	//	located at (i_DX1, i_DY1).
	//------------------------------------------------------------------------
	void DrawImage(	int i_DX1,
					int i_DY1,
					int i_SX1,
					int i_SY1,
					int i_SX2,
					int i_SY2,
					const g2dImage& i_Image);

	//------------------------------------------------------------------------
	// Access to buffers
	//------------------------------------------------------------------------
	// buffer MUST be release()'ed !!!!
	virtual g2dD3D11RenderTargetPtr GetBackBuffer() = 0;

	//------------------------------------------------------------------------
	//	IsOpen returns true if the window is still open
	//------------------------------------------------------------------------
	virtual bool IsOpen() const = 0;

	//------------------------------------------------------------------------
	//	Returns true if this object has allocated a depth buffer.
	//------------------------------------------------------------------------
	virtual bool GetHasDepthBuffer() const;

	//------------------------------------------------------------------------
	//	GetDepthStencilBuffer accesses the depth/stencil if the target has one.
	//------------------------------------------------------------------------
	virtual shared_ptr<g2dDepthStencilBuffer> GetDepthStencilBuffer() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetDepthBuffer(shared_ptr<g2dDepthStencilBuffer> i_DepthStencil);

protected:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dWindowDX11();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void FreeBuffers();

	//------------------------------------------------------------------------
	//	FrameRate information
	//------------------------------------------------------------------------
	virtual float GetFrameRateAverage();
	virtual float GetFrameRate();

protected:
	g2dD3D11RenderTargetPtr m_pBackBuffer;

	shared_ptr<g2dDepthStencilBufferDX11> m_DepthStencil;

	IDXGISwapChain* m_pSwapChain;
	D3D11_TEXTURE2D_DESC m_BackBufferDesc;

	ID2D1RenderTarget* m_pBackBuffer2D;
	ID2D1SolidColorBrush* m_pSolidBrush2D;

private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void configure_device();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void mark_frame();

private:
	g2dDebugDisplay*	m_pDebugDisplay;

	//	frame rate counter stuff
	maRunningAverage	m_FPSAverage;
	float m_StartTime;
	float m_FrameRate;
	int m_Frames;
};
