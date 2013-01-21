#pragma once

#include "Graphics/g2d/g2dSystem.hpp"
#include <vector>
class oglWindow;
class g2dFontUtilGL;

class oglSystem2D :
	public g2dSystem
{
public:
	oglSystem2D(void);
	virtual ~oglSystem2D(void);

	//------------------------------------------------------------------------
	//	CreateAppWindow creates a single window	of the given width and
	//  height with the upper left corner at i_X and i_Y to be the main
	//	window for the application.  If the user specifies invalid
	//	coordinates a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y);

	//------------------------------------------------------------------------
	//	CreateAppWindow creates a window to be used within a larger
	//	application form. Pass in the appropriate OS handle to define
	//	the size and location of the window (HWND for MS Windows).
	//  If the user specifies invalid coordinates a
	//	g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateAppWindow(void* i_Handle);

	//------------------------------------------------------------------------
	// Creates sub window within given window handle.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubWindow(void* i_Handle);

	//------------------------------------------------------------------------
	// Create sub window in new window with given size and position
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y);

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes the drawing system to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateFullScreen(int i_Width, int i_Height, int i_BitDepth);

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes a new sub widnow to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	virtual g2dWindow* CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual void DestroyWindow(g2dWindow* i_pWindow);

	//------------------------------------------------------------------------
	// warning: this call is potentially expensive!
	//------------------------------------------------------------------------
	virtual void ClearManagedResources();

private:
	std::vector<oglWindow*> m_Windows;
	g2dFontUtilGL* mFontImpl;

};
