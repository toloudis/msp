/*****************************************************************************
**  ptclApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "ptclApp.hpp"

#include "ptclLevel.hpp"

//	tool library
#include "Tool/api3d/api3dScene.hpp"
//#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
//#include "ToolUIManaged/mui/muiDialogTabbedMgr.hpp"
//#include "ToolUIManaged/mui/muiMenuMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
//#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"
//
////	library
#include "Tool/api3d/api3dLightMgr.hpp"
//#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/app/appSimTime.hpp"
//#include "Core/app/appCharEvent.hpp"
//#include "Core/app/appCharEventHandler.hpp"
//#include "Core/app/appMouseEvent.hpp"
//#include "Core/app/appMouseEventHandler.hpp"
//#include "Core/app/appFlowEvent.hpp"
//#include "Core/app/appPackage.hpp"
//#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsPackage.hpp"
//#include "Core/fs/fsFileUtil.hpp"
//#include "Core/fs/fsFileX.hpp"
//#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
//#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX9/g2d/g2dSystemDX9.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
//#include "Graphics/g3d/g3dExceptionX.hpp"
//#include "Graphics/g3d/g3dFragment.hpp"
//#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
//#include "Graphics/g3d/g3dSceneNode.hpp"
//#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX9/g3d/g3dSystemDX9.hpp"
////#include "GraphicsDX9/g3d/g3dSceneRendererDX9.hpp"
//#include "GraphicsDX9/shdw/shdwShadowSceneRendererD3D.hpp"
#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Core/gf/gfPackage.hpp"
#include "InputDI/in/inDeviceMgr.hpp"
#include "InputDI/in/inPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
//#include "Graphics/mat/matMaterial.hpp"
//#include "Graphics/scr/scrCreator.hpp"
//
//#include "Graphics/an/anKeyAnimation.hpp"
//#include "Tool/api3d/api3dParticleGenerator.hpp"
//#include "Core/gf/gfPaths.hpp"
//#include "Core/ma/maConstants.hpp"
//#include "Graphics/mat/matTextureMgr.hpp"
//#include "Graphics/mat/matExceptionX.hpp"
//#include "Graphics/prt/prtConeParticleGenerator.hpp"
//#include "Graphics/prt/prtGeneratorUtil.hpp"


//
//	namespace
//
namespace
{
	// test object
	//api3dObject *l_pCone = NULL;

	//g3dRenderState* l_pRootRenderState = NULL;

	ptclApp * l_pAppInstance = NULL;
	bool l_bRenderEnabled = true;
	bool l_bActive = false;
	bool l_bTimePaused = false;
}

//
void render_3dWindow()
{
	cam3dMgr::Think();

	if (l_pAppInstance)
	{
		//float begin_time = appTime::GetTime();

		l_pAppInstance->Render();

		//float duration = appTime::GetTime() - begin_time;
		//if (duration > 1.0f)
		//{
		//	DBG_LOG1("Long render %f", duration);
		//}
	}
}


//
//	ptclApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void ptclApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool ptclApp::IsActive()
{
	return l_bActive;

	//if (l_pAppInstance)
	//{
	//	return true;
	//}

	//return false;
}


//--------------------------------------------------------------------
//	set this app as "active"
//--------------------------------------------------------------------
//static
void ptclApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//	let the app not move time, but keep controls active
//--------------------------------------------------------------------
//static 
bool ptclApp::IsTimePaused()
{
	return l_bTimePaused;
}
//static 
void ptclApp::SetTimePaused( bool i_bPause )
{
	l_bTimePaused = i_bPause;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void ptclApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void ptclApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool ptclApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptclApp::ptclApp()
:	m_pSystem(NULL),
	m_pSystem3D(NULL),
	m_pRenderer(NULL),
	m_pViewer(NULL)
{
	l_pAppInstance = this;

	ptclApp::SetActive( false );

	//mnmPaths::SetupPaths();

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_ExeArt), dir );
	//muiMenuMgr::SetIconDirectory( dir.c_str() );

	//mnmModeMgr::Initialize();

	//mnmModeID modeID;
	//mnmModeID modeIDCapture;
	//mnmModeID modeIDManip;
	//modeID = mnmModeMgr::AddMode( new mnmModePlayback(), true, "play.gif" );
	//m_pModeCapture = new mnmModeCapture();
	//modeIDCapture = mnmModeMgr::AddMode( m_pModeCapture, true, "record.gif" );
	//modeIDManip = mnmModeMgr::AddMode( new mnmModeObjectManip(), true, "globe.gif" );
	//modeID = mnmModeMgr::AddMode( new mnmModeCaptureBatch( modeIDCapture, modeIDManip ), true, "copy.gif" );
	//DialogAbortDialogUtil::SetCaptureBatchID( modeID );
	//mnmModeMgr::Push( modeIDManip );

	// Mode without gui button
	//modeID = mnmModeMgr::AddMode( new mnmModeRecord() );
	//mnmRecordUtil::SetMode(modeID);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ptclApp::~ptclApp()
{
	//mnmModeMgr::DestroyModes();
	//mnmModeMgr::DeInitialize();

	if (l_pAppInstance == this)
	{
		l_pAppInstance = NULL;
	}
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ptclApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
{
	m_pSystem = new g2dSystemDX9();

	// This window is owned by the system
	//g2dWindow *window = m_pSystem->CreateAppWindow(i_Width, i_Height, i_Top, i_Left);
	g2dWindow *window = m_pSystem->CreateAppWindow(i_HwndFake);
	m_pWindow = m_pSystem->CreateSubWindow(i_Hwnd);
//	mnmScreenCaptureUtil::SetCaptureWindow( m_pWindow );
//	mnmDebugInfo::SetWindow( m_pWindow );

	m_pSystem3D = new g3dSystemDX9();

	// set up renderer and viewer for this window
	//m_pRenderer = new g3dSceneRendererDX9();
	//m_pRenderer = new shdwShadowSceneRendererD3D();
	m_pRenderer = new shdwShadowLayerRendererDX9();
	//g3dSingleLightRendering::SetDoSingleLightRendering(true); // hook this up to a menu
	m_pViewer = new g3dViewer(m_pWindow, m_pRenderer);
	//m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );

	api3dLightMgr::Initialize();
	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(0.2f, -0.7f, 0.1f));

	cam3dMgr::Initialize();
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());

	api3dScene::Initialize();
	m_pViewer->SetScene(api3dScene::GetScene());

	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

	// Start with effShaderArray enabled
	matShaderMgr::SetUseShaderArray(true);

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void ptclApp::DeInitializeRender()
{
	api3dScene::DeInitialize();
	cam3dMgr::DeInitialize();

	api3dLightMgr::DeInitialize();

	delete m_pViewer;
	delete m_pRenderer;
	delete m_pSystem3D;
	delete m_pSystem;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::Think()
{
	if (!IsActive())
		return;

	if (!IsTimePaused())
		appSimTime::IncrementTime();

	float simTime = appSimTime::GetTime();

	//	poll devices
	//
	inDeviceMgr::Think();

	// think the cursor manager
	tma3dCursorMgr::Think();

	// think the camera manager
	//mnmCameraMgr::Think();

	//
	//	input
	//
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	if ( pKeyboard )
	{
		//if ( pKeyboard->IsReleased( inKeys::e_F8 ))
		//{
		//	//cmraCueDialogUtil::Show();
		//}
	}

	//	Think the running mode
	//
//	if( mnmModeMgr::IsEmpty() )
//	{
//		this->Exit();
//	}
//	else
//	{
//		mnmModeMgr::Think();
//	}

	ptclLevel::Think();

	api3dScene::Think( simTime );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::Render()
{
	if (ptclApp::IsRenderEnabled())
	{
		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( appSimTime::GetTime() );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ptclApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}



