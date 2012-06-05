#include "stdafx.h"

// Local Project includes
#include "MainForm.h"
#include "rpnRenderPane.hpp"
#include "mainCommands.hpp"
#include "mainDocumentInterest.hpp"
#include "mainPython.hpp"

// SourceMan/NonMan
#include "lwAnimDataUtil.hpp"
#include "lwSongDataUtil.hpp"
#include "lwModeRender.hpp"
#include "cptrRenderUtil.hpp"
#include "mnmApp.hpp"
#include "mnmPaths.hpp"
#include "mnmVJoystick.hpp"

#include "cmmSystem.hpp"
#include "cmmSystemDialogUtil.hpp"
#include "gfPackage.hpp"
#include "PrefsData.hpp"
#include "PrefsMgr.hpp"
#include "rndrPrefsMgr.hpp"

// System includes
//#define NO_SYSTEMS
#ifndef NO_SYSTEMS
#include "billSystem.hpp"
#include "chtrSystem.hpp"
#include "chtrAnimDataInterest.hpp"
#include "chtrSongDataInterest.hpp"
#include "cmraSystem.hpp"
#include "dcutSystem.hpp"
//#include "dirltSystem.hpp"
#include "envtSystem.hpp"
#include "fogSystem.hpp"
#include "grupSystem.hpp"
#include "lsetSystem.hpp"
#include "lyrsSystem.hpp"
#include "propSystem.hpp"
#include "prtclSystem.hpp"
#include "prjltSystem.hpp"
#include "ptltSystem.hpp"
#include "rcdSystem.hpp"
#include "setsSystem.hpp"
#include "sbrdSystem.hpp"
//#include "skySystem.hpp"
//#include "todSystem.hpp"
#endif

// MachStudio layers 
#include "SupportLayer.hpp"
#include "FeaturesLayer.hpp"

// Tool includes
#include "docSingleDocumentMgr.hpp"
#include "docSingleTypeMgr.hpp"
#include "muiMessageBox.hpp"
#include "muiRegistryUtil.hpp"
#include "prtyCallbackMgr.hpp"
#include "prtyControlFactoryBase.hpp"
#include "prtyControlFactoryTMC.hpp"
#include "prtyControlMgr.hpp"
#include "rstkDocumentInterest.hpp"
#include "scrtyMgr.hpp"
#include "scrtyDongleX.hpp"
#include "tmaSingleDocHandler.hpp"
#include "tmaSplashMessage.hpp"
#include "tmaSplashMgr.hpp"
#include "tmlnTimeLine.hpp"

// Terawatt includes
#include "appApplicationPAC.hpp"
//#include "dayPackage.hpp"
//#include "dbgLog.hpp"
#include "fsResourceTracker.hpp"
#include "g3dExceptionX.hpp"
//#include "gfPaths.hpp"
#include "inPackage.hpp"
#include "snExceptionX.hpp"
#include "snSoundManager.hpp"
#include "snSoundSystem.hpp"

// Layers for init and cleanup
#include "AppLayer.hpp"
#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
#include "EffectsLayer.hpp"
#include "GraphicsLayer.hpp"
#include "MathLayer.hpp"
#include "ModelLayer.hpp"

#include "pythModules.hpp"
#include "pythUtil.hpp"

#undef MessageBox // for muiMessageBox

//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace System::Windows::Forms;
using namespace ConceptCave::WaitCursor;
using namespace TerawattManagedControls;
using namespace SplashNS;
#endif

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
	AudioLayer::Init();

	cptrRenderUtil::Initialize();	// no deinitialize
	tmlnTimeLine::Initialize();
}

//
void deinitialize_Terawatt()
{
	// clean-up everything
	//
	fsResourceTracker::CleanUp();
	tmlnTimeLine::DeInitialize();

	AudioLayer::CleanUp();
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool app_instance_exists()
{
	bool bAppRunning = false;

	bAppRunning |= app_instance_exists("Global\\OrthoRender.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-D.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-Debug Managed.exe");
	//bAppRunning |= app_instance_exists("Global\\MachStudio-Release Managed.exe");

	//EnumWindows((WNDENUMPROC)Report, 0); 

	return bAppRunning;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void security_init()
{
	try
	{
		scrtyMgr::Init();
	}
	catch (scrtyDongleDoesntExistX)
	{
		muiMessageBox::Show("Dongle doesn't exist", "Critical Error");
	}
	catch (scrtyAPIFailedX)
	{
		muiMessageBox::Show("Security API failed", "Critical Error");
	}
	catch (scrtyVersionIncorrectX)
	{
		muiMessageBox::Show("Security version incorrect", "Critical Error");
	}
	catch (scrtyReadFailedX)
	{
		muiMessageBox::Show("Dongle read failed", "Critical Error");
	}
	catch (scrtyWriteFailedX)
	{
		muiMessageBox::Show("Dongle write failed", "Critical Error");
	}
	catch (scrtyEncryptFailedX)
	{
		muiMessageBox::Show("Dongle encrypt failed", "Critical Error");
	}
	catch (scrtyDecryptFailedX)
	{
		muiMessageBox::Show("Dongle decrypt failed", "Critical Error");
	}
	catch (scrtyInvalidUserX)
	{
		muiMessageBox::Show("Dongle invalid user failed", "Critical Error");
	}
	catch (scrtyMemSizeErrorX)
	{
		muiMessageBox::Show("Dongle memory size failed", "Critical Error");
	}
	catch (scrtyPortAddrNotPresentX)
	{
		muiMessageBox::Show("Dongle port address not present failed", "Critical Error");
	}
	catch (scrtyDongleCountExpiredX)
	{
		muiMessageBox::Show("Dongle usage expired failed", "Critical Error");
	}
	catch (scrtyDongleDateExpiredX)
	{
		muiMessageBox::Show("Dongle time usage expired failed", "Critical Error");
	}
	catch (scrtyUnknownX)
	{
		muiMessageBox::Show("Unknown dongle error", "Critical Error");
	}
	catch (...)
	{
		muiMessageBox::Show("Critical unknown dongle error", "Critical Error");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void security_check()
{
	try
	{
		if (!scrtyMgr::DongleValid())
		{
			muiMessageBox::Show("Dongle invalid", "Critical Error");
			
		}
	}
	catch (scrtyReadFailedX)
	{
		muiMessageBox::Show("Dongle read failed", "Critical Error");
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void security_cleanup()
{
	scrtyMgr::CleanUp();
}


//============================================================================
//
// Main entry point
//
//============================================================================
#ifdef _MANAGED
[STAThread] 
#endif
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

	//	Splash
	//SplashScreen::ShowSplashScreen(); 
	//SplashScreen::SetStatus("Initializing Main Form"); System::Threading::Thread::Sleep(1);

#ifdef _MANAGED
	// Add the following line to your Application Startup code somewhere (the earlier the better)
	ApplicationWaitCursor::Cursor = Cursors::WaitCursor;		// You can use an Cursor you like aswell

	// Use the following line (at any point in your App, or just once) to configure the length
	// of time to wait before showing the WaitCursor
	ApplicationWaitCursor::Delay  = TimeSpan(0, 0, 0, 0, 400);
#endif

	//
	appApplicationPAC::SetHINSTANCE(hInstance);

	gfPackage::Init();
	fsLocator appPath = gfPaths::GetPath(gfPaths::e_ExePath);
	mnmPaths::SetupPaths( appPath );
	muiRegistryUtil::Init("OrthoRender");

#ifdef _MANAGED
	StudioFramework::MainForm ^ main_form = gcnew StudioFramework::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );
#else // _MANAGED
	// TODO: The width and height values here need to be based on some XML file
	// or some consideration of the command line arguments so that we capture
	// at the correct resolution.
	const int app_width = 800;
	const int app_height = 600;
	const int app_x = 100;
	const int app_y = 100;
	appApplication::CreateMainWindow( app_width, app_height, app_x, app_y, itString("OrthoRender") );

	// This line causes the capture mode to use the main application window we just opened
	// instead of opening a new sub-window.
	lwModeRender::SetUseApplicationWindow(true);
#endif // _MANAGED

	//SplashScreen::SetStatus("Initializing Library"); System::Threading::Thread::Sleep(1);

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	mnmApp *pApp = new mnmApp();
	mnmApp::SetActive( false );

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
//	System::String* splashImage = tmaManagedStringUtils::LocatorToManagedString( gfPaths::GetPath(mnmPaths::e_ExeArt) );
//	splashImage = String::Concat( splashImage, S"\\Splash.png" );
//	tmaSplashMgr::GetSplashMessage()->SetImageFile( splashImage );
//	tmaSplashMgr::GetSplashMessage()->SetMessageText( "Intializing Mach Studio..." );
//#ifndef _DEBUG	// don't show the splash if in debug mode
//	tmaSplashMgr::GetSplashMessage()->Show();
//#endif

	//SplashScreen::SetStatus("Initializing Windows"); System::Threading::Thread::Sleep(1);

	//	set-up rendering window
	//
	try 
	{
#ifdef _MANAGED
		pApp->InitializeRender( (void*)main_form->GetInitRenderWindow()->Handle,
			(void*)main_form->GetRenderPane(0)->Handle,
			main_form->GetRenderPane(0)->Width,
			main_form->GetRenderPane(0)->Height);

		System::Drawing::Rectangle rect = main_form->GetRenderPane(0)->ClientRectangle;
		tma3dScreenUtil::SetWindowSize( maPoint2d( rect.Left, rect.Top ),
			maPoint2d( rect.Width, rect.Height ) );
#else // _MANAGED
		pApp->InitializeRender( 0, 0, app_width, app_height );
		tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
										maPoint2d( app_width, app_height ) );
#endif // _MANAGED
	}
	catch (const g3dShaderLoadX& ex)
	{
		std::string message("Failed to load shader: ");
		message += ex.GetShaderName();
		muiMessageBox::Show(message.c_str(), "Critical Error");
	}
	

#ifdef _MANAGED
	// Create other render views
	pApp->CreateRenderView( (void*)main_form->GetRenderPane(1)->Handle );
	pApp->CreateRenderView( (void*)main_form->GetRenderPane(2)->Handle );
	pApp->CreateRenderView( (void*)main_form->GetRenderPane(3)->Handle );

	// Hook up the render panes to the render views
	main_form->GetRenderPane(0)->SetRenderView(pApp->GetRenderView(0));
	main_form->GetRenderPane(1)->SetRenderView(pApp->GetRenderView(1));
	main_form->GetRenderPane(2)->SetRenderView(pApp->GetRenderView(2));
	main_form->GetRenderPane(3)->SetRenderView(pApp->GetRenderView(3));

	//main_form->GetRenderPane(0)->SetTextVisible(true);
	//main_form->GetRenderPane(1)->SetTextVisible(true);

	prtyCallbackMgr::Initialize();
	prtyControlMgr::Initialize();
	prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryBase() );
	prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryTMC() );
#endif // _MANAGED

	//SplashScreen::SetStatus("Initializing Sound, Input"); System::Threading::Thread::Sleep(1);

	security_init();
	security_check();

	inPackage::Init();
	mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick

	try
	{
		snSoundSystem::Initialize();
	}
	catch (snSoundSystemCreateFailedX)
	{
		muiMessageBox::Show("No sound card installed!", "Critical Error");
	}
	snSoundManager::Initialize();

	//SplashScreen::SetStatus("Initializing Effects, Model"); System::Threading::Thread::Sleep(1);

	//	read the render prefs data
	//
	rndrPrefsMgr::ReadPrefs(rndrPrefsMgr::e_ViewportPrefs);
	//	commit the render prefs to g3d
	rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);

	ModelLayer::InitGraphics();
	EffectsLayer::Init();
	EffectsLayer::InitGraphics();
	initialize_objects();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
	fsLocator mru_file( gfPaths::GetPath( mnmPaths::e_Configs ) );
	mru_file.Push("RecentFiles.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	PrefsData prefsdata = PrefsMgr::Data();
	docSingleTypeMgr::Init( prefsdata.m_MRUHistory.GetValue() );
	docSingleTypeMgr::SetFilter(".mab", "Mach Files");

	// grab the top item off of the MRU list and use that directory
	// as the default.  If the MRU is empty then set MS dir.
	//
	std::vector<fsLocator>	mru_list;
	docSingleTypeMgr::GetMRUList( mru_list );
	if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
	{
		fsLocator dir = mru_list[0];
		dir.Pop();	// remove the filename
		tmaSingleDocHandler::SetInitialDirectory( dir );
	}
	else
	{
		tmaSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
	}
	tmaSingleDocHandler::SetInitialDirectoryToBeLast(true);

	//SplashScreen::SetStatus("Initializing Layers"); System::Threading::Thread::Sleep(1);

	//
	//
	cmmSystem::Init();						// Init common first
	SupportLayer::Init();
	FeaturesLayer::Init(pApp->GetSystem());

	//	set-up the main menu items
	//
	mainCommands::SetupMenu();

	// Set up main python commands
	mainPython::AddCommands("mach");

	//SplashScreen::SetStatus("Initializing Systems"); System::Threading::Thread::Sleep(1);

	// Initialize MachStudio systems here
	//
	fsLocator app_dir( gfPaths::GetPath(gfPaths::e_AppPath) );

#ifndef NO_SYSTEMS
	setsSystem::Init(app_dir, itString("Sets"));
	propSystem::Init(itString("Props"));
	chtrSystem::Init(app_dir, itString("Characters"));
	//	add interest here so don't have to change chtr at all.
	lwAnimDataUtil::AddInterest( new chtrAnimDataInterest );
	lwSongDataUtil::AddInterest( new chtrSongDataInterest );
	prtclSystem::Init(app_dir, itString("Effects"));
	billSystem::Init(app_dir, itString("Effects"));
	ptltSystem::Init();
	prjltSystem::Init(app_dir, itString("Effects"));
	cmraSystem::Init(app_dir, itString("Sets")); // could have Cameras dir later?
	dcutSystem::Init(pApp->GetSystem());
//	dirltSystem::Init();
	fogSystem::Init();
	//todSystem::Init();
//	skySystem::Init();
	rcdSystem::Init();
	sbrdSystem::Init(app_dir, itString("Storyboards"));
	envtSystem::Init(app_dir, itString("Effects"));
	grupSystem::Init();	
	// Putting these last makes sure that the lights and objects 
	// are written to file before the light sets and layers
	lsetSystem::Init();	
	lyrsSystem::Init();	
#endif

	// After all of the systems have initialized and created the
	// python commands they will need, we can now submit these
	// python commands to the interpretor. No more commands can be
	// created after this point.
	pythModules::SubmitModules();

	// Add ResourceTracker document interest
	docSingleTypeMgr::AddDocumentInterest(new rstkDocumentInterest());

	// Add the "version" document chunk LAST so it gets written at the end.
	std::string exestr;
#ifdef _MANAGED
	tmaManagedStringUtils::ManagedStringToStdString(main_form->GetExecutableVersion(), exestr);
#else
	exestr = "2.9.5.99";
#endif
	docSingleTypeMgr::AddDocumentInterest(new mainDocumentInterest(exestr));

	// Make NewDocument here after document interests have been registered
	tmaSingleDocHandler::New();

	//	Show child dialogs (if flags set)
	//
	//	TODO - this would be better if the main form had a "child window interest"
	//	and it could let all the windows know that this is their chance to show
	//	themselves.
	//
	cmmSystemDialogUtil::ShowInitial();

	//	run the app
	//
#ifdef _MANAGED
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
#endif

	//SplashScreen::SetStatus("Initialization Complete"); System::Threading::Thread::Sleep(1);
    //SplashScreen::CloseForm();
//#ifndef _DEBUG	// don't show the splash if in debug mode
//	m_Splash->ShutDown();
//	//tmaSplashMgr::GetSplashMessage()->Hide();
//#endif

	// Set up python command line arguments just so Tkinter is happy.
	const int argc = 1;
	char* argv[2] = { "", NULL };
	PySys_SetArgv(argc, argv);

	// Add the directory ".\python"  to the python path so that we
	// can source scripts from that directory easily
	fsLocator script_dir = gfPaths::GetPath(gfPaths::e_ExePath);
	script_dir.Push("python");
	std::string script_dir_str;
	fsFileUtil::LocatorToANSIFilename(script_dir, script_dir_str);
	std::string append_path_cmd("sys.path.append('");
	append_path_cmd += script_dir_str;
	append_path_cmd += std::string("')");
	pythUtil::ExecuteCommand(append_path_cmd);

	// Load a python script file at start, can define functions for later use
	// in command shell
	fsLocator script_loc = script_dir;
	script_loc.Push("StartUp.py");
	if (fsFileUtil::FileExists(script_loc))
		pythUtil::ScriptFile( script_loc );
	// The LocalScripts.py file should not be checked into SourceSafe,
	// just allow a user to define his/her own local functions
	fsLocator local_script = script_dir;
	local_script.Push("LocalScripts.py");
	if (fsFileUtil::FileExists(local_script))
		pythUtil::ScriptFile( local_script );

	//
	//	parse the command line and configure the app accordingly
	//
	std::string cmdline( lpCmdLine );
	mnmApp::ParseCommandLine(cmdline);

	//	set the app active right before the app runs
	mnmApp::SetActive( true );

	//
	//	the "run" loop
	//
#ifdef _MANAGED
	Application::Run( main_form );
#else // _MANAGED
	pApp->Run();
#endif // _MANAGED
	//
	//

	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	tmaSingleDocHandler::Exit();
#ifdef _MANAGED
	tmaSplashMgr::CleanUp();
#endif // _MANAGED

#ifndef NO_SYSTEMS
	// Clean up systems
	lyrsSystem::CleanUp();
	lsetSystem::CleanUp();
	grupSystem::CleanUp();
	sbrdSystem::CleanUp();
	envtSystem::CleanUp();
	rcdSystem::CleanUp();
//	skySystem::CleanUp();
	//todSystem::CleanUp();
	fogSystem::CleanUp();
//	dirltSystem::CleanUp();
	dcutSystem::CleanUp();
	cmraSystem::CleanUp();
	prjltSystem::CleanUp();
	ptltSystem::CleanUp();
	billSystem::CleanUp();
	prtclSystem::CleanUp();
	propSystem::CleanUp();
	chtrSystem::CleanUp();
	setsSystem::CleanUp();
#endif
	cmmSystem::CleanUp();

	FeaturesLayer::CleanUp();
	SupportLayer::CleanUp();

	//	close it
	//
	deinitialize_objects();

	EffectsLayer::CleanUpGraphics();
	EffectsLayer::CleanUp();
	ModelLayer::CleanUpGraphics();
	snSoundManager::DeInitialize();
	snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	security_cleanup();

#ifdef _MANAGED
	prtyControlMgr::DeInitialize();
	prtyCallbackMgr::DeInitialize();
#endif // _MANAGED

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

		//		
		//	//	process application startup
		//	DoStartup(args);

		//	//	if the form is still shown...
		//	Splasher.Close();
		//}

		//static void DoStartup(string[] args)
		//{
		//	//	do whatever you need to do
		//	Form1 f = new Form1();
		//	Application.Run(f);
		//}
