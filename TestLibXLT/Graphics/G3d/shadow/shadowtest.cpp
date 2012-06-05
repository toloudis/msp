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

#include "anPackage.hpp"
#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appCharEvent.hpp"
#include "appCharEventHandler.hpp"
#include "appMouseEvent.hpp"
#include "appMouseEventHandler.hpp"
#include "appFlowEvent.hpp"
#include "appFlowEventHandler.hpp"
#include "appPackage.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "demModeManager.hpp"

#include "demG3dTestHierarch.hpp"
#include "demG3dTestShadow.hpp"

#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dScreen.hpp"
#include "g2dExceptionX.hpp"
#include "g3dPackage.hpp"
#include "g3dRenderer.hpp"
#include "g3dExceptionX.hpp"
#include "itPackage.hpp"
#include "gfPackage.hpp"

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

	g2dScreen::InitializeWindow(1024, 768, 100, 100);
//	g2dScreen::InitializeFullScreen(1024, 768, 32);
	g3dRenderer::Initialize();
	demModeManager::Push(new demG3dTestShadow);
	demModeManager::Push(new demG3dTestHierarch);
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	g3dRenderer::DeInitialize();
	g2dFontUtil::ReleaseAllFonts();
	g2dScreen::DeInitialize();
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

	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	itPackage::Init();
	appPackage::Init();
	gfPackage::Init();
	anPackage::Init();
	g2dPackage::Init();
	g3dPackage::Init();
	g3dPackage::SetShadowLayers(g3dPackage::e_World, g3dPackage::e_World);

	DBG_WARNING1("Video Hardware Name: %s", g3dPackage::GetVideoAdapterName());

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

	g3dPackage::CleanUp();
	g2dPackage::CleanUp();
	anPackage::CleanUp();
	gfPackage::CleanUp();
	appPackage::CleanUp();
	itPackage::CleanUp();
	fsPackage::CleanUp();
	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 13;
}
