/****************************************************************************\
**  g2dSystemDX11.hpp
**
**      g2dSystemDX11.hpp supplies the D3D implementation for g2dSystem
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef G2D_SYSTEMDX11_HPP
#error g2dSystemDX11.hpp multiply included
#endif
#define G2D_SYSTEMDX11_HPP

#ifndef G2D_SYSTEM_HPP
#include "Graphics/g2d/g2dSystem.hpp"
#endif
#ifndef G2D_SCREENRESOLUTION_HPP
#include "Graphics/g2d/g2dScreenResolution.hpp"
#endif

#include <vector>


//============================================================================
//============================================================================
class g2dWindowDX11;
class g2dFontUtilDX11;
class g2dImageCreateDX11;
class g2dImageSaveDX11;
class g2dResourceCounterDX11;
class effShaderSDKDX11;

//============================================================================
//============================================================================
class g2dSystemDX11 : public g2dSystem
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dSystemDX11(int i_Adapter = 0);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	~g2dSystemDX11();

	//------------------------------------------------------------------------
	//	CreateAppWindow creates a single window	of the given width and
	//  height with the upper left corner at i_X and i_Y to be the main
	//	window for the application.  If the user specifies invalid
	//	coordinates a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	g2dWindow* CreateAppWindow(int i_Width, int i_Height, int i_X, int i_Y);

	//------------------------------------------------------------------------
	//	CreateSubWindow creates a window to be used within a larger
	//	application form. Pass in the appropriate OS handle to define
	//	the size and location of the window (HWND for MS Windows).
	//  If the user specifies invalid coordinates a
	//	g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	g2dWindow* CreateAppWindow(void* i_Handle);

	//------------------------------------------------------------------------
	// Creates sub window within given window handle.
	//------------------------------------------------------------------------
	g2dWindow* CreateSubWindow(void* i_Handle);

	//------------------------------------------------------------------------
	// Create sub window in new window with given size and position
	//------------------------------------------------------------------------
	g2dWindow* CreateSubWindow(int i_Width, int i_Height, int i_X, int i_Y);

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes the drawing system to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	g2dWindow* CreateFullScreen(int i_Width, int i_Height, int i_BitDepth);

	//------------------------------------------------------------------------
	//	InitializeFullScreen initializes a new sub window to use the whole
	//	screen in the given resolution.  If an unsupported mode is
	//	requested, a g2dUnsupportedScreenModeX exception will be thrown.
	//------------------------------------------------------------------------
	g2dWindow* CreateSubFullScreen(int i_Width, int i_Height, int i_BitDepth);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void DestroyWindow(g2dWindow* i_Window);

	//------------------------------------------------------------------------
	// warning: this call is potentially expensive!
	//------------------------------------------------------------------------
	virtual void ClearManagedResources();

private:
	std::vector<g2dScreenResolution> m_Resolutions;
	std::vector<g2dWindowDX11*> m_Windows;
	g2dResourceCounterDX11* m_pResourceCounterImpl;
	g2dFontUtilDX11* m_pFontImpl;
	g2dImageCreateDX11* m_pImageCreator;
	g2dImageSaveDX11* m_pImageSaver;
	effShaderSDKDX11* m_pShaderSDK;

	void CreateDevice(void* i_Hwnd, int i_Adapter);

};
