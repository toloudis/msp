// Windows-Specific Requirements first
#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#undef CreateFile
#undef CopyFile
#undef DeleteFile
#undef CreateDirectory
#undef DrawText

// Application requirements
#include "g2dModeBaseTest.hpp"
#include "g2dModeLoadTexture.hpp"
#include "g2dModeWindowTest.hpp"

// Terawatt requirements
#include "anPackage.hpp"
#include "appApplication.hpp"
#include "appApplicationPAC.hpp"
#include "appModeList.hpp"
#include "appModeMgr.hpp"
#include "appPackage.hpp"
#include "appSimTime.hpp"
#include "dbgLog.hpp"
#include "dbgPackage.hpp"
#include "envPackage.hpp"
#include "envPackageCleanUp.hpp"
#include "fsFileX.hpp"
#include "fsPackage.hpp"
#include "fsFileUtil.hpp"
#include "g2dFontUtil.hpp"
#include "g2dPackage.hpp"
#include "g2dSystemD3D.hpp"
#include "gfPackage.hpp"
#include "inPackage.hpp"
#include "itPackage.hpp"

// STL?


namespace
{
	// the camera used by this program
	//scCamera	l_Camera;  // Inialized in ReceiveStartEvent

	// CUSTOM:  Put your modes here
	enum eModes
	{
		e_BaseTest		= appModeList::e_UserBegin,
		e_WindowTest,
		e_LoadTexture,

		e_NumModes
	};

	//====================================================================
	// initialize_modes -- called on ReceiveStartEvent it will allocate
	// all custom modes and add them to appModeList
	//====================================================================
	void initialize_modes(g2dSystemD3D &i_System, g2dWindow &i_Window)
	{
		// allocate modes and add to appModeList
		appModeList::Init( e_NumModes );

		appModeList::SetMode(e_BaseTest, new g2dModeBaseTest(i_Window));
		appModeList::SetMode(e_WindowTest, new g2dModeWindowTest(i_System, i_Window));
		appModeList::SetMode(e_LoadTexture, new g2dModeLoadTexture(i_Window));
	}

	//====================================================================
	// push_modes will push all the modes to the appModeMgr in succession
	// last to first
	//====================================================================
	void push_modes()
	{
		// For most tests, it is sufficient to push all the modes
		// in succession.  SPACE bar will kill a mode and remove it
		// thus moving to the next.  ESC will terminate the application
		// by emptying the appModeMgr and then exiting.
		appModeMgr::Push( appModeList::GetMode( e_LoadTexture ) );
		appModeMgr::Push( appModeList::GetMode( e_BaseTest ) );
		appModeMgr::Push( appModeList::GetMode( e_WindowTest ) );
	}

	//====================================================================
	// deinitialize_modes -- Called by ReceiveStopEvent to cleanup your modes
	// but they may NOT be deleted here.  Do the majority of cleanup here
	// as necessary.
	//====================================================================
	void deinitialize_modes()
	{
		// nothing to do, no additional cleanup necessary
	}

	//====================================================================
	// destroy_modes -- called from the app destructor to destroy all modes
	//====================================================================
	void destroy_modes()
	{
		delete appModeList::GetMode( e_BaseTest );
		delete appModeList::GetMode( e_LoadTexture );
		delete appModeList::GetMode( e_WindowTest );
		appModeList::CleanUp();
	}
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
	g2dSystemD3D *m_pSystem;
};

MyApp::MyApp() : m_pSystem(NULL)
{
}

MyApp::~MyApp() 
{
	destroy_modes();
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

	// finish intializing
	appSimTime::SetMaxTimeDelta(0.5f);

	inPackage::Init();	// input package

	int width	= 1024;
	int height	= 768;
	//g2dScreen::InitializeWindow(width, height, 100, 100);

	m_pSystem = new g2dSystemD3D();
	// This window is owned by the system
	g2dWindow *window = m_pSystem->CreateAppWindow(width, height, 100, 100);

	// Allocate and push all modes
	initialize_modes(*m_pSystem, *window); // alloc and add to appModeList

	push_modes();
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	// CleanUp the modes
	deinitialize_modes(); // clean up...destroy in our destructor!

	if (m_pSystem)
	{
		delete m_pSystem;
		m_pSystem = NULL;
	}

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
	
	envPackageCleanUp<envPackage> env_package;
	envPackageCleanUp<dbgPackage> dbg_package;
	envPackageCleanUp<fsPackage> fs_package;
	envPackageCleanUp<itPackage> it_package;
	envPackageCleanUp<gfPackage> gf_package;
	// error handler would be setup here

//	appBasicStrings::SetupBasicStrings();
	envPackageCleanUp<appPackage> app_package;

	// only g2d package being tested here
	envPackageCleanUp<g2dPackage> g2d_package;

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

	return 13;
}
