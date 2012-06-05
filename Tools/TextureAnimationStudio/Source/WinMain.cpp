#include "stdafx.h"

// Local Project includes
#include "Form1.h"
//#include "mainDocumentInterest.hpp"

// SourceMan/NonMan and System includes
#include "tasApp.hpp"
#include "tasPaths.hpp"
#include "tasTUVMgr.hpp"
#include "tasUVAMgr.hpp"
//#include "cptrScreenCaptureUtil.hpp"
//#include "mnmTimeCodeUtil.hpp"
//#include "mnmVJoystick.hpp"
//#include "billSystem.hpp"
//#include "chtrSystem.hpp"
//#include "cmraSystem.hpp"
//#include "cmmSystem.hpp"
//#include "dcutSystem.hpp"
////#include "dirltSystem.hpp"
//#include "envtSystem.hpp"
//#include "fogSystem.hpp"
#include "gfPackage.hpp"
//#include "grupSystem.hpp"
//#include "lsetSystem.hpp"
//#include "lyrsSystem.hpp"
//#include "PrefsData.hpp"
//#include "PrefsMgr.hpp"
//#include "propSystem.hpp"
//#include "prtclSystem.hpp"
//#include "prjltSystem.hpp"
//#include "ptltSystem.hpp"
//#include "rcdSystem.hpp"
//#include "setsSystem.hpp"
////#include "skySystem.hpp"
////#include "todSystem.hpp"

// MachStudio layers 
//#include "SupportLayer.hpp"
//#include "FeaturesLayer.hpp"

// Tool includes
#include "docSingleDocumentMgr.hpp"
#include "docSingleTypeMgr.hpp"
//#include "muiRegistryUtil.hpp"
//#include "prtyCallbackMgr.hpp"
//#include "prtyControlFactoryBase.hpp"
//#include "prtyControlFactoryTMC.hpp"
//#include "prtyControlMgr.hpp"
//#include "rstkDocumentInterest.hpp"
#include "tmaSingleDocHandler.hpp"
//#include "tmaSplashMessage.hpp"
//#include "tmaSplashMgr.hpp"
#include "tma3dScreenUtil.hpp"
//#include "tmlnTimeLine.hpp"

// Terawatt includes
#include "appApplicationPAC.hpp"
//#include "dayPackage.hpp"
#include "dbgLog.hpp"
#include "fsResourceTracker.hpp"
//#include "gfPaths.hpp"
#include "inPackage.hpp"
//#include "snSoundManager.hpp"
//#include "snSoundSystem.hpp"

// Layers for init and cleanup
#include "AppLayer.hpp"
//#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
//#include "EffectsLayer.hpp"
#include "GraphicsLayer.hpp"
#include "MathLayer.hpp"
#include "ModelLayer.hpp"


//============================================================================
//============================================================================
using namespace System;
using namespace System::Windows::Forms;
using namespace ConceptCave::WaitCursor;
using namespace TerawattManagedControls;


//============================================================================
//============================================================================
HANDLE g_hMutexAppRunning = NULL;


//============================================================================
//
//	terawatt functions
//
//============================================================================
void initialize_Terawatt_base()
{
	// initialize everything
	//
	BaseLayer::Init();
	AppLayer::Init();
	MathLayer::Init();
	fsResourceTracker::Init();
}

void initialize_Terawatt_postApp()
{
	// initialize everything else
	//
	GraphicsLayer::Init();
	ModelLayer::Init();
//	AudioLayer::Init();

//	cptrScreenCaptureUtil::Initialize();	// no deinitialize
//	tmlnTimeLine::Initialize();
}

//
void deinitialize_Terawatt()
{
	// clean-up everything
	//
	fsResourceTracker::CleanUp();
//	tmlnTimeLine::DeInitialize();

//	AudioLayer::CleanUp();
	ModelLayer::CleanUp();
	GraphicsLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();
}

bool app_instance_exists(LPCTSTR i_pName)
{
	// Create a global mutex. Use a unique name, for example
	// incorporating your company and application name.
	g_hMutexAppRunning = CreateMutex( NULL, false, i_pName);

	// Check if the mutex object already exists, indicating an
	// existing application instance
	if (   ( g_hMutexAppRunning != NULL ) 
		&&
		   ( GetLastError() == ERROR_ALREADY_EXISTS))
	{
		// Close the mutex for this application instance. This assumes
		// the application will inform the user that it is
		// about to terminate
		CloseHandle( g_hMutexAppRunning );
		g_hMutexAppRunning = NULL;
	}

	// Return False if a new mutex was created,
	// as this means it's the first app instance
	return ( g_hMutexAppRunning == NULL );
}
bool CALLBACK Report(int hwnd, int lParam) 
{
	LPTSTR lpString = new char[128];
	GetWindowText((HWND)hwnd, lpString, 128);

	LPTSTR lpStringF = new char[128];
	GetWindowModuleFileName((HWND)hwnd,lpStringF,256);

	DBG_LOG2("Window title(%s) filename(%s)", lpString, lpStringF);

	return true;
}
bool app_instance_exists()
{
	bool bAppRunning = false;

	bAppRunning |= app_instance_exists("Global\\TextureAnimationStudio.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-D.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-Debug Managed.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-Release Managed.exe");

	//EnumWindows((WNDENUMPROC)Report, 0); 

	return bAppRunning;
}

//
// Main entry point
//
[STAThreadAttribute]
int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	//	don't allow MS to launch more than once.
	//
	// NOTE: How does this play out with Fast User Switching? [rjk]
	if (app_instance_exists())
	{
		HWND hWndOtherInstance;
		hWndOtherInstance = FindWindow(appApplicationPAC::GetWindowClassName(), NULL);	// window class, title
		if ( hWndOtherInstance != (HWND)NULL )
		{
			// Application is running in current user's session
			if (IsIconic(hWndOtherInstance))
				ShowWindow(hWndOtherInstance, SW_RESTORE);
			SetForegroundWindow(hWndOtherInstance);
		}
		else
		{
			//MessageBox(NULL, TEXT("An instance of this app is running"), l_cWINDOW_TITLE, MB_OK);
		}
		return false;
	}

	// Add the following line to your Application Startup code somewhere (the earlier the better)
	ApplicationWaitCursor::Cursor = Cursors::WaitCursor;		// You can use an Cursor you like aswell

	// Use the following line (at any point in your App, or just once) to configure the length
	// of time to wait before showing the WaitCursor
	ApplicationWaitCursor::Delay  = TimeSpan(0, 0, 0, 0, 400);

	//
	appApplicationPAC::SetHINSTANCE(hInstance);

	gfPackage::Init();
	fsLocator appPath = gfPaths::GetPath(gfPaths::e_ExePath);
	tasPaths::SetupPaths( appPath );
//	muiRegistryUtil::Init("MachStudio");

	TextureAnimationStudio::Form1^ main_form = gcnew TextureAnimationStudio::Form1();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	tasApp *pApp = new tasApp();
	tasApp::SetActive( false );

	initialize_Terawatt_postApp();

//#ifndef _DEBUG	// don't show the splash if in debug mode
//	//	Splash screen
//	SplashImage* m_Splash;
//	fsLocator exePath = gfPaths::GetPath(gfPaths::e_ExePath);
//	exePath.Push("Data");
//	exePath.Push("SplashMach.png");
//	std::string sfile;
//	fsFileUtil::LocatorToANSIFilename(exePath,sfile);
//	m_Splash = new SplashImage(sfile.c_str());
//	m_Splash->SplashFadeInTime = 500;
//	m_Splash->SplashFadeOutTime = 500;
//	//m_Splash->SetTextColor(255,255,255,0,0,0);
//	m_Splash->SetTextColor(255,0,0,0,255,0);
//	m_Splash->SplashTextX = 42;
//	m_Splash->SplashTextY = 200;
//	m_Splash->SplashString = "Welcome...One Moment Please";
//	m_Splash->StartUp();
//#endif

//	//	show the splash form
//	//Splash::Loader::Show();
//	tmaSplashMgr::Init();
//	System::String* splashImage = tmaManagedStringUtils::LocatorToManagedString( gfPaths::GetPath(tasPaths::e_ExeArt) );
//	splashImage = String::Concat( splashImage, S"\\Splash.png" );
//	tmaSplashMgr::GetSplashMessage()->SetImageFile( splashImage );
//	tmaSplashMgr::GetSplashMessage()->SetMessageText( S"Intializing Mach Studio..." );
//#ifndef _DEBUG	// don't show the splash if in debug mode
//	tmaSplashMgr::GetSplashMessage()->Show();
//#endif

	//	set-up rendering window
	//
	pApp->InitializeRender( (void*)main_form->GetTUVRenderInitWindow()->Handle,
		(void*)main_form->GetTUVRenderWindow()->Handle,
		main_form->GetTUVRenderWindow()->Width,
		main_form->GetTUVRenderWindow()->Height);

	System::Drawing::Rectangle rect = main_form->GetTUVRenderWindow()->ClientRectangle;
	tma3dScreenUtil::SetWindowSize( maPoint2d( rect.Left, rect.Top ),
		maPoint2d( rect.Width, rect.Height ) );

	DBG_ASSERT0( pApp->GetWindow() != 0, "No main render window" );

//	prtyCallbackMgr::Initialize();
//	prtyControlMgr::Initialize();
//	prtyControlMgr::AddControlFactory( new prtyControlFactoryBase() );
//	prtyControlMgr::AddControlFactory( new prtyControlFactoryTMC() );
//
//	mnmTimeCodeUtil::SetMainWindow( pApp->GetWindow() );
//	mnmTimeCodeUtil::SetWindow( pApp->GetWindow() );

	inPackage::Init();
//	mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick
//	
//	snSoundSystem::Initialize();
//	snSoundManager::Initialize();

	ModelLayer::InitGraphics();
//	EffectsLayer::Init();
//	initialize_objects();

	tasUVAMgr::Initialize();
	tasTUVMgr::Initialize();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
//	PrefsData prefsdata = PrefsMgr::Data();
//	docSingleTypeMgr::Init( prefsdata.m_MRUHistory.GetValue() );
//	docSingleTypeMgr::SetFilter(".mab", "Mach Files");

	// grab the top item off of the MRU list and use that directory
	// as the default.  If the MRU is empty then set MS dir.
	//
//	std::vector<fsLocator>	mru_list;
//	docSingleTypeMgr::GetMRUList( mru_list );
//	if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
//	{
//		fsLocator dir = mru_list[0];
//		dir.Pop();	// remove the filename
//		tmaSingleDocHandler::SetInitialDirectory( dir );
//	}
//	else
//	{
		tmaSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( tasPaths::e_SaveShots ) );
//	}
//	tmaSingleDocHandler::SetInitialDirectoryToBeLast(true);

	//
	//
//	cmmSystem::Init();						// Init common first
//	SupportLayer::Init();
//	FeaturesLayer::Init(pApp->GetSystem());

	//	set-up the main menu items
	//
//	mainCommands::SetupMenu();

	// Initialize MachStudio systems here
	//
	fsLocator app_dir( gfPaths::GetPath(gfPaths::e_AppPath) );
//	setsSystem::Init(app_dir, itString("Sets"));
//	propSystem::Init(itString("Props"));
//	chtrSystem::Init(app_dir, itString("Characters"));
//	prtclSystem::Init(app_dir, itString("Effects"));
//	billSystem::Init(app_dir, itString("Effects"));
//	ptltSystem::Init();
//	prjltSystem::Init(app_dir, itString("Effects"));
//	cmraSystem::Init(app_dir, itString("Sets")); // could have Cameras dir later?
//	dcutSystem::Init(pApp->GetSystem());
////	dirltSystem::Init();
//	fogSystem::Init();
//	//todSystem::Init();
////	skySystem::Init();
//	rcdSystem::Init();
//	// Putting these last makes sure that the lights and objects 
//	// are written to file before the light sets and layers
//	lsetSystem::Init();	
//	lyrsSystem::Init();	
//	grupSystem::Init();	
//	envtSystem::Init(app_dir, itString("Effects"));

	// Add ResourceTracker document interest
//	docSingleTypeMgr::AddDocumentInterest(new rstkDocumentInterest());

	// Add the "version" document chunk LAST so it gets written at the end.
//	std::string exestr;
//	tmaManagedStringUtils::ManagedStringToStdString(main_form->GetExecutableVersion(), exestr);
//	docSingleTypeMgr::AddDocumentInterest(new mainDocumentInterest(exestr));

	// Make NewDocument here after document interests have been registered
//	tmaSingleDocHandler::New();

	//	run the app
	//
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

//#ifndef _DEBUG	// don't show the splash if in debug mode
//	m_Splash->ShutDown();
//	//tmaSplashMgr::GetSplashMessage()->Hide();
//#endif

	//
	//	parse the command line and configure the app accordingly
	//
	std::string cmdline( lpCmdLine );
//	tasApp::ParseCommandLine(cmdline);

	//	set the app active right before the app runs
	tasApp::SetActive( true );

	//
	//	the "run" loop
	//
	Application::Run( main_form );
	//
	//

	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	tmaSingleDocHandler::Exit();
//	tmaSplashMgr::CleanUp();

//	// Clean up systems
//	envtSystem::CleanUp();
//	grupSystem::CleanUp();
//	lyrsSystem::CleanUp();
//	lsetSystem::CleanUp();
//	rcdSystem::CleanUp();
////	skySystem::CleanUp();
//	//todSystem::CleanUp();
//	fogSystem::CleanUp();
////	dirltSystem::CleanUp();
//	dcutSystem::CleanUp();
//	cmraSystem::CleanUp();
//	prjltSystem::CleanUp();
//	ptltSystem::CleanUp();
//	billSystem::CleanUp();
//	prtclSystem::CleanUp();
//	propSystem::CleanUp();
//	chtrSystem::CleanUp();
//	setsSystem::CleanUp();
//	cmmSystem::CleanUp();
//
//	FeaturesLayer::CleanUp();
//	SupportLayer::CleanUp();
//
//	//	close it
//	//
//	deinitialize_objects();
//
//	EffectsLayer::CleanUp();
	ModelLayer::CleanUpGraphics();
//	snSoundManager::DeInitialize();
//	snSoundSystem::DeInitialize();
	inPackage::CleanUp();

//	prtyControlMgr::DeInitialize();
//	prtyCallbackMgr::DeInitialize();

	tasUVAMgr::DeInitialize();
	tasTUVMgr::DeInitialize();

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();

	if (g_hMutexAppRunning != NULL )
	{
		CloseHandle(g_hMutexAppRunning);
		g_hMutexAppRunning = NULL;
	}

	return 0;
}

