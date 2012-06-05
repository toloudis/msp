/*****************************************************************************
**  tasApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "tasApp.hpp"

#include "tasTUVMgr.hpp"
#include "tasUVAMgr.hpp"

//	tool library
#include "api3dScene.hpp"
#include "cam3dMgr.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dViewerMgr.hpp"

//	library
#include "api3dLightMgr.hpp"
#include "appTime.hpp"
#include "appSimTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "matShaderMgr.hpp"
#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "g2dPackage.hpp"
#include "g2dARGBColor.hpp"
#include "g2dRGBColor.hpp"
#include "g2dSystemDX9.hpp"
#include "g2dWindow.hpp"
#include "g3dSceneGlobal.hpp"
#include "g3dFragment.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dPackage.hpp"
#include "g3dSceneNode.hpp"
#include "g3dSingleLightRendering.hpp"
#include "g3dSystemDX9.hpp"
#include "shdwShadowSceneRendererD3D.hpp"
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

	tasApp*	l_pAppInstance = NULL;
	bool	l_bRenderEnabled = true;
	bool	l_bActive = false;
	bool	l_bTimePaused = false;
	int		l_nScreenSpaceIndex = 0;

	//============================================================================
	//	Rendering stuff
	//============================================================================
	g3dFragment* l_Fragment;
}

//============================================================================
//============================================================================
//void make_uva_model()
//{
//	l_Material.SetDiffuse(g3dARGBColor(1.0f, 1.0f, 1.0f, 1.0f));
//	l_Material.SetAmbient(g3dARGBColor(1.0f, 1.0f, 1.0f, 1.0f));
//
//	int width, height;
//	g2dScreen::GetDimensions(width, height);
//	float aspect = float(width) / float(height);
//
//	l_Fragment = g3dPrimitiveFragmentUtil::CreateTexturedRectangle(0.5f, 0.5f * aspect, 1, 1);
//	g3dFragmentManager::SetMaterial(l_Fragment, &l_Material, 0);
//	g3dHFragment hfrag(l_Fragment);
//	hfrag.GetMatrix().MakeTranslate(-0.4f, 0.10f, 0.5);
//	l_Model = g3dRenderer::AddDynamicModel(hfrag);
//	l_Model->SetRenderSpace(g3dModel::e_ScreenSpace);
//	l_Model->SetRenderable(true);
//}

//
void render_3dWindow()
{
	//cam3dMgr::Think();

	if (l_pAppInstance)
	{
		float begin_time = appTime::GetTime();

		l_pAppInstance->Render();

		float duration = appTime::GetTime() - begin_time;
		if (duration > 1.0f)
		{
			//DBG_LOG1("Long render %f", duration);
		}
	}
}


//
//	tasApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void tasApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool tasApp::IsActive()
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
void tasApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//	let the app not move time, but keep controls active
//--------------------------------------------------------------------
//static 
bool tasApp::IsTimePaused()
{
	return l_bTimePaused;
}
//static 
void tasApp::SetTimePaused( bool i_bPause )
{
	l_bTimePaused = i_bPause;
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void tasApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void tasApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool tasApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tasApp::tasApp()
:	m_pSystem(NULL),
	m_pSystem3D(NULL),
	//m_pRenderer(NULL),
	m_pViewer(NULL)
{
	l_pAppInstance = this;

	tasApp::SetActive( false );

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
	//modeID = mnmModeMgr::AddMode( new mnmModeCaptureUVA( modeIDCapture, modeIDManip ), true, "copy.gif" );
	//DialogAbortDialogUtil::SetCaptureUVAID( modeID );
	//mnmModeMgr::Push( modeIDManip );

	// Mode without gui button
	//modeID = mnmModeMgr::AddMode( new mnmModeRecord() );
	//mnmRecordUtil::SetMode(modeID);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tasApp::~tasApp()
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
void tasApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
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
	
	m_pViewer->SetMatchAspectToWindow(true); // automatically set apect ratio of camera based on window size

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

	//	Create a screen-space layer
	//
	l_nScreenSpaceIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
												g3dLayer::e_Screen, g3dLayer::e_Additive,
												false, false, true );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void tasApp::DeInitializeRender()
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
void tasApp::Think()
{
	if (!IsActive())
		return;

	if (!IsTimePaused())
	{
		appSimTime::IncrementTime();
		g3dSceneGlobal::g_FrameTime = appSimTime::GetTime();
	}

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

//	tasUVAMgr::Think();
//	tasTUVMgr::Think();

	api3dScene::Think( simTime );
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::Render()
{
	if (tasApp::IsRenderEnabled())
	{
		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( appSimTime::GetTime() );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tasApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
int tasApp::GetScreenSpaceIndex()
{
	return l_nScreenSpaceIndex;
}


