#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

#include "Core/env/envPlatform.hpp"

#include "demTestLoadFile.hpp"
#include "demTestOrthographic.hpp"

#include "Tool/api3d/api3dLightMgr.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Core/app/appApplication.hpp"
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/app/appCharEvent.hpp"
#include "Core/app/appFlowEvent.hpp"
#include "Core/app/appMouseEvent.hpp"
#include "Core/app/appMouseEventHandler.hpp"
#include "Core/app/appModeMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "GraphicsDX9/g2d/g2dSystemDX9.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "GraphicsDX9/g3d/g3dSystemDX9.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "InputDI/in/inPackage.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/G3d/g3dSceneRenderer.hpp"
#include "Graphics/G3d/g3dSceneRendererCreate.hpp"

// Layers for init and cleanup
#include "Core/CoreLayer.hpp"
//#include "AudioLayer.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"

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
	if( appModeMgr::IsEmpty() )
		this->Exit();
	else
		appModeMgr::Think();
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
	GraphicsLayer::InitGraphics();
	GraphicsDX9Layer::InitGraphics();
		
	matShaderMgr::SetUseShaderArray(false);


	// set up renderer and viewer for this window
	m_pRenderer = g3dSceneRendererCreate::CreateDefaultRenderer();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );

	// Set up Tool3D
	api3dLightMgr::Initialize();
	cam3dMgr::Initialize();
	m_pViewer->SetCamera(&cam3dMgr::GetCamera());
	api3dScene::Initialize();
	m_pViewer->SetScene(api3dScene::GetScene());

	// Note: add new modes here
	appModeMgr::Push(new demTestOrthographic(*m_pViewer));
	//appModeMgr::Push(new demTestLoadFile(*m_pViewer));
	
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();

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

	CoreLayer::Init();
	GraphicsLayer::Init();
	GraphicsDX9Layer::Init();
	
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

	GraphicsDX9Layer::CleanUp();
	GraphicsLayer::CleanUp();
	CoreLayer::CleanUp();
	return 13;
}
