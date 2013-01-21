#include "Area18/ogl/oglSystem2D.h"

#include "Area18/ogl/oglWindow.h"
#include "Area18/g2d/g2dFontUtilGL.hpp"

#include "Core/app/appApplication.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/it/itString.hpp"

oglSystem2D::oglSystem2D(void)
{
	mFontImpl = new g2dFontUtilGL();
	g2dFontUtil::SetImplementation(mFontImpl);

}

oglSystem2D::~oglSystem2D(void)
{

	// Delete windows
	envSTLHelpers::DeleteContainer(m_Windows);

	g2dFontUtil::SetImplementation(NULL);
	delete mFontImpl;
}

//------------------------------------------------------------------------
//	CreateAppWindow creates a single window	of the given width and
//  height with the upper left corner at i_X and i_Y to be the main
//	window for the application.  If the user specifies invalid
//	coordinates a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y)
{
	appApplication::SetMainWindowSize(i_X, i_Y, i_Width, i_Height);

	oglWindow* window = new oglWindow((HWND)appApplication::GetMainWindowHandle());
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
//	CreateAppWindow creates a window to be used within a larger
//	application form. Pass in the appropriate OS handle to define
//	the size and location of the window (HWND for MS Windows).
//  If the user specifies invalid coordinates a
//	g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateAppWindow(void* i_Handle)
{
	oglWindow* window = new oglWindow((HWND)i_Handle);
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
// Creates sub window within given window handle.
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateSubWindow(void* i_Handle)
{
	oglWindow* window = new oglWindow((HWND)i_Handle);
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
// Create sub window in new window with given size and position
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y)
{
	itString title("");
	HWND hwnd = (HWND)appApplication::CreateSubWindow(i_Width, i_Height, i_X, i_Y, title);

	oglWindow* window = new oglWindow(hwnd, true, i_Width, i_Height);
	m_Windows.push_back(window);
	return window;
}

//------------------------------------------------------------------------
//	InitializeFullScreen initializes the drawing system to use the whole
//	screen in the given resolution.  If an unsupported mode is
//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateFullScreen(int i_Width, int i_Height, int i_BitDepth)
{
	DBG_ERROR("Fullscreen not supported.");
	return NULL;
}

//------------------------------------------------------------------------
//	InitializeFullScreen initializes a new sub widnow to use the whole
//	screen in the given resolution.  If an unsupported mode is
//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
//------------------------------------------------------------------------
g2dWindow* oglSystem2D::CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth)
{
	DBG_ERROR("Fullscreen not supported.");
	return NULL;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void oglSystem2D::DestroyWindow(g2dWindow* i_pWindow)
{
	envSTLHelpers::DeleteOneValue(m_Windows, i_pWindow);
}

//------------------------------------------------------------------------
// warning: this call is potentially expensive!
//------------------------------------------------------------------------
void oglSystem2D::ClearManagedResources()
{
}
