/*****************************************************************************
**  mnmApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "mnmApp.hpp"

#include "mnmDebugInfo.hpp"
#include "modeModeMgr.hpp"
#include "mnmPaths.hpp"
#include "mnmScreenCaptureUtil.hpp"
#include "mnmSecurityMgr.hpp"
#include "mnmTimeCodeUtil.hpp"
#include "mnmVJoystick.hpp"

#include "cptrRenderUtil.hpp"
#include "lwPackage.hpp"
#include "mnpPackage.hpp"
#include "rpnPanelViewer.hpp"

//	tool library
#include "api3dLightMgr.hpp"
#include "api3dTargetRendererMgr.hpp"
#include "api3dScene.hpp"
#include "cam3dMgr.hpp"
#include "docSingleTypeMgr.hpp"
#include "muiDialogTabbedMgr.hpp"
#include "muiMenuMgr.hpp"
#include "tma3dCursorMgr.hpp"
#include "tma3dRenderView.hpp"
#include "tma3dScreenUtil.hpp"
#include "tma3dViewerMgr.hpp"

//	library
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
#include "envPackage.hpp"
#include "envSTLHelpers.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dExceptionX.hpp"
#include "g2dSystemDX9.hpp"
#include "g2dWindow.hpp"
#include "g3dExceptionX.hpp"
#include "g3dDirectionalLight.hpp"
#include "g3dFragment.hpp"
#include "g3dPrimitiveFragmentUtil.hpp"
#include "g3dPackage.hpp"
#include "g3dPrefs.hpp"
#include "g3dSystemDX9.hpp"
#include "g3dSceneRendererDX9.hpp"
#include "matShaderMgr.hpp"
#include "shdwHDRRendererDX9.hpp"
#include "shdwShadowSceneRendererD3D.hpp"
#include "shdwShadowLayerRendererDX9.hpp"
#include "gfPackage.hpp"
#include "inDeviceMgr.hpp"
#include "inPackage.hpp"
#include "itPackage.hpp"
#include "matMaterial.hpp"
#include "mayReader.hpp"
#include "pythUtil.hpp"

#include <assert.h>


//============================================================================
//	namespace
//============================================================================
namespace
{
	//g3dRenderState* l_pRootRenderState = NULL;
	//g3dDirectionalLight *l_pDirLight = NULL; // temp until lights system

	mnmApp * l_pAppInstance = NULL;
	bool	l_bRenderEnabled = true;
	bool	l_bActive = false;
	int		l_nScreenSpaceIndex = 0;
	float	l_SecurityLastCheck = 0;

	const float lc_SecurityCheckDelay = 1000.0f * 60.0f * 5.0f;
	const char* c_ViewerFontName = "font-lucd00.png";
}
//g3dSceneNode * l_pNode;	//temporary


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void initialize_objects()
{
	//g3dFragment* l_pTetraFragment = g3dPrimitiveFragmentUtil::CreateCube(3.0f);
	//matMaterial *mat = new matMaterial;
	//mat->SetAmbient( 1.0f, 0.5f, 0.2f, 1.0f );
	//l_pTetraFragment->SetMaterial( mat );
	//l_pNode = new g3dSceneNode();
	//l_pNode->SetFragment( l_pTetraFragment );
	//g3dScene::AddToWorldRoot( l_pNode );
	//l_pNode->SetRenderable(true);


	//camCamera::LookAt(maPoint3d(10,10,0), maPoint3d(0,0,0), maVector3d(0,1,0));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void deinitialize_objects()
{
	//
	//g3dScene::RemoveFromWorldRoot( l_pNode );
	//delete l_pNode;
	//l_pNode = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void render_3dWindow()
{
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
//	mnmApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mnmApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Think();
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
//static 
int mnmApp::GetScreenSpaceIndex()
{
	return l_nScreenSpaceIndex;
}


//--------------------------------------------------------------------
//	parse the command line and configure the app accordingly
//--------------------------------------------------------------------
//static
void mnmApp::ParseCommandLine(std::string& i_lpCmdLine)
{
	if ( i_lpCmdLine.length() == 0 )
		return;

	//DBG_LOG1("Command Line: %s", i_lpCmdLine.c_str());

	char seps[]		= " \t\n";
	char seps2[]	= "\"";
	char *token;

	char pLine[1024];
	strcpy( pLine, i_lpCmdLine.c_str() );
	token = strtok( pLine, seps );
	while( token != NULL )
	{
		//DBG_LOG1( "   %s", token );

		if ( strcmp(token,"/file") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line FILE (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			mnpPackage::LaunchFile( scenefile );
		}
		else if ( strcmp(token, "/lastfile") == 0 )
		{
			std::vector<fsLocator>	mru_list;
			docSingleTypeMgr::GetMRUList( mru_list );
			if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
			{
				fsLocator lastfile = mru_list[0];
				mnpPackage::LaunchFile( lastfile );
			}
		}
		else if ( strcmp(token,"/batch") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line BATCH-RENDER (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchBatchRender( scenefile );
		}
		else if ( strcmp(token,"/render") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line RENDER (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchRender( scenefile );
		}
		else if ( strcmp(token,"/quickrender") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line QUICK-RENDER (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchQuickRender( scenefile );
		}
		else if ( strcmp(token,"DEF") == 0 )
		{
			//	grab each sub-parameter
			token = strtok( NULL, seps );
			token = strtok( NULL, seps );
		}
		else if ( strcmp(token,"/script") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line SCRIPT (%s)", token );

			fsLocator script_file;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), script_file );

			if (fsFileUtil::FileExists( script_file ))
				pythUtil::ScriptFile( script_file );
		}

		token = strtok( NULL, seps );
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::mnmApp()
: m_pSystem(NULL), 
m_pSystem3D(NULL)
{
	l_pAppInstance = this;

	mnmApp::SetActive( false );

	std::string dir;
	fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_ExeArt), dir );
	muiMenuMgr::SetIconDirectory( dir.c_str() );

	//modeModeMgr::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::~mnmApp()
{
	//modeModeMgr::DestroyModes();
	//modeModeMgr::DeInitialize();

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
	g2dWindow *app_window = NULL;
	if (!i_HwndFake && !i_Hwnd)
	{
		app_window = m_pSystem->CreateAppWindow(i_Width, i_Height, 100, 100);
	}
	else
	{
		g2dWindow *fake_window = m_pSystem->CreateAppWindow(i_HwndFake);
		app_window = m_pSystem->CreateSubWindow(i_Hwnd);
	}

	// Start with effShaderArray enabled,
	// need to do this before the 3D system is created
	matShaderMgr::SetUseShaderArray(true);

	m_pSystem3D = new g3dSystemDX9();

	api3dLightMgr::Initialize();
	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(0.2f, -0.7f, 0.1f));
	cam3dMgr::Initialize();
	api3dScene::Initialize();

	// Create main render view's window
	//g2dWindow *window = m_pSystem->CreateSubWindow(i_Hwnd);
	cptrRenderUtil::SetAppWindow( app_window );
	cptrRenderUtil::SetCaptureWindow( app_window );
	mnmScreenCaptureUtil::SetCaptureWindow( app_window );
	mnmDebugInfo::SetWindow( app_window );
	mnmTimeCodeUtil::SetMainWindow( app_window );
	mnmTimeCodeUtil::SetWindow( app_window );

	// Create render view class for the main view
	rpnPanelViewer *pPanelViewer = new rpnPanelViewer();
	pPanelViewer->SetAllowShadows(true); // Allow shadows in the main window
	tma3dRenderView *pMainView = new tma3dRenderView(app_window, pPanelViewer);
	pMainView->SetRenderer(tma3dRenderView::e_Default);
	this->m_RenderViews.push_back(pMainView);
	tma3dRenderView::SetActiveRenderView(pMainView);

	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

	// Force ambient color in materials to be full white
	mayReader::SetAlwaysFullAmbient(true);

	//	Create a screen-space layer
	//
	l_nScreenSpaceIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
												g3dLayer::e_Screen, g3dLayer::e_Additive,
												false, false, true );
	//DBG_LOG1( "Screen-space layer #%d", sceneroot_index );

	
	// Now we can create the text label for the main render window
	// after the shaders are enabled and the screen layer has been created.
	fsLocator font_loc;
	font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) );
	font_loc.Push( c_ViewerFontName );
	try
	{
		pPanelViewer->CreateTextLabel( font_loc );
	}
	catch (fsFileDoesntExistX& i_Ex)
	{
		std::string fname;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), fname);
		DBG_ERROR1("File doesn't exist (%s)", fname.c_str());
		assert(false);
	}
}

//--------------------------------------------------------------------
// Create an additional render view using the given HWND
//--------------------------------------------------------------------
void mnmApp::CreateRenderView(void *i_Hwnd)
{
	// RenderView class takes ownership of renderer
	g2dWindow *window = m_pSystem->CreateSubWindow(i_Hwnd);
	rpnPanelViewer *pPanelViewer = new rpnPanelViewer();
	tma3dRenderView *pView = new tma3dRenderView(window, pPanelViewer);
	pView->EnableViewer(false);	// Starting with single pane, this view will be invisible

	// Tried using simple renderer, but that did not do camera space correctly?
	//pView->SetRenderer(tma3dRenderView::e_Simple);

	fsLocator font_loc;
	font_loc.Push( gfPaths::GetPath(mnmPaths::e_ExeArt) );
	font_loc.Push( c_ViewerFontName );
	pPanelViewer->CreateTextLabel( font_loc );

	this->m_RenderViews.push_back(pView);
}

//--------------------------------------------------------------------
// GetRenderView
//--------------------------------------------------------------------
tma3dRenderView* mnmApp::GetRenderView(int i_Index)
{
	DBG_ASSERT2(i_Index>=0 && i_Index<m_RenderViews.size(), "Render View %d out of range %d", i_Index, m_RenderViews.size());
	return this->m_RenderViews[i_Index];
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mnmApp::DeInitializeRender()
{
	envSTLHelpers::DeleteContainer(m_RenderViews);

	api3dScene::DeInitialize();
	cam3dMgr::DeInitialize();

	api3dLightMgr::DeInitialize();

	delete m_pSystem3D;
	delete m_pSystem;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Think()
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
	if( modeModeMgr::IsEmpty() )
	{
		this->Exit();
	}
	else
	{
		modeModeMgr::Think();
	}

	// Render views
	this->Render();

	//	security check
	if (l_SecurityLastCheck + lc_SecurityCheckDelay > appTime::GetTime())
	{
		l_SecurityLastCheck = appTime::GetTime();

		//	only perform if the dongle is present
		if (!mnmSecurityMgr::CheckForDongle())
		{
			mnmSecurityMgr::AnnounceIfNoDongle();
			this->Exit();
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::Render()
{
	if (mnmApp::IsRenderEnabled())
	{
		api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );
		// All render panels are in the tma3dViewerMgr now
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::SetRenderer(int i_Renderer)
{
	//l_pAppInstance->m_pViewer->SetRenderer(l_pAppInstance->m_pRenderer[i_Renderer]);

	if (!l_pAppInstance->m_RenderViews.empty())
	{
		// enum type conversion
		tma3dRenderView::RendererType rType;
		switch(i_Renderer)
		{
		case g3dSceneRendererCreate::e_Default:
			rType = tma3dRenderView::e_Default;
			break;
		case g3dSceneRendererCreate::e_HDR:
			rType = tma3dRenderView::e_HDR;
			break;
		case g3dSceneRendererCreate::e_AmbientOcclusion:
			rType = tma3dRenderView::e_AmbientOcclusion;
			break;
		default:
			rType = tma3dRenderView::e_Default;
			break;
		};
		// Set renderer for main render pane only
		l_pAppInstance->m_RenderViews[0]->SetRenderer( rType );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	//DBG_LOG0("Start Event");

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
	//DBG_LOG0("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	//DBG_LOG0("Resume Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mnmApp::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
	this->Exit();
}

//--------------------------------------------------------------------
//	Exit() should be called by the client when it decides it is
//	ready to end the program.
//--------------------------------------------------------------------
void mnmApp::Exit()
{
	this->SetActive(false);

	appApplication::Exit();
}

