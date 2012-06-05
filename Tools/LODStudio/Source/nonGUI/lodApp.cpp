/*****************************************************************************
**  lodApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "lodApp.hpp"

#include "lodLevel.hpp"
#include "lodModeMgr.hpp"
#include "lodModeLODTest.hpp"

//	tool library
#include "api3dScene.hpp"
#include "cam3dMgr.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dViewerMgr.hpp"

////	library
#include "api3dLightMgr.hpp"
#include "appSimTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "g2dPackage.hpp"
#include "g2dRGBColor.hpp"
#include "g2dSystemDX9.hpp"
#include "g2dWindow.hpp"
#include "g3dPackage.hpp"
#include "g3dSystemDX9.hpp"
#include "shdwShadowLayerRendererDX9.hpp"
#include "g3dViewer.hpp"
#include "gfPackage.hpp"
#include "inDeviceMgr.hpp"
#include "inPackage.hpp"
#include "itPackage.hpp"


//
//	namespace
//
namespace
{
	// test object
	//api3dObject *l_pCone = NULL;

	//g3dRenderState* l_pRootRenderState = NULL;

	lodApp * l_pAppInstance = NULL;
	bool l_bRenderEnabled = true;
	bool l_bActive = false;
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
//	lodApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void lodApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool lodApp::IsActive()
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
void lodApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void lodApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void lodApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool lodApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lodApp::lodApp()
:	m_pSystem(NULL),
	m_pSystem3D(NULL),
	m_pRenderer(NULL),
	m_pViewer(NULL)
{
	l_pAppInstance = this;

	lodApp::SetActive( false );

	//lodPaths::SetupPaths();

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(lodPaths::e_ExeArt), dir );
	//muiMenuMgr::SetIconDirectory( dir.c_str() );

	lodModeMgr::Initialize();

	lodModeID modeID;
	modeID = lodModeMgr::AddMode( new lodModeLODTest(), true, "globe.gif" );
	lodModeMgr::Push( modeID );

	// Mode without gui button
	//modeID = lodModeMgr::AddMode( new lodModeRecord() );
	//lodRecordUtil::SetMode(modeID);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lodApp::~lodApp()
{
	lodModeMgr::DestroyModes();
	lodModeMgr::DeInitialize();

	if (l_pAppInstance == this)
	{
		l_pAppInstance = NULL;
	}
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void lodApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
{
	m_pSystem = new g2dSystemDX9();

	// This window is owned by the system
	//g2dWindow *window = m_pSystem->CreateAppWindow(i_Width, i_Height, i_Top, i_Left);
	g2dWindow *window = m_pSystem->CreateAppWindow(i_HwndFake);
	m_pWindow = m_pSystem->CreateSubWindow(i_Hwnd);
//	lodScreenCaptureUtil::SetCaptureWindow( m_pWindow );
//	lodDebugInfo::SetWindow( m_pWindow );

	m_pSystem3D = new g3dSystemDX9();

	// set up renderer and viewer for this window
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
	//effShaderArray::SetUseShaderArray(true);

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void lodApp::DeInitializeRender()
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
void lodApp::Think()
{
	appSimTime::IncrementTime();

	float simTime = appSimTime::GetTime();

	//	poll devices
	//
	inDeviceMgr::Think();

	// think the cursor manager
	tma3dCursorMgr::Think();

	// think the camera manager
	//lodCameraMgr::Think();

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
	if( lodModeMgr::IsEmpty() )
	{
		this->Exit();
	}
	else
	{
		lodModeMgr::Think();
	}

	lodLevel::Think();

	api3dScene::UpdateLOD( cam3dMgr::GetCamera().GetPosition() );
	api3dScene::Think( simTime );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::Render()
{
	if (lodApp::IsRenderEnabled())
	{
		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( appSimTime::GetTime() );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lodApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}



