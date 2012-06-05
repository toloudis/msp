#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

// Layers for init and cleanup
#include "AppLayer.hpp"
//#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
#include "EffectsLayer.hpp"
#include "GraphicsLayer.hpp"
#include "MathLayer.hpp"
#include "ModelLayer.hpp"

#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appFlowEventHandler.hpp"
#include "dbgLog.hpp"
#include "demModeManager.hpp"

#include "demPrtTestStaticParticle.hpp"
#include "demPrtTestConeParticle.hpp"
//#include "demPrtTestCone3DParticle.hpp"
#include "demPrtTestSpiralParticle.hpp"

#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dExceptionX.hpp"
#include "g3dExceptionX.hpp"
//#include "inPackage.hpp"

#include "g2dSystemD3D.hpp"
#include "g3dSceneRendererD3D.hpp"
#include "g3dSystemD3D.hpp"
#include "g3dViewer.hpp"

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
	g2dSystem* m_pSystem;
	g3dSystem* m_pSystem3D;
	g3dSceneRenderer* m_pRenderer;
	g3dViewer* m_pViewer;
};

MyApp::MyApp() 
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

	this->SetWindowTitle(itString("Particle Test"));

	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();

	m_pSystem = new g2dSystemD3D();

	// This window is owned by the system
	g2dWindow *window = m_pSystem->CreateAppWindow(640, 480, 100, 100);
	//g2dWindow *window = m_pSystem->CreateFullScreen(1024, 768, 16);

	m_pSystem3D = new g3dSystemD3D();
	EffectsLayer::Init();
	//	This should be done after the application window is created
	//inPackage::Init();

	// set up renderer and viewer for this window
	m_pRenderer = new g3dSceneRendererD3D();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );


	//demModeManager::Push(new demPrtTestCone3DParticle(*m_pViewer));
	demModeManager::Push(new demPrtTestSpiralParticle(*m_pViewer));
	demModeManager::Push(new demPrtTestConeParticle(*m_pViewer));
	demModeManager::Push(new demPrtTestStaticParticle(*m_pViewer));
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	delete m_pRenderer;
	delete m_pViewer;

	EffectsLayer::CleanUp();
	//inPackage::CleanUp();
	delete m_pSystem3D;
	delete m_pSystem;
	g2dFontUtil::ReleaseAllFonts();

	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
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
int WINAPI WinMain(
					HINSTANCE hInstance,      // handle to current instance
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
	//AudioLayer::Init();

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
		throw;
	}
	catch( ... )
	{
		DBG_WARNING0("Uncaught non-Terawatt exception");
		throw;
	}

	//AudioLayer::CleanUp();
	ModelLayer::CleanUp();
	GraphicsLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();

	return 13;
}
