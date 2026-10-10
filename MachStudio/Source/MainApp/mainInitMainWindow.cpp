/****************************************************************************\
**	mainInitMainWindow.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitMainWindow.hpp"
#ifdef MSP_GUI_QT
#include "MainApp/qtGUI/qtMainForm.hpp"
#else
#include "MainApp/wxGUI/wxMainForm.hpp"
#endif

#include "Features/Capture/cptrModeRender.hpp"
#include "Support/mnm/mnmConstants.hpp"

#include "Core/CoreLayer.hpp"
#include "Core/app/appApplication.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiSplashScreen.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/qui/quiPackage.hpp"
#else
#include "ToolUIWx/wui/wuiPackage.hpp"
#endif

#include "Core/app/private/appApplicationPAC.hpp"	// FIX - if this is sorted with includes, it compiles with errors.



//--------------------------------------------------------------------
// Initializes library packages
//--------------------------------------------------------------------
mainInitMainWindow::mainInitMainWindow()
{
#ifdef USE_WXWIDGETS
	// create the main application window
	m_pMainFrame = new wxMainForm(wxString(mnmConstants::c_PRODUCT_FOR_DISPLAY, wxConvUTF8));
	appApplicationPAC::SetHWND( (HWND)(m_pMainFrame->GetHandle()) );

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	//m_pMainFrame->Show(true);
#elif defined(USE_QT)
	// create the main application window
	m_pMainFrame = new qtMainForm(QString::fromWCharArray(mnmConstants::c_PRODUCT_FOR_DISPLAY));
	appApplicationPAC::SetHWND( (HWND)(m_pMainFrame->winId()) );
#else // USE_WXWIDGETS

	const int app_width = 800;
	const int app_height = 600;
	const int app_x = 100;
	const int app_y = 100;
	appApplication::CreateMainWindow( app_width, app_height, app_x, app_y, itString(mnmConstants::c_PRODUCT_FOR_DISPLAY) );

	// This line causes the capture mode to use the main application window we just opened
	// instead of opening a new sub-window.
	//cptrModeRender::SetUseApplicationWindow(true);

	// Initialize Tool/gui with non-wxWidgets implementations
	wuiPackage::Initialize();
#endif // USE_WXWIDGETS

#ifndef _DEBUG
	guiSplashScreen::SetDoSplashTimeout(true);
	fsLocator splashPath = gfPaths::GetPath(gfPaths::e_ExePath);
	splashPath.Push("Data");
	splashPath.Push("Load.png");
	if (fsFileUtil::FileExists(splashPath))
	{
		guiSplashScreen::StartUp(splashPath, "Loading...Please Wait");
	}
#endif

	// does this have to be after the above code
	// because we have to have created an application window first?
	CoreLayer::Init();
	fsResourceTracker::Init(); // should go in the Layer Init function above
}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitMainWindow::~mainInitMainWindow()
{
	// In wxWidgets, there isn't any need to delete the main frame,
	// it will be closed when wxWidgets shuts down

#ifdef USE_QT
	// Normally already deleted when the application loop ended
	DestroyMainFrame();
#elif !defined(USE_WXWIDGETS)
	// Even without wxWidgets, we need to use the wuiPackage.
	// wxMainForm would usually clean up the package, but here we have to
	// do it ourselves
	wuiPackage::DeInitialize();
#endif

	cmaCommandMgr::DeInitialize();
	fsResourceTracker::CleanUp();
	CoreLayer::CleanUp();
}

#ifdef USE_WXWIDGETS
//--------------------------------------------------------------------
// Return pointer to main frame created
//--------------------------------------------------------------------
wxMainForm* mainInitMainWindow::GetMainFrame()
{
	return m_pMainFrame;
}
#elif defined(USE_QT)
//--------------------------------------------------------------------
// Return pointer to main frame created
//--------------------------------------------------------------------
qtMainForm* mainInitMainWindow::GetMainFrame()
{
	return m_pMainFrame;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mainInitMainWindow::DestroyMainFrame()
{
	delete m_pMainFrame;
	m_pMainFrame = NULL;
}
#endif