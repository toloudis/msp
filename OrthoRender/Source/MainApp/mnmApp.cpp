/*****************************************************************************
**  mnmApp.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "stdafx.h"
#include "MainApp/mnmApp.hpp"
#include "MainApp/LightWaitMessage.hpp"

#include "Support/mnm/mnmDebugInfo.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmScreenCaptureUtil.hpp"
#include "Support/mnm/mnmSecurityMgr.hpp"
#include "Support/mnm/mnmTimeCodeUtil.hpp"
#include "Support/mnm/mnmVJoystick.hpp"

#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/orthoPackage.hpp"
#include "Features/ObjectManip/mnpPackage.hpp"
#include "Features/RenderPanels/rpnPanelViewer.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Requests/orthoRemoteCommandMgr.hpp"

//	tool library
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dTargetRendererMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiDialogTabbedMgr.hpp"
//#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"

//	library
#include "Core/App/private/appApplicationPAC.hpp"
#include "Core/App/appSimTime.hpp"
#include "Core/App/appCharEvent.hpp"
//#include "Core/App/appCharEventHandler.hpp"	//included in OrthoXMLParser
#include "Core/App/appMouseEvent.hpp"
#include "Core/App/appMouseEventHandler.hpp"
#include "Core/App/appFlowEvent.hpp"
#include "Core/App/appPackage.hpp"
#include "Core/App/appTime.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/Dbg/dbgPackage.hpp"
#include "Core/Env/envPackage.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsPackage.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Fs/fsFileX.hpp"
#include "Graphics/G2d/g2dFontUtil.hpp"
#include "Graphics/G2d/g2dPackage.hpp"
#include "Graphics/G2d/g2dExceptionX.hpp"
#include "GraphicsDX9/G2d/g2dSystemDX9.hpp"
#include "Graphics/G2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dExceptionX.hpp"
#include "Graphics/G3d/g3dDirectionalLight.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/G3d/g3dPackage.hpp"
#include "Graphics/G3d/g3dPrefs.hpp"
#include "GraphicsDX9/G3d/g3dSystemDX9.hpp"
#include "GraphicsDX9/G3d/g3dSceneRendererDX9.hpp"
#include "Graphics/Mat/matShaderMgr.hpp"
#include "GraphicsDX9/shdw/shdwHDRRendererDX9.hpp"
#include "GraphicsDX9/shdw/shdwShadowSceneRendererD3D.hpp"
#include "GraphicsDX9/shdw/shdwShadowLayerRendererDX9.hpp"
#include "Core/Gf/gfPackage.hpp"
#include "InputDI/In/inDeviceMgr.hpp"
#include "InputDI/In/inPackage.hpp"
#include "Core/It/itPackage.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mdl/mdlReader.hpp"
#include "Support/pyth/pythUtil.hpp"

#include <assert.h>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------


//============================================================================
//	namespace
//============================================================================

//----------------------------------------------------------------------------
// 4-8-08 PSM
//----------------------------------------------------------------------------
namespace LightWaitMessaging 
{
	Thread *l_ConsumerThread;
	Thread *l_ProducerThread;

	LightWaitMessageConsumer *l_Consumer;
	LightWaitMessageProducer *l_Producer;

	CRITICAL_SECTION remoteMessagingCriticalSection; 
} ;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
	//g3dRenderState* l_pRootRenderState = NULL;
	//g3dDirectionalLight *l_pDirLight = NULL; // temp until lights system

	mnmApp * l_pAppInstance = NULL;
	bool	l_bRenderEnabled = true;
	bool	l_bActive = false;
	int		l_nScreenSpaceIndex = 0;
	int		l_nIconsLayerIndex = 0;
	float	l_SecurityLastCheck = 0;
	UINT	l_MaxVideoMem = 0;
	UINT	l_CurrentVideoMem = 0;

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
//	Run() should be called to start the application.  When control
//	returns from Run(), the application is finished.
//--------------------------------------------------------------------
void mnmApp::Run()
{
	m_pImp->PreMessageLoop();

	//	Application message loop
	//
	message_loop();

	m_pImp->PostMessageLoop();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mnmApp::ThinkApp()
{
	DBG_ASSERT0( l_pAppInstance,"The app has not been created");

	// check for remote messages from the message queueing system
	//
	if (remote_message_processing()) 
	{
		DBG_LOG0("message received and posted.\n") ;
	}

	l_pAppInstance->Think();

	Sleep(20);
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
//--------------------------------------------------------------------
//static 
int mnmApp::GetIconsLayerIndex()
{
	return l_nIconsLayerIndex;
}

//--------------------------------------------------------------------
//	parse the command line and configure the app accordingly (right after startup so only set states, no calls)
//--------------------------------------------------------------------
//static
void mnmApp::PreParseCommandLine(std::string& i_lpCmdLine)
{
	if ( i_lpCmdLine.length() == 0 ) return;

	//DBG_LOG1("Command Line: %s", i_lpCmdLine.c_str());

	char seps[]		= " \t\n";
	char seps2[]	= "\"";
	char *token;

	char pLine[1024];
	strcpy( pLine, i_lpCmdLine.c_str() );
	token = strtok( pLine, seps );
	while( token != NULL )
	{
		if( strcmp(token,"/local") == 0 )
		{
			DBG_LOG0( "   Command-line - Running in local mode." );
			g_bRemoteRenderingCommand = false;
		}
		else if( strcmp(token,"/compressResponse") == 0 )
		{
			DBG_LOG0( "   Command-line - Compressing responses." );
			g_bRemoteCompressResponses = true;
		}
		else if( strcmp(token,"/commandfile" ) == 0 )	//XML file with commands to process
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line - COMMANDFILE (%s)", token );

			orthoRemoteCommandMgr::l_pCommandFile = new std::ifstream( token );
			if( !orthoRemoteCommandMgr::l_pCommandFile->is_open() )
			{
				DBG_ERROR1( "   Command-line - COMMANDFILE (%s) not found!", token );
				delete orthoRemoteCommandMgr::l_pCommandFile;
				orthoRemoteCommandMgr::l_pCommandFile = NULL;
			}

		}

		token = strtok( NULL, seps );
	}
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

			cptrPackage::LaunchRender( scenefile, 0, 0);
		}
		else if ( strcmp(token,"/quickrender") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line QUICK-RENDER (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			cptrPackage::LaunchQuickRender( scenefile, 0, 0 );
		}
		else if ( strcmp(token,"/convert") == 0 )
		{
			token = strtok( NULL, seps2 );
			DBG_LOG1( "   Command-line CONVERT (%s)", token );

			fsLocator scenefile;
			fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );

			mnpPackage::ConvertFile( scenefile );
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

//static 
int mnmApp::QueryVideoMemory()
{
	if( l_pAppInstance && l_pAppInstance->GetSystem3D() )
	{
		l_CurrentVideoMem = l_pAppInstance->GetSystem3D()->GetVideoMemory();
		return l_CurrentVideoMem;
	}
	return -1;
}

int mnmApp::QueryMaxVideoMemory()
{
	return l_MaxVideoMem;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
mnmApp::mnmApp()
:	m_pSystem(NULL), 
	m_pSystem3D(NULL)
{
	l_pAppInstance = this;

	mnmApp::SetActive( false );

//	std::string dir;
//	fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath(mnmPaths::e_ExeArt), dir );
//	guiMenuMgr::SetIconDirectory( dir.c_str() );

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
	setup_remote_message_processing();

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
	l_MaxVideoMem = m_pSystem3D->GetVideoMemory();

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

	//	disable time and thread stamps
	std::vector<dbgStream*> _osList = dbgMsg::GetStreamList(); 
	for(int i = 0; i < _osList.size(); ++i)
	{
		dbgMsg::enableThreadStamp(_osList[i]->GetName(), false);
		dbgMsg::enableFileNameStamp(_osList[i]->GetName(), false);
	}
	DBG_LOG("Video Hardware Name: " << m_pSystem3D->GetVideoAdapterName());
	//	enable time and thread stamps
	for(int i = 0; i < _osList.size(); ++i)
	{
		dbgMsg::enableThreadStamp(_osList[i]->GetName(), true);
		dbgMsg::enableFileNameStamp(_osList[i]->GetName(), true);
	}

	// Force ambient color in materials to be full white
	mdlReader::SetAlwaysFullAmbient(true);

	//	Create a screen-space layer
	//
	l_nScreenSpaceIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
												g3dLayer::e_Screen, g3dLayer::e_Additive,
												false, false, true, false );
	//DBG_LOG1( "Screen-space layer #%d", sceneroot_index );

	//	Create a world space layer for icons only:
	l_nIconsLayerIndex = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
		g3dLayer::e_World, g3dLayer::e_Additive,
		false, false, false, false);
	
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
		
		// removed so that the renderer doesn't crash on start up
		// PSM 8-15-08
		//std::string msg = "Cannot find file\n" + fname;
		//guiMessageBox::Show(msg.c_str(), "Error", guiMessageBox::e_OKOnly);
		//assert(false);
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
#ifndef _MANAGED
		// While we are working on the wxWidgets version of MachStudio,
		// add in the hot keys you need here by hand. Use the python
		// command form of the menu item ("Next Camera" -> "nextCamera").
		//if ( pKeyboard->IsReleased( inKeys::e_F8 ))
		//{
		//	pythUtil::ExecuteCommand("mach.execCommand('nextCamera')");
		//}
#endif
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
		D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"mnmApp::Render" );
		api3dTargetRendererMgr::RenderTargets( appSimTime::GetTime() );

		// All render panels are in the tma3dViewerMgr now
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );
		D3DPERF_EndEvent();
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
		case g3dSceneRendererCreate::e_Depth:
			rType = tma3dRenderView::e_Depth;
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

	// ----------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------
	if (   (LightWaitMessaging::l_Consumer != NULL)
		&& (LightWaitMessaging::l_Producer != NULL)
		&& (LightWaitMessaging::l_ProducerThread != NULL))
	{
		try {
			LightWaitMessaging::l_Consumer->stop();

			LightWaitMessaging::l_ConsumerThread->join();
			delete LightWaitMessaging::l_ConsumerThread;
			LightWaitMessaging::l_ConsumerThread = NULL;

			delete LightWaitMessaging::l_Consumer;
			LightWaitMessaging::l_Consumer = NULL;

			LightWaitMessaging::l_Producer->stop();

			LightWaitMessaging::l_ProducerThread->join();
			delete LightWaitMessaging::l_ProducerThread;
			LightWaitMessaging::l_ProducerThread = NULL;

			delete LightWaitMessaging::l_Producer;
			LightWaitMessaging::l_Producer = NULL;
		}
		catch ( ... ) 
		{
		}
	}

	// ----------------------------------------------------------------------------------
	// ----------------------------------------------------------------------------------
}



//------------------------------------------------------------------------
//------------------------------------------------------------------------
int mnmApp::setup_remote_message_processing(void)
{
	if( g_bRemoteRenderingCommand )
	{
		static bool gRemoteThreadStarted = false;
		if (gRemoteThreadStarted) 
		{
			return 0;
		}

   		PrefsData& data = PrefsMgr::Data();
		

		gRemoteThreadStarted = true;

		// Set the URI to point to the IPAddress of your broker.
		// add any optional params to the url to enable things like
		// tightMarshalling or tcp logging etc.  See the CMS website for
		// a full list of configuration options.
		//
		//  http://activemq.apache.org/cms/
		//
		// Wire Format Options:
		// --------------------=
		// Use either stomp or openwire, the default ports are different for each
		//
		// Examples:
		//    tcp://127.0.0.1:61616                      default to openwire
		//    tcp://127.0.0.1:61616?wireFormat=openwire  same as above
		//    tcp://127.0.0.1:61613?wireFormat=stomp     use stomp instead
		//
		std::string brokerURI;

		brokerURI.append(data.m_Lightwait_MessagingProtocol.GetValue());
		brokerURI.append("://");
		brokerURI.append(data.m_Lightwait_MessagingServer.GetValue());
		brokerURI.append(":");
		brokerURI.append(data.m_Lightwait_MessagingPort.GetValue());
		brokerURI.append("?wireFormat=stomp&transport.useAsyncSend=true");


			// "tcp://messaging.avatarassembly.com:61613"
			// "?wireFormat=stomp"
			// "&transport.useAsyncSend=true";

		//------------------------------------------------------------
		// set to true to use topics instead of queues
		// Note in the code above that this causes createTopic or
		// createQueue to be used in both consumer an producer.
		//------------------------------------------------------------
		bool useTopics = true;
		int numMessages = 2000;

		InitializeCriticalSection(&LightWaitMessaging::remoteMessagingCriticalSection);

		// Start the consumer thread.
		LightWaitMessaging::l_Consumer = new LightWaitMessageConsumer( brokerURI, numMessages, useTopics );
		LightWaitMessaging::l_ConsumerThread = new Thread( LightWaitMessaging::l_Consumer );
		LightWaitMessaging::l_ConsumerThread->start();

		// Wait for the consumer to indicate that its ready to go.
		LightWaitMessaging::l_Consumer->waitUntilReady();

		// Start the producer thread.
		LightWaitMessaging::l_Producer = new LightWaitMessageProducer( brokerURI, numMessages, useTopics );
		LightWaitMessaging::l_ProducerThread = new Thread( LightWaitMessaging::l_Producer );
		LightWaitMessaging::l_ProducerThread->start();
	}

	return 0;
}


// 4-1-08 PSM
//------------------------------------------------------------------------
//------------------------------------------------------------------------
bool mnmApp::remote_message_processing(void) 
{
#if 0
	queue<LightWaitMessage *> *messageQueue = LightWaitMessaging::l_Consumer->getQueue();

	if (messageQueue->empty()) 
	{
		return false;
	}

	DBG_LOG0("remote_message_processing(): message received.\n");

	LightWaitMessage *message = messageQueue->front();

	messageQueue->pop();

	// send the message on to the windows message queue for now
	// 4-1-08 PSM
//	(void) ::PostMessage(this->GetHWND(),WM_CHAR,message->keyPressed, 0);

	delete message;
	return true;
#endif
	return false;
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void mnmApp::message_loop()
{
	// Run message loop
	//
	const bool l_MustUseANSI = false;
	if ( l_MustUseANSI )
	{			
		while ( !IsQuitting() )
		{
			//	Windows Messaging
			//
			MSG msg;
			BOOL got_msg = 0;

			got_msg = ::PeekMessageA( &msg, NULL, 0, 0, PM_REMOVE );

			//	If windows message handle it first
			//
			if( got_msg )
			{
				::TranslateMessage(&msg);
				::DispatchMessageA(&msg);
			}
			else
			{
				// no messages, so do some idle processing
				//
				Think();
				Sleep(2);
			}
		}
	}
	else
	{
		while ( !IsQuitting() )
		{
			MSG msg;
			BOOL got_msg = 0;

			got_msg = ::PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE );

			if( got_msg )
			{
				::TranslateMessage(&msg);
				::DispatchMessageW(&msg);
			}
			else
			{
				// no messages, so do some idle processing
				//
				Think();
				Sleep(2);
			}
		}
	}
}
