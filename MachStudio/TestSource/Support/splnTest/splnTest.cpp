#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "envPlatform.hpp"

#include "demModeManager.hpp"
#include "demTestSplineOrient.hpp"

#include "api3dLightMgr.hpp"
#include "api3dScene.hpp"
#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appCharEvent.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appFlowEventHandler.hpp"
#include "cam3dMgr.hpp"
#include "dbgLog.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dSystemDX9.hpp"
#include "g2dExceptionX.hpp"
#include "g3dExceptionX.hpp"
#include "g3dSystemDX9.hpp"
#include "g3dViewer.hpp"
#include "inPackage.hpp"
#include "matTextureMgr.hpp"
//#include "g3dSceneRendererDX9.hpp"
#include "shdwShadowSceneRendererD3D.hpp"

// Layers for init and cleanup
#include "AppLayer.hpp"
//#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
#include "GraphicsLayer.hpp"
#include "MathLayer.hpp"
#include "ModelLayer.hpp"

namespace
{

bool l_bWindowed = true;

}

//====================================================================
//====================================================================
class MyApp	:	public appApplication,
				public appFlowEventHandler
{
	public:

		MyApp();
		~MyApp();

		//====================================================================
		//	Override this function to get appStartEvents.
		//====================================================================
		virtual void ReceiveStartEvent(appStartEvent& i_Event);

		//====================================================================
		//	Override this function to get appStopEvents.
		//====================================================================
		virtual void ReceiveStopEvent(appStopEvent& i_Event);

		//====================================================================
		//	Override this function to get appSuspendEvents.
		//====================================================================
		virtual void ReceiveSuspendEvent(appSuspendEvent& i_Event);

		//====================================================================
		//	Override this function to get appResumeEvents.
		//====================================================================
		virtual void ReceiveResumeEvent(appResumeEvent& i_Event);

		//====================================================================
		//====================================================================
		virtual void Think();

private:
	g2dSystemDX9* m_pSystem;
	g3dSystem* m_pSystem3D;
	g3dSceneRenderer* m_pRenderer;
	g3dViewer* m_pViewer;
};

MyApp::MyApp()  : m_pSystem(NULL), m_pSystem3D(NULL), m_pRenderer(NULL), m_pViewer(NULL)
{
}

MyApp::~MyApp() 
{
}

void MyApp::Think()
{
	if( demModeManager::IsEmpty() )
		this->Exit();
	else
		demModeManager::Think();
}

void MyApp::ReceiveStartEvent(appStartEvent& i_Event)
{
	DBG_LOG0("Start Event");

	m_pSystem = new g2dSystemDX9();

	// This window is owned by the system
	g2dWindow *window = NULL;
	if( l_bWindowed )
		window = m_pSystem->CreateAppWindow(640, 480, 100, 100);
	else
		window = m_pSystem->CreateFullScreen(1024, 768, 16);

	m_pSystem3D = new g3dSystemDX9();
	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

	// Initialize rest of Terawatt after window is created.
	inPackage::Init();
	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();
	ModelLayer::InitGraphics();


	// set up renderer and viewer for this window
	//m_pRenderer = new g3dSceneRendererDX9();
	m_pRenderer = new shdwShadowSceneRendererD3D();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );

	// Set up Tool3D
	api3dLightMgr::Initialize();
	cam3dMgr::Initialize();
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());
	api3dScene::Initialize();
	m_pViewer->SetScene(api3dScene::GetScene());

	// Note: add new modes here
	demModeManager::Push(new demTestSplineOrient(*m_pViewer));
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	delete m_pSystem3D;
	g2dFontUtil::ReleaseAllFonts();
	delete m_pSystem;
	delete m_pRenderer;
	delete m_pViewer;
}

void MyApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG0("Suspend Event");
}

void MyApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG0("Resume Event");
}

//====================================================================
//====================================================================
int WINAPI WinMain(	HINSTANCE hInstance,      // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
					LPSTR lpCmdLine,          // command line
					int nCmdShow)             // show state
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	BaseLayer::Init();
	AppLayer::Init();
	MathLayer::Init();
	GraphicsLayer::Init();
	ModelLayer::Init();

	matTextureMgr::SetAllowNullTextures(true);

	std::string cmd_line = lpCmdLine;
	if( cmd_line.find("/f") != std::string::npos )
		l_bWindowed = false;
	else
		l_bWindowed = true;
				
	try
	{
		MyApp app;

		app.Run();
	}
	catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING1("File not found: %s", filename.c_str());
	}
	catch( const envExceptionX& i_Ex )
	{
		DBG_WARNING1("Uncaught exception - error code %x", i_Ex.Index());
		//throw;
	}
	catch( ... )
	{
		DBG_WARNING0("Uncaught non-Terawatt exception");
		//throw;
	}

	ModelLayer::CleanUp();
	GraphicsLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();
	return 13;
}
