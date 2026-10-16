/****************************************************************************\
**  g2dWindowPrimaryDX11.hpp
**
**      g2dWindowPrimaryDX11.hpp supplies the D3D implementation
**	for the primary window for a device.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_WINDOWPRIMARYDX11_HPP
#error g2dWindowPrimaryDX11.hpp multiply included
#endif
#define G2D_WINDOWPRIMARYDX11_HPP

#ifndef G2D_WINDOWDX11_HPP
#include "GraphicsDX11/g2d/g2dWindowDX11.hpp"
#endif


//============================================================================
//============================================================================
class g2dWindowPrimaryDX11 : public g2dWindowDX11
{
public:
	//------------------------------------------------------------------------
	// General window constructor
	//------------------------------------------------------------------------
	g2dWindowPrimaryDX11(HWND i_Hwnd, bool i_bOwnHwnd, 
		int i_Width = -1, int i_Height = -1);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~g2dWindowPrimaryDX11();

	//------------------------------------------------------------------------
	//	SetDebugInfo causes the the given text to displayed at the given line
	//	of the debug text overlay.  The debug overlay is toggled on and off
	//	by the user; currently, this is done with the tilde key.
	//	To remove a debug info, call the function with i_Text == NULL.
	//------------------------------------------------------------------------
	//void SetDebugInfo(int i_Line, const char* i_Text);

	//------------------------------------------------------------------------
	// BeginScene must be called before rendering to this window
	//------------------------------------------------------------------------
	virtual void BeginScene();

	//------------------------------------------------------------------------
	//	EndScene must be called when you are done with the drawing operations
	//	on the current frame.  It will cause whatever drawing you have
	//	requested to be visible on the screen.
	//------------------------------------------------------------------------
	virtual void EndScene();

	//------------------------------------------------------------------------
	// Resize window to new size
	//------------------------------------------------------------------------
	virtual void ResizeWindow(int i_Width, int i_Height);

	//------------------------------------------------------------------------
	//	Render offscreen at the given size and scale the image to fit the
	//	window, centered with black bars, in Present.  0, 0 turns this off.
	//------------------------------------------------------------------------
	virtual void SetRenderResolution(int i_Width, int i_Height);

	//------------------------------------------------------------------------
	//	Where the rendered image is drawn in the window, in window pixels
	//------------------------------------------------------------------------
	virtual void GetPresentRect(int& o_X, int& o_Y, int& o_Width, int& o_Height) const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void Present();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void SetTitle(const itString& i_Title);

	//------------------------------------------------------------------------
	//	IsOpen returns true if the window is still open
	//------------------------------------------------------------------------
	bool IsOpen() const;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual g2dD3D11RenderTargetPtr GetBackBuffer();// { return m_pColorBuffer; }

	//------------------------------------------------------------------------
	// Access to HWND
	//------------------------------------------------------------------------
	virtual void* GetHandle();

private:
	//------------------------------------------------------------------------
	// private init functions
	//------------------------------------------------------------------------
	void initialize_window(HWND i_Hwnd, int i_Width, int i_Height, g2dPFD& o_PixelFormat);
	void initialize_window(int i_Width, int i_Height, int i_X, int i_Y, g2dPFD& o_PixelFormat);
	void initialize_fullscreen(int i_Width, int i_Height, int i_BitDepth, g2dPFD& o_PixelFormat);
	HRESULT InitRenderTargetInfo();
	HRESULT CreateDepthBuffer(int i_Width, int i_Height);
	HRESULT create_offscreen_buffer(int i_Width, int i_Height);
	void release_color_buffers();
	bool is_offscreen() const;

private:

	HWND m_Hwnd;
	bool m_bOwnHwnd;

	// size of the swap chain (the window's client area)
	int m_WindowWidth;
	int m_WindowHeight;

	// requested render resolution, 0 when rendering at the window size
	int m_RenderWidth;
	int m_RenderHeight;

	// when rendering offscreen, m_pBackBuffer is the offscreen target and
	// these hold the swap chain's target and the offscreen texture
	g2dD3D11RenderTargetPtr m_pSwapChainBuffer;
	g2dD3D11TexturePtr m_pOffscreenTexture;
	g2dD3D11ShaderResourcePtr m_pOffscreenTextureView;
};
