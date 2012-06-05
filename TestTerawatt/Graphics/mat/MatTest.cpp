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

#include "demMatTestTextureCompression.hpp"

#include "envPackage.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "fsFileX.hpp"
#include "g2dExceptionX.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dSystemD3D.hpp"
#include "g3dExceptionX.hpp"
#include "g3dPackage.hpp"
#include "g3dSystemD3D.hpp"
#include "g3dSceneRendererD3D.hpp"
#include "g3dViewer.hpp"
#include "gfPackage.hpp"
#include "itPackage.hpp"
#include "matPackage.hpp"
#include "tmeshPackage.hpp"

namespace
{

bool l_bWindowed = true; //false;

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
	g2dSystemD3D* m_pSystem;
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

	m_pSystem = new g2dSystemD3D();

	// This window is owned by the system
	g2dWindow *window = NULL;
	if( l_bWindowed )
		window = m_pSystem->CreateAppWindow(640, 480, 100, 100);
	else
		window = m_pSystem->CreateFullScreen(1024, 768, 16);

	m_pSystem3D = new g3dSystemD3D();
	DBG_WARNING1("Video Hardware Name: %s", m_pSystem3D->GetVideoAdapterName());

	// set up renderer and viewer for this window
	m_pRenderer = new g3dSceneRendererD3D();
	m_pViewer = new g3dViewer(window, m_pRenderer);
	m_pViewer->SetBackgroundColor( g2dRGBColor(0x30, 0x80, 0xa0) );

	demModeManager::Push(new demMatTestTextureCompression(*m_pViewer));
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

	envPackage::Init();
	dbgPackage::Init();
	fsPackage::Init();
	itPackage::Init();
	appPackage::Init();
	gfPackage::Init();
	anPackage::Init();
	g2dPackage::Init();
	g3dPackage::Init();
	matPackage::Init();
	tmeshPackage::Init();

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
		throw;
	}
	catch( ... )
	{
		DBG_WARNING0("Uncaught non-Terawatt exception");
		throw;
	}

	tmeshPackage::CleanUp();
	matPackage::CleanUp();
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
