/****************************************************************************\
**	mainInitGraphics.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitGraphics.hpp"
#include "MainApp/mnmApp.hpp"
#include "MainApp/wxGUI/wxMainForm.hpp"
#include "MainApp/wxGUI/wxMainDropTarget.hpp"

#include "Features/RenderPanels/wxGUI/rpnRenderPanel.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Support/ptm/ptmControlFactoryTimeline.hpp"

#include "AudioDS/AudioDSLayer.hpp"
#include "AudioDS/Sn/snExceptionX.hpp"
#include "AudioDS/Sn/snSoundManager.hpp"
#include "AudioDS/Sn/snSoundSystem.hpp"
#include "Core/Dbg/dbgSystemInfo.hpp"
#include "Graphics/G2d/g2dExceptionX.hpp"
#include "Graphics/G2d/g2dFontUtil.hpp"
#include "Graphics/G3d/g3dExceptionX.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "ImportExport/ImportExportLayer.hpp"
#include "Input/in/inDeviceMgr.hpp"
#include "Input/In/inPackage.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmErrorCodes.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "ToolUIWx/pwx/pwxControlFactoryBase.hpp"
#include "ToolUIWx/pwx/pwxControlFactoryCustom.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "Area18/Area18Layer.hpp"
//#include "GraphicsDX11/GraphicsDX11Layer.hpp"


//============================================================================
//============================================================================
namespace
{
}	// end of namespace


//--------------------------------------------------------------------
// Initializes library packages
//--------------------------------------------------------------------
#ifdef USE_WXWIDGETS
mainInitGraphics::mainInitGraphics(wxMainForm *i_pMainFrame)
#else
mainInitGraphics::mainInitGraphics(int i_AppWidth,
								   int i_AppHeight,
								   int i_ViewOffsetX,
								   int i_ViewOffsetY )
#endif
:	m_pApp(NULL),
	m_bSuccessInit(false)
{	
	// needed before initialize_render()
	m_pApp = new mnmApp();
	mnmApp::SetActive( false );

	// initialize everything else
	//
	try 
	{
		GraphicsLayer::Init();

#ifdef USE_WXWIDGETS
		Area18Layer::Init(i_pMainFrame->GetRenderWindow()->GetHandle());
#else
		Area18Layer::Init(appApplication::GetMainWindowHandle());
#endif
		GraphicsLayer::InitGraphics(Area18Layer::GetSystem2D(), Area18Layer::GetSystem3D());
		Area18Layer::InitGraphics();

		fsLocator font_loc;
		font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) ); 
		font_loc.Push( "font-lucd00.png" );
		g2dFontUtil::SetGlobalFontBitmap(font_loc);

		AudioDSLayer::Init();
		ImportExportLayer::Init();

		cptrRenderUtil::Initialize();	// no deinitialize
		tmlnTimeLine::Initialize();

		dbgSystemInfo::logSystemInfo();

		//	set-up rendering window
		//
#ifdef USE_WXWIDGETS
		//wxSize size = i_pMainFrame->GetSize();
		//wxSize client_size = i_pMainFrame->GetClientSize();
		wxRect rect = i_pMainFrame->GetRenderWindow()->GetRect();
		m_pApp->InitializeRender( (void*)i_pMainFrame->GetRenderWindow()->GetHandle(),
			(void*)i_pMainFrame->GetRenderWindow()->GetHandle(),
			rect.GetWidth(), rect.GetHeight());

		tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
			maPoint2d( rect.GetWidth(), rect.GetHeight() ) );

		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			i_pMainFrame->GetRenderWindow()->SetRenderView(m_pApp->GetRenderView(0));
			i_pMainFrame->GetRenderWindow()->SetDropTarget(new wxMainDropTarget(m_pApp->GetRenderView(0))); // for drag & drop
			inDeviceMgr::SetWindowHandle( i_pMainFrame->GetRenderPane(0)->GetHandle() );
		}
#else // USE_WXWIDGETS
		m_pApp->InitializeRender( 0, 0, i_AppWidth, i_AppHeight );
		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			tma3dScreenUtil::SetWindowSize( maPoint2d( i_ViewOffsetX, i_ViewOffsetY ),
											maPoint2d( i_AppWidth, i_AppHeight ) );
		}
#endif // USE_WXWIDGETS

#ifdef USE_WXWIDGETS
		//	if InitializeRender didn't work, there will be no render views.
		//
		if (m_pApp->GetNumberOfRenderViews() > 0)
		{
			// Create other render views and hook up the render panes to the render views
			for (int i=1; i<4; ++i)
			{
				m_pApp->CreateRenderView( i_pMainFrame->GetRenderPane(i)->GetHandle() );
				i_pMainFrame->GetRenderPane(i)->SetRenderView(m_pApp->GetRenderView(i));
				i_pMainFrame->GetRenderPane(i)->SetDropTarget(new wxMainDropTarget(m_pApp->GetRenderView(i))); // for drag & drop
			}
		}
#endif // USE_WXWIDGETS
		DBG_TRACE("Render Window Init complete.");
		m_bSuccessInit = (m_pApp->GetNumberOfRenderViews() > 0);
	}
	catch (const g3dShaderLoadX& ex)
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorLoadShaders);
		std::string message("Failed to load shader: ");
		message += ex.GetShaderName();
		guiMessageBox::Show(message.c_str(), "Critical Error", guiMessageBox::e_OKOnly);
	}
	catch( const g2dScreenInitX& )
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorCreatingGraphics);
		guiMessageBox::Show("There is a problem initializing the graphics system. Please close all programs and try again. If the problem persists, your graphics hardware may be incompatible with DirectX 11. Try running Windows Update and updating your graphics drivers. If your OS is Windows Vista, try applying update KB971512.", "Error Initializing Graphics");
	}
	catch( const g2dHardwareCapabilityX& )
	{
		mnmAppUtil::SetErrorCode(mnmErrorCodes::c_ErrorCreatingGraphics);
		guiMessageBox::Show("Your graphics hardware may be incompatible with DirectX 11. Try running Windows Update and updating your graphics drivers. If your OS is Windows Vista, try applying update KB971512.", "Error Initializing Graphics");
	}

	if (m_bSuccessInit)
	{
#ifdef USE_WXWIDGETS
		pwxControlMgr::Initialize();
		pwxControlMgr::AddControlFactory( new pwxControlFactoryBase() );
		pwxControlMgr::AddControlFactory( new pwxControlFactoryCustom() );
		pwxControlMgr::AddControlFactory( new ptmControlFactoryTimeline() );
#endif // USE_WXWIDGETS
		DBG_TRACE("Property controls Init complete.");

		//SplashScreen::SetStatus("Initializing Sound, Input"); System::Threading::Thread::Sleep(1);
		//guiMessageBox::Show("Test Test", "Critical Test");

		inPackage::Init();
		mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick
		DBG_TRACE("Input Init complete.");

		try
		{
			snSoundSystem::Initialize();
		}
		catch (snSoundSystemCreateFailedX)
		{
			//	Don't show a dialog, just record the error
			//
			//guiMessageBox::Show("No sound card installed!", "Critical Error", guiMessageBox::e_OKOnly);
			DBG_ERROR("No sound card installed!");
		}
		snSoundManager::Initialize();
		DBG_TRACE("Sound Init complete.");

		//SplashScreen::SetStatus("Initializing Effects, Model"); System::Threading::Thread::Sleep(1);

		//	read the render prefs data
		//
		rndrPrefsMgr::ReadPrefs(rndrPrefsMgr::e_ViewportPrefs);
		//	commit the render prefs to g3d
		rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);

		DBG_TRACE("Graphics Init complete.");
	}
}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitGraphics::~mainInitGraphics()
{
	//	close it
	//
	snSoundManager::DeInitialize();
	snSoundSystem::DeInitialize();
	inPackage::CleanUp();

#ifdef USE_WXWIDGETS
//	pwxCallbackMgr::DeInitialize();
	pwxControlMgr::DeInitialize();
#endif // USE_WXWIDGETS

	m_pApp->DeInitializeRender();
	delete m_pApp;

	tmlnTimeLine::DeInitialize();

	ImportExportLayer::CleanUp();
	AudioDSLayer::CleanUp();
	Area18Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	Area18Layer::CleanUp();
	GraphicsLayer::CleanUp();
}

//--------------------------------------------------------------------
// Returns the Graphics system that was created
//--------------------------------------------------------------------
g2dSystem* mainInitGraphics::GetSystem()
{
	return m_pApp->GetSystem();
}

