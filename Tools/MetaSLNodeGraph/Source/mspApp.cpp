/*****************************************************************************
**  mspApp.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "mspApp.hpp"
#include "mspLighting.hpp"
#include "mspModel.hpp"
#include "mspViewSettings.hpp"

//	library
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appCharEventHandler.hpp"
#include "Core/app/appFlowEvent.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appMouseEventHandler.hpp"
#include "Core/app/appPackage.hpp"
#include "Core/app/appSimTime.hpp"
#include "Core/app/appTime.hpp"
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsPackage.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Core/Gf/gfPaths.hpp"
#include "Core/it/itPackage.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "Graphics/Eff/effPhongData.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dPackage.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/g3d/g3dDirectionalLight.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/G3d/g3dLightMgr.hpp"
#include "Graphics/g3d/g3dPackage.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/g3d/g3dPrimitiveFragmentUtil.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dSystem.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/scr/scrCreator.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"

// bad! needed for override material hack
#include "GraphicsDX11/G3d/g3dDX11Util.hpp"

#include "Input/in/inDeviceMgr.hpp"
#include "Input/in/inPackage.hpp"
#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/api3d/api3dShape.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/tma3d/tma3dCursorMgr.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "Tool/tma3d/tma3dViewerMgr.hpp"
#include "ToolUIManaged/mui/muiDialogTabbedMgr.hpp"
#include "ToolUIManaged/mui/muiMenuMgr.hpp"




//
//	namespace
//
namespace
{
	const bool c_bDoSimpleOverride = false;

	mspApp * l_pAppInstance = NULL;
	bool l_bRenderEnabled = true;
	bool l_bActive = false;
}


//g3dSceneNode * l_pNode;	//temporary


//
void initialize_objects()
{

	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(1.2f, -1.0f, -1.0f));
	//l_pDirLight->SetIntensity(maFloatRGBA(0.85f, 0.85f, 0.85f, 1.0f));
	//l_pDirLight->Enable();
	//l_pDirLight->SetCastsShadow(true);
}

//
void deinitialize_objects()
{
	//cmpsPackage::DeInitialize();	// compass


	//api3dLightMgr::DestroyLight(l_pDirLight);
	//l_pDirLight = NULL;
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
//	mspApp functions
//

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
void mspApp::ThinkApp()
{
	DBG_ASSERT( l_pAppInstance,"The app has not been created");

	l_pAppInstance->Update();
}

//--------------------------------------------------------------------
//	Thinks the app instance
//--------------------------------------------------------------------
//static
bool mspApp::IsActive()
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
void mspApp::SetActive( bool i_bActive )
{
	l_bActive = i_bActive;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void mspApp::ResizeRender(int i_Width, int i_Height)
{
	if (l_pAppInstance)
		l_pAppInstance->ResizeRenderWindow(i_Width, i_Height);
}

//--------------------------------------------------------------------
// Enable/Disable rendering in main window
//--------------------------------------------------------------------
//static
void mspApp::EnableRender(bool i_bVal)
{
	l_bRenderEnabled = i_bVal;
}
//static
bool mspApp::IsRenderEnabled()
{
	return l_bRenderEnabled;
}

//--------------------------------------------------------------------
//	parse the command line and configure the app accordingly
//--------------------------------------------------------------------
//static
void mspApp::ParseCommandLine(std::string& i_lpCmdLine)
{
	if ( i_lpCmdLine.length() == 0 || i_lpCmdLine.length() >= 1024)
		return;

	DBG_LOG("Command Line: " << i_lpCmdLine );

	char seps[]		= " \t\n";
	char seps2[]	= "\"";
	char *token;

	char pLine[1024];
	strcpy( pLine, i_lpCmdLine.c_str() );
	if (i_lpCmdLine[0] == '"')
		token = strtok( pLine, seps2 );
	else
		token = strtok( pLine, seps );
	if ( token != NULL )
	{
		//DBG_LOG1( "   %s", token );

		// No real command line arguments, just try to load the parameter like a file
		fsLocator scenefile;
		fsFileUtil::ANSIFilenameToLocator(std::string(token), scenefile );
		guiCustomDocHandler::Open(scenefile);
	}
}



//--------------------------------------------------------------------
//--------------------------------------------------------------------
mspApp::mspApp(const fsLocator& i_DataDirectory)
:	m_pSystem(NULL), 
	m_pSystem3D(NULL), 
	m_pRenderer(NULL), 
	m_pViewer(NULL),
	m_pSimpleMaterial(NULL),
	m_pLighting(NULL)
{
	l_pAppInstance = this;

	mspApp::SetActive( false );

	//mnmPaths::SetupPaths();

	std::vector<fsLocator> iconPathList;
	fsLocator icons_dir = i_DataDirectory;
	icons_dir.Push("sgpuIcons");
	iconPathList.push_back(icons_dir);
	guiMenuMgr::SetIconDirectory( iconPathList );

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
mspApp::~mspApp()
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
void mspApp::InitializeRender(void *i_HwndFake, void *i_Hwnd, int i_Width, int i_Height)
{
	m_pSystem = GraphicsLayer::GetSystem2D();

	// This window is owned by the system
	if (!i_HwndFake && !i_Hwnd)
	{
		m_pWindow = m_pSystem->CreateSubWindow(appApplication::GetMainWindowHandle());
	}
	else
	{
		m_pWindow = m_pSystem->CreateSubWindow(i_Hwnd);
	}

	// Need to enable this before creating the g3dSystemDX11
	matShaderMgr::SetUseShaderArray(true);
	
	// Allow textures to be missing.
	matTextureMgr::SetAllowNullTextures(true);

	// maximum subdivision level allowed
	smdlSubdivCharacter::SetMaxSubdivLevel(2);

	m_pSystem3D = GraphicsLayer::GetSystem3D();

	// set up renderer and viewer for this window
	m_pRenderer = g3dSceneRendererCreate::CreateDefaultRenderer();
	
	// Going to have to set these up correctly later when we are using shaders. 
	// This is left over from the simple rendering built into the ModelInspector...
	g3dSingleLightRendering::SetDoSingleLightRendering(true); 
	//g3dSingleLightRendering::SetDoSingleLightRendering(false); // hook this up to a menu
	//g3dPrefs::CurrentPrefs().m_bHeadlightOn = true;
	// turning off these prefs will prevent some shaders from being loaded:
	g3dPrefs::CurrentPrefs().m_bEnableGlow = false;
	g3dPrefs::CurrentPrefs().m_bEnableDOF = false;
	g3dPrefs::CurrentPrefs().m_bEnableAO = false;

	m_pViewer = new g3dViewer(m_pWindow, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );
	//m_pViewer->SetBackgroundColor( g2dRGBColor(0x00, 0x00, 0x00) );

	api3dLightMgr::Initialize();
	//l_pDirLight = api3dLightMgr::CreateDirectionalLight();
	//l_pDirLight->SetDirection(maVector3d(0.2f, -0.7f, 0.1f));

	if (c_bDoSimpleOverride)
	{
		matTextureMgr::SetSkipAllTextures(true);

		m_pSimpleMaterial = new matMaterial("default");
		effPhongData* pSimpleData = dynamic_cast<effPhongData*>(m_pSimpleMaterial->GetEffectData());
		if (pSimpleData)
		{
			pSimpleData->m_ColorAmbient.Set(0.1f, 0.1f, 0.1f, 0.0f);
			pSimpleData->m_ColorSpecular.Set(0.1f, 0.1f, 0.1f, 0.0f);
		}
	}
	else
	{
		g3dLightMgr::SetAmbient(maFloatRGBA(0.1f, 0.1f, 0.1f, 0.0f));
	}

	cam3dMgr::Initialize();
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());

	api3dScene::Initialize();
	m_pViewer->SetScene(api3dScene::GetScene());

	// Custom lighting rig for shader ball
	m_pLighting = new mspLighting();

	DBG_WARNING("Video Hardware Name: " << m_pSystem3D->GetVideoAdapterName());

}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::ResizeRenderWindow(int i_Width, int i_Height)
{
	m_pWindow->ResizeWindow(i_Width, i_Height);
//	cam3dMgr::SetAspect(i_Width, i_Height);
}

//--------------------------------------------------------------------
//
//--------------------------------------------------------------------
void mspApp::DeInitializeRender()
{
	delete m_pLighting;
	api3dScene::DeInitialize();
	cam3dMgr::DeInitialize();

	api3dLightMgr::DeInitialize();

	if (m_pSimpleMaterial)
	{
		delete m_pSimpleMaterial;
		m_pSimpleMaterial = NULL;
	}

	delete m_pViewer;
	delete m_pRenderer;
}

//--------------------------------------------------------------------
// Called from appApplication::Run(), 
// do all of update and render at once.
//--------------------------------------------------------------------
void mspApp::Think()
{
	Update();
	Render();
}
//--------------------------------------------------------------------
// Called from MainForm through ThinkApp()
//--------------------------------------------------------------------
void mspApp::Update()
{
	bool bIncrementTime =((mspModel::HasAnimation()) &&
		(!mspViewSettings::sm_PauseAnimation.GetValue()));
	if (bIncrementTime)
	{
		// In playback so increment time
		appSimTime::IncrementTime();

		// Get current frame from the model and set it into the timeline
		int stepped_frame = (int) (mspModel::ComputeFrame(appSimTime::GetTime())); // snap to frame below

		// Loop around to begining if past the end
		int begin_frame = (int) mspViewSettings::sm_PlaybackBegin.GetValue();
		int end_frame = (int) mspViewSettings::sm_PlaybackEnd.GetValue();
		if (stepped_frame > end_frame)
			stepped_frame = begin_frame;
		else if (stepped_frame < begin_frame)
			stepped_frame = begin_frame;

		mspViewSettings::sm_CurrentFrame.SetValue( (float) stepped_frame );
	}
	else
	{
		// set time based on frame in timeline slider
		appSimTime::ResetTime(mspModel::ComputeTime( mspViewSettings::sm_CurrentFrame.GetValue() ));
	}

	float simTime = appSimTime::GetTime();

	//	poll devices
	//
	inDeviceMgr::Think();

	// think the cursor manager
	tma3dCursorMgr::Think();

	//
	//	input
	//
	inKeyboard* pKeyboard = inDeviceMgr::GetKeyboard();
	if ( pKeyboard )
	{
		//Spacebar will toggle play and stop.
		if (pKeyboard->IsReleased( inKeys::e_SPACE ))
		{
			//DBG_LOG0("Toggle Play and Stop");
			
			if(!mspViewSettings::sm_PauseAnimation.GetValue())
			{
				//DBG_LOG0("Stop");
				mspViewSettings::sm_PauseAnimation.SetValue(1);
			}
			else
			{
				//DBG_LOG0("Play");
				mspViewSettings::sm_PauseAnimation.SetValue(0);
			}

		}
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


	// Handle camera motion
	cam3dMgr::Think();

	// Force all materials to use Simple.fx
	if (c_bDoSimpleOverride)
	{
		// Force the material to simple material
		// DX11: BAD! need a generic way to do this!
		g3dDX11Util::SetOverrideMaterial(m_pSimpleMaterial);
	}

	// Animate model
	api3dScene::Think(simTime);
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::Render()
{
	if (mspApp::IsRenderEnabled())
	{
		m_pLighting->OrientLights();

		if (mspViewSettings::sm_ViewWireframe.GetValue())
			api3dScene::GetRoot(api3dScene::WorldLayerIndex())->SetDrawStyle(g3dSceneNode::e_Wireframe);
//			g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

		m_pViewer->SetCamera(&cam3dMgr::GetCamera());
		m_pViewer->Render( appSimTime::GetTime() );
		m_pViewer->Present();
		tma3dViewerMgr::RenderViews( appSimTime::GetTime() );

		if (mspViewSettings::sm_ViewWireframe.GetValue())
			api3dScene::GetRoot(api3dScene::WorldLayerIndex())->SetDrawStyle(g3dSceneNode::e_Solid);
//			g2dDX11Global::g_pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG("Start Event");

//	g2dScreen::InitializeWindow(640, 480, 0, 0);
//	g3dScene::Initialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	//g3dScene::DeInitialize();
	//g2dFontUtil::ReleaseAllFonts();
	//g2dScreen::DeInitialize();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
//	DBG_LOG("Suspend Event");
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void mspApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
//	DBG_LOG("Resume Event");
}

//--------------------------------------------------------------------
//	Override this function to get appQuitReqeustedEvents.
//--------------------------------------------------------------------
void mspApp::ReceiveQuitRequestEvent(appQuitRequestEvent& i_Event)
{
	// This is called when the user tries to close the non-managed window
	// from the button in the upper right corner
	this->Exit();
}



