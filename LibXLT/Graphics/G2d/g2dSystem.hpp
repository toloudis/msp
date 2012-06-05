/****************************************************************************\
**  g2dSystem.hpp
**
**      g2dSystem.hpp supplies operations for the general 2d system which
**	can create and manage more than one "window".
**
**	Note: the windows created are owned by the system. It will delete
**	them in the destructor.  Use the DestroyWindow() function to destroy
**	it before then.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_SYSTEM_HPP
#error g2dSystem.hpp multiply included
#endif
#define G2D_SYSTEM_HPP


//============================================================================
//============================================================================
class g2dWindow;


//============================================================================
//============================================================================
class g2dSystem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~g2dSystem();

	//------------------------------------------------------------------------
	//	CreateAppWindow creates a single window	of the given width and
	//  height with the upper left corner at i_X and i_Y to be the main
	//	window for the application.  If the user specifies invalid
	//	coordinates a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y) = 0;

	//------------------------------------------------------------------------
	//	CreateAppWindow creates a window to be used within a larger
	//	application form. Pass in the appropriate OS handle to define
	//	the size and location of the window (HWND for MS Windows).
	//  If the user specifies invalid coordinates a
	//	g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateAppWindow(void* i_Handle) = 0;

	//------------------------------------------------------------------------
	// Creates sub window within given window handle.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubWindow(void* i_Handle) = 0;

	//------------------------------------------------------------------------
	// Create sub window in new window with given size and position
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y) = 0;

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes the drawing system to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateFullScreen(int i_Width, int i_Height, int i_BitDepth) = 0;

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes a new sub widnow to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth) = 0;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void DestroyWindow(g2dWindow* i_pWindow) = 0;

	//------------------------------------------------------------------------
	// warning: this call is potentially expensive!
	//------------------------------------------------------------------------
	virtual void ClearManagedResources() = 0;

	//------------------------------------------------------------------------
	//	TestFullscreenSupported will return true if the requested
	//	resolution change is supported by the hardware.
	//------------------------------------------------------------------------
	//virtual bool TestFullscreenSupported(int i_Width, int i_Height, int i_BitDepth);

	//------------------------------------------------------------------------
	//	Sets the gamma correction for the screen
	//------------------------------------------------------------------------
	//virtual float GetGamma();
	//virtual void SetGamma(float i_Level);

	//------------------------------------------------------------------------
	//	EnableDebugDisplay allows the client to control whether the debug
	//	display appears when the user presses the tilde.
	//------------------------------------------------------------------------
	//virtual void EnableDebugDisplay(bool i_Enable);
};
