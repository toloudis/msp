/*****************************************************************************
**  mnmApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "mnmApp.hpp"

//#include "mnmDebugInfo.hpp"
//#include "mnmModeMgr.hpp"
//#include "mnmModeObjectManip.hpp"
//#include "mnmPaths.hpp"
//#include "mnmRecordUtil.hpp"
//#include "mnmScreenCaptureUtil.hpp"
//#include "mnmVJoystick.hpp"

//	tool library
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "ToolUIManaged/mui/muiDialogTabbedMgr.hpp"
#include "ToolUIManaged/mui/muiMenuMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"

//	library
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appCharEventHandler.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appMouseEventHandler.hpp"
#include "Core/app/appFlowEvent.hpp"
#include "Core/app/appPackage.hpp"
#include "Core/app/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g2d/g2dRGBColor.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "GraphicsDX9/g2d/g2dSystemDX9.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX9/g3d/g3dSystemDX9.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Core/gf/gfPackage.hpp"
#include "InputDI/in/inDeviceMgr.hpp"
#include "InputDI/in/inPackage.hpp"
#include "Core/it/itPackage.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/scr/scrCreator.hpp"


//
//	namespace
//
namespace
{
	// test object
	//api3dObject *l_pCone = NULL;

	//g3dRenderState* l_pRootRenderState = NULL;
	g3dDirectionalLight *l_pDirLight = NULL; // temp until lights system

	mnmApp * l_pAppInstance = NULL;
	bool l_bRenderEnabled = true;
	bool l_bActive = false;
}


//g3dSceneNode * l_pNode;	//temporary

//
void initialize_objects()
{
	//cmpsPackage::Initialize();	// compass

	//int sceneroot_index;
	//sceneroot_index = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
	//										g3dLayer::e_Screen, g3dLayer::e_Additive,
	//										false, false, true );
	//DBG_LOG1( "Screen-space layer #%d", sceneroot_index );

	//l_pCone = api3dShape::CreateCone( maFloatRGBA( 1.0f, 0.5f, 0.2f, 1.0f ), 1.5f, 2.0f, 24);
	//api3dScene::AddObject(l_pCone);
	//cam3dMgr::FocusCamera(maPoint3d(0,0,0), 4.0f);

	l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	l_pDirLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	l_pDirLight->SetIntensity(maFloatRGBA(0.65f, 0.65f, 0.6f, 1.0f));
	l_pDirLight->Enable();
	l_pDirLight->SetCastsShadow(true);
}

//
void deinitialize_objects()
{
	//cmpsPackage::DeInitialize();	// compass

	//if (l_pCone) api3dScene::RemoveObject(l_pCone);
	//delete l_pCone;
	//l_pCone = NULL;

	api3dLightMgr::DestroyLight(l_pDirLight);
	l_pDirLight = NULL;
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
//	mnmApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mnmApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Update();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool mnmApp::IsActive()
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
void mnmApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void mnmApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void mnmApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool mnmApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::mnmApp()
: m_pSystem(NULL), 
m_pSystem3D(NULL), 
m_pRenderer(NULL), 
m_pViewer(NULL)
{
	l_pAppInstance = this;

	mnmApp::SetActive( false );

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
mnmApp::~mnmApp()
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
void mnmApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
{
	m_pSystem = new g2dSystemDX9();

	// This window is owned by the system
	if (!i_HwndFake && !i_Hwnd)
	{
		m_pWindow = m_pSystem->CreateAppWindow(i_Width, i_Height, 100, 100);
	}
	else
	{
		g2dWindow *window = m_pSystem->CreateAppWindow(i_HwndFake);
		m_pWindow = m_pSystem->CreateSubWindow(i_Hwnd);
	}

	// Need to enable this before creating the g3dSystemDX9
	matShaderMgr::SetUseShaderArray(true);

	m_pSystem3D = new g3dSystemDX9();

	// set up renderer and viewer for this window
	m_pRenderer = g3dSceneRendererCreate::CreateDefaultRenderer();
	//g3dSingleLightRendering::SetDoSingleLightRendering(true); // hook this up to a menu
	m_pViewer = new g3dViewer(m_pWindow, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );
	//m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );

	api3dLightMgr::Initialize();
	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(0.2f, -0.7f, 0.1f));

	cam3dMgr::Initialize();
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());

	api3dScene::Initialize();
	m_pViewer->SetScene(api3dScene::GetScene());

	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mnmApp::DeInitializeRender()
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
// Called from appApplication::Run(), 
// do all of update and render at once.
//--------------------------------------------------------------------
void mnmApp::Think()
{
	Update();
	cam3dMgr::Think();
	Render();
}
//--------------------------------------------------------------------
// Called from MainForm through ThinkApp()
//--------------------------------------------------------------------
void mnmApp::Update()
{
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

	// Always pass the same time in order to freeze animation
	api3dScene::Think(0.0f);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Render()
{
	if (mnmApp::IsRenderEnabled())
	{
		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( appSimTime::GetTime() );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}

//--------------------------------------------------------------------
//	Override this function to get appQuitReqeustedEvents.
//--------------------------------------------------------------------
void mnmApp::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
	// This is called when the user tries to close the non-managed window
	// from the button in the upper right corner
	this->Exit();
}



