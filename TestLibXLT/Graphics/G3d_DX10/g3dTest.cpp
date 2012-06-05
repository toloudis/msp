#include "demModeManager.hpp"

#include "demG3dTestLights.hpp"
#include "demG3dTestRenderNormals.hpp"
#include "demG3dTestRenderText.hpp"
#include "demG3dTestTessellation.hpp"
#include "demG3dTestTexturedQuad.hpp"
#include "demG3dTestWindow.hpp"

#include "Core/CoreLayer.hpp"
#include "Core/app/appApplication.hpp"
#include "Core/app/private/appApplicationPAC.hpp"	
#include "Core/app/appFlowEventHandler.hpp"
#include "Core/gf/gfPackage.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "Graphics/g2d/g2dFontUtil.hpp"
#include "Graphics/g2d/g2dSystem.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSceneRendererCreate.hpp"
#include "Graphics/g3d/g3dSystem.hpp"
#include "Graphics/g3d/g3dViewer.hpp"
#include "Graphics/mat/matShaderMgr.hpp"

//#define TEST_DX9 1
//#define TEST_DX10 1
#define TEST_DX11 1

#ifdef TEST_DX11
	#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#endif
#ifdef TEST_DX10
	#include "GraphicsDX10/GraphicsDX10Layer.hpp"
#endif
#ifdef TEST_DX9
	#include "GraphicsDX9/GraphicsDX9Layer.hpp"
#endif

#include <windows.h>
//#include "MemMgr.h"


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
	g3dSceneRenderer* m_pRenderer;
	g3dViewer* m_pViewer;
};

MyApp::MyApp()  : m_pRenderer(NULL), m_pViewer(NULL)
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
	DBG_LOG("Start Event");

	GraphicsLayer::Init();
#ifdef TEST_DX11
	GraphicsDX11Layer::Init(appApplication::GetMainWindowHandle());
	GraphicsLayer::InitGraphics(GraphicsDX11Layer::GetSystem2D(), GraphicsDX11Layer::GetSystem3D());
	GraphicsDX11Layer::InitGraphics();
#endif
#ifdef TEST_DX10
	GraphicsDX10Layer::Init(appApplication::GetMainWindowHandle());
	GraphicsLayer::InitGraphics(GraphicsDX10Layer::GetSystem2D(), GraphicsDX10Layer::GetSystem3D());
	GraphicsDX10Layer::InitGraphics();
#endif
#ifdef TEST_DX9
	GraphicsDX9Layer::Init(appApplication::GetMainWindowHandle());
	GraphicsLayer::InitGraphics(GraphicsDX9Layer::GetSystem2D(), GraphicsDX9Layer::GetSystem3D());
	GraphicsDX9Layer::InitGraphics();
#endif

	// This window is owned by the system
	g2dWindow* window = GraphicsLayer::GetSystem2D()->CreateSubWindow(appApplication::GetMainWindowHandle());

	matShaderMgr::SetUseShaderArray(true);
//	DBG_WARNING("Video Hardware Name: " << GraphicsLayer::GetSystem3D()->GetVideoAdapterName());

	// set up renderer and viewer for this window
	m_pRenderer = NULL;//g3dSceneRendererCreate::CreateDefaultRenderer();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );
	

	// The following modes work. Some may be commented out temporarily
	// in order to work on a specific mode. Uncomment them all in
	// to see the full test.

//	demModeManager::Push(new demG3dTestLights(*m_pViewer));


//	demModeManager::Push(new demG3dTestTessellation(*m_pViewer));
//	demModeManager::Push(new demG3dTestRenderNormals(*m_pViewer));
	demModeManager::Push(new demG3dTestRenderText(*m_pViewer));
//	demModeManager::Push(new demG3dTestTexturedQuad(*m_pViewer));
//	demModeManager::Push(new demG3dTestWindow(*window));
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	delete m_pViewer;
	delete m_pRenderer;

#ifdef TEST_DX11
	GraphicsDX11Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	GraphicsDX11Layer::CleanUp();
#endif
#ifdef TEST_DX10
	GraphicsDX10Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	GraphicsDX10Layer::CleanUp();
#endif
#ifdef TEST_DX9
	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	GraphicsDX9Layer::CleanUp();
#endif

	GraphicsLayer::CleanUp();
}

void MyApp::ReceiveSuspendEvent(appSuspendEvent& i_Event)
{
	DBG_LOG("Suspend Event");
}

void MyApp::ReceiveResumeEvent(appResumeEvent& i_Event)
{
	DBG_LOG("Resume Event");
}

//====================================================================
//====================================================================
int WINAPI WinMain(	HINSTANCE hInstance,      // handle to current instance
					HINSTANCE hPrevInstance,  // handle to previous instance
					LPSTR lpCmdLine,          // command line
					int nCmdShow)             // show state
{

	_crtBreakAlloc = 0;

	appApplicationPAC::SetHINSTANCE(hInstance);

	gfPackage::Init();
	CoreLayer::Init();

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
/*
catch( const fsFileDoesntExistX& i_Ex )
	{
		std::string filename;
		fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);
		DBG_WARNING("File not found: " << filename);
	}
	catch( const envExceptionX& i_Ex )
	{
		DBG_WARNING("Uncaught exception - error code " << i_Ex.Index());
		throw;
	}
*/
	catch( ... )
	{
		DBG_WARNING("Uncaught non-Terawatt exception");
		throw;
	}

	CoreLayer::CleanUp();
	gfPackage::CleanUp();
	return 13;
}
