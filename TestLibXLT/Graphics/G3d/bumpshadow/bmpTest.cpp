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
#include "camCameraMgr.hpp"
#include "tstModeBumpMaterialTest.hpp"

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
#include "g2dPackage.hpp"
#include "g2dFontUtil.hpp"
#include "g2dScreen.hpp"
#include "g3dPackage.hpp"
#include "g3dScene.hpp"
#include "gfPackage.hpp"
#include "inPackage.hpp"
#include "itPackage.hpp"
#include "mayPackage.hpp"
#include "scCamera.hpp"
#include "scPackage.hpp"
//#include "tilGridSpec.hpp"
//#include "tilTile.hpp"

// STL?


namespace
{
	// the camera used by this program
	scCamera	l_Camera;  // Inialized in ReceiveStartEvent

	// CUSTOM:  Put your modes here
	enum eModes
	{
		e_BumpMaterial = appModeList::e_UserBegin,

		e_NumModes
	};

	//====================================================================
	// initialize_modes -- called on ReceiveStartEvent it will allocate
	// all custom modes and add them to appModeList
	//====================================================================
	void initialize_modes()
	{
		// allocate modes and add to appModeList
		appModeList::Init( e_NumModes );

		appModeList::SetMode(e_BumpMaterial, new tstModeBumpMaterialTest);
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
		appModeMgr::Push( appModeList::GetMode( e_BumpMaterial ) );
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
		delete appModeList::GetMode( e_BumpMaterial );
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
};

MyApp::MyApp() 
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

	int width = 620;
	int height = 480;
	g2dScreen::InitializeWindow(width, height, 100, 100);
//	g2dScreen::InitializeFullScreen(1024, 768, 16);
	g3dScene::Initialize();

	// camera
	g2dScreen::GetDimensions( width, height );
	l_Camera.SetAspect( float( width ) / height );
	l_Camera.SetFOV( 90.0f );
	l_Camera.SetClip( 0.5f, 2000.0f );
	camCameraMgr::Initialize(&l_Camera);

	// generic staring point 25 meters away looking at the origin
	l_Camera.LookAt( maPoint3d(0.0f,3.0f,-25.0f), maPoint3d(0.0f,0.0f,0.0f), maVector3d(0.0f,1.0f,0.0f) );
	camCameraMgr::SetCurrentManip(camCameraMgr::e_Orbit);

	// Allocate and push all modes
	initialize_modes(); // alloc and add to appModeList

	push_modes();
}

void MyApp::ReceiveStopEvent(appStopEvent& i_Event)
{
	// CleanUp the modes
	deinitialize_modes(); // clean up...destroy in our destructor!

	//	This should be done before the application window is destroyed
	inPackage::CleanUp();

	camCameraMgr::DeInitialize();

	g3dScene::DeInitialize();
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


	envPackageCleanUp<anPackage> an_package;
	envPackageCleanUp<g2dPackage> g2d_package;

	try
	{
		g3dPackage::Init();
	}
	catch( ... )
	{
		return 15;
	}

	envPackageCleanUp<g3dPackage> g3d_package(false);  // initialized above
	envPackageCleanUp<mayPackage> may_package;
	envPackageCleanUp<scPackage> sc_package;

//	tilGridSpec::SetGridUnit(128.0f);

//	envPackageCleanUp<phyPackage> phy_package;
//	envPackageCleanUp<colPackage> col_package;

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

	return 13;
}
