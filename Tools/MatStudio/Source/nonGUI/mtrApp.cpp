/*****************************************************************************
**  mtrApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "mtrApp.hpp"

#include "mtrLevel.hpp"
#include "mtrLightMgr.hpp"

//	tool library
#include "api3dTargetRendererMgr.hpp"
#include "api3dScene.hpp"
#include "api3dShape.hpp"
#include "cam3dMgr.hpp"
#include "muiDialogTabbedMgr.hpp"
#include "muiMenuMgr.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dScreenUtil.hpp"
#include "tma3dViewerMgr.hpp"

//	library
#include "api3dLightMgr.hpp"
#include "appApplicationPAC.hpp"
#include "appSimTime.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appPackage.hpp"
#include "appTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "effShaderArray.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dRGBColor.hpp"
#include "g2dExceptionX.hpp"
#include "g2dSystemDX9.hpp"
#include "g2dWindow.hpp"
#include "g3dExceptionX.hpp"
#include "g3dFragment.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dPackage.hpp"
#include "g3dSceneNode.hpp"
#include "g3dSingleLightRendering.hpp"
#include "g3dSystemDX9.hpp"
//#include "g3dSceneRendererDX9.hpp"
#include "shdwShadowSceneRendererD3D.hpp"
#include "shdwShadowLayerRendererDX9.hpp"
#include "g3dViewer.hpp"
#include "gfPackage.hpp"
#include "inDeviceMgr.hpp"
#include "inPackage.hpp"
#include "itPackage.hpp"
#include "matMaterial.hpp"
#include "scrCreator.hpp"

//
//	namespace
//
namespace
{
	// test object
	//api3dObject *l_pCone = NULL;

	//g3dRenderState* l_pRootRenderState = NULL;

	mtrApp * l_pAppInstance = NULL;
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

	mtrLightMgr::Initialize();
}

//
void deinitialize_objects()
{
	//cmpsPackage::DeInitialize();	// compass

	//if (l_pCone) api3dScene::RemoveObject(l_pCone);
	//delete l_pCone;
	//l_pCone = NULL;

	mtrLightMgr::DeInitialize();
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
//	mtrApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mtrApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool mtrApp::IsActive()
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
void mtrApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void mtrApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void mtrApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool mtrApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mtrApp::mtrApp()
: m_pSystem(NULL), 
m_pSystem3D(NULL), 
m_pRenderer(NULL), 
m_pViewer(NULL)
{
	l_pAppInstance = this;

	mtrApp::SetActive( false );

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
mtrApp::~mtrApp()
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
void mtrApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
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

	// Start with effShaderArray enabled
	effShaderArray::SetUseShaderArray(true);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mtrApp::DeInitializeRender()
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
void mtrApp::Think()
{
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

	mtrLevel::Think();
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::Render()
{
	if (mtrApp::IsRenderEnabled())
	{
		float sim_time =  appSimTime::GetTime();
		if (g3dSingleLightRendering::GetDoSingleLightRendering())
			api3dTargetRendererMgr::RenderTargets( sim_time );
		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( sim_time );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( sim_time );
	}

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mtrApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}



