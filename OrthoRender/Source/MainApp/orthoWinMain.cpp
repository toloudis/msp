/****************************************************************************\
**	orthoWinMain.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"

// Local Project includes
#include "Features/Requests/Tasks/orthoRequestTaskUtil.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"
#include "Features/Capture/orthoAvatarDataUtil.hpp"
#include "Features/Capture/orthoModeRender.hpp"
#include "Features/FeaturesLayer.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/PythonObject/pytFeature.hpp"
#include "Features/RenderPanels/rpnRenderPane.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "MainApp/Commands/mainCommands.hpp"
#include "MainApp/Data/mainDocumentInterest.hpp"
#include "MainApp/mainConstants.hpp"
#include "MainApp/MainForm.h"
#include "MainApp/mainPython.hpp"
#include "MainApp/mnmApp.hpp"
// #include "Record/rcdSystem.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mnm/mnmVJoystick.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/SupportLayer.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"
#include "Systems/AmbientOcclusion/aoSystem.hpp"
#include "Systems/Billboard/billSystem.hpp"
#include "Systems/Cameras/cmraSystem.hpp"
#include "Systems/Cameras/Object/cmraAvatarDataInterest.hpp"
#include "Systems/Character/chtrSystem.hpp"
#include "Systems/Character/Object/chtrAvatarDataInterest.hpp"
#include "Systems/Common/cmmSystem.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/DirectorsCut/dcutSystem.hpp"
#include "Systems/Environments/envtSystem.hpp"
#include "Systems/Fog/fogSystem.hpp"
#include "Systems/Groups/grupSystem.hpp"
#include "Systems/Layers/lyrsSystem.hpp"
#include "Systems/LightSets/lsetSystem.hpp"
#include "Systems/Particles/prtclSystem.hpp"
#include "Systems/PrjLt/prjltSystem.hpp"
#include "Systems/Props/propSystem.hpp"
#include "Systems/PtLt/ptltSystem.hpp"
#include "Systems/Sets/setsSystem.hpp"
#include "Systems/Storyboards/sbrdSystem.hpp"

// library includes
#include "AudioDS/AudioDSLayer.hpp"
#include "AudioDS/Sn/snExceptionX.hpp"
#include "AudioDS/Sn/snSoundManager.hpp"
#include "AudioDS/Sn/snSoundSystem.hpp"
#include "Core/App/private/appApplicationPAC.hpp"
#include "Core/CoreLayer.hpp"
#include "Core/Dbg/dbgLog.hpp"
#include "Core/dbg/dbgSystemInfo.hpp"
#include "Core/Fs/fsResourceTracker.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/Gf/gfPackage.hpp"
#include "Graphics/G2d/g2dExceptionX.hpp"
#include "Graphics/G3d/g3dExceptionX.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"
#include "InputDI/In/inPackage.hpp"
#include "SecurityMatrix/scrty/scrtyDongleX.hpp"
#include "SecurityMatrix/scrty/scrtyMgr.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiXMLTextReader.hpp"
#include "Tool/gui/guiXMLTextReaderStd.hpp"
#include "Tool/gui/guiXMLTextWriter.hpp"
#include "Tool/gui/guiXMLTextWriterStd.hpp"
#include "Tool/rstk/rstkDocumentInterest.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "ToolUIWx/wui/wuiXMLTextReader.hpp"
#include "ToolUIWx/wui/wuiXMLTextWriter.hpp"
#ifdef _MANAGED
#include "ToolUIManaged/prtym/prtyCallbackMgr.hpp"
#include "ToolUIManaged/prtym/prtyControlFactoryBase.hpp"
#include "ToolUIManaged/prtym/prtyControlFactoryTMC.hpp"
#include "ToolUIManaged/prtym/prtyControlMgr.hpp"
#include "ToolUIManaged/tma/tmaSplashMessage.hpp"
#include "ToolUIManaged/tma/tmaSplashMgr.hpp"
#endif

#undef MessageBox // for guiMessageBox

#include "d3d9.h"


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
//============================================================================
namespace
{
	guiXMLTextReaderStd* l_pXMLTextReaderImpl;
	guiXMLTextWriterStd* l_pXMLTextWriterImpl;
}


//============================================================================
//
//	terawatt functions
//
//============================================================================
void initialize_Terawatt_base()
{
	// initialize everything
	//
	CoreLayer::Init();
	fsResourceTracker::Init();
}

void initialize_Terawatt_postApp()
{
	// initialize everything else
	//
	GraphicsLayer::Init();
	GraphicsDX9Layer::Init();
	AudioDSLayer::Init();

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

	cmaCommandMgr::DeInitialize();

	AudioDSLayer::CleanUp();
	GraphicsDX9Layer::CleanUp();
	GraphicsLayer::CleanUp();
	CoreLayer::CleanUp();
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

	DBG_LOG("Window title(" << lpString << ") filename(" << lpStringF << ")");

	return true;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool app_instance_exists()
{
	bool bAppRunning = false;

	bAppRunning |= app_instance_exists("Global\\OrthoRender.exe");
	//bAppRunning |= app_instance_exists("Global\\OrthoRender-D.exe");
	//bAppRunning |= app_instance_exists("Global\\OrthoRender-Debug Managed.exe");
	//bAppRunning |= app_instance_exists("Global\\OrthoRender-Release Managed.exe");

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
		guiMessageBox::Show("Dongle doesn't exist", "Critical Error");
	}
	catch (scrtyAPIFailedX)
	{
		guiMessageBox::Show("Security API failed", "Critical Error");
	}
	catch (scrtyVersionIncorrectX)
	{
		guiMessageBox::Show("Security version incorrect", "Critical Error");
	}
	catch (scrtyReadFailedX)
	{
		guiMessageBox::Show("Dongle read failed", "Critical Error");
	}
	catch (scrtyWriteFailedX)
	{
		guiMessageBox::Show("Dongle write failed", "Critical Error");
	}
	catch (scrtyEncryptFailedX)
	{
		guiMessageBox::Show("Dongle encrypt failed", "Critical Error");
	}
	catch (scrtyDecryptFailedX)
	{
		guiMessageBox::Show("Dongle decrypt failed", "Critical Error");
	}
	catch (scrtyInvalidUserX)
	{
		guiMessageBox::Show("Dongle invalid user failed", "Critical Error");
	}
	catch (scrtyMemSizeErrorX)
	{
		guiMessageBox::Show("Dongle memory size failed", "Critical Error");
	}
	catch (scrtyPortAddrNotPresentX)
	{
		guiMessageBox::Show("Dongle port address not present failed", "Critical Error");
	}
	catch (scrtyDongleCountExpiredX)
	{
		guiMessageBox::Show("Dongle usage expired failed", "Critical Error");
	}
	catch (scrtyDongleDateExpiredX)
	{
		guiMessageBox::Show("Dongle time usage expired failed", "Critical Error");
	}
	catch (scrtyUnknownX)
	{
		guiMessageBox::Show("Unknown dongle error", "Critical Error");
	}
	catch (...)
	{
		guiMessageBox::Show("Critical unknown dongle error", "Critical Error");
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
			guiMessageBox::Show("Dongle invalid", "Critical Error");
			
		}
	}
	catch (scrtyReadFailedX)
	{
		guiMessageBox::Show("Dongle read failed", "Critical Error");
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
#ifdef D3D_DEBUG_INFO
	D3DPERF_SetOptions( 0 );
#else
	D3DPERF_SetOptions( 1 );
#endif

	//	don't allow MS to launch more than once.
	//
	// NOTE: How does this play out with Fast User Switching? [rjk]
	//if (app_instance_exists())
	//{
	//	HWND hWndOtherInstance;
	//	hWndOtherInstance = FindWindow(appApplicationPAC::GetWindowClassName(), NULL);	// window class, title
	//	if ( hWndOtherInstance != (HWND)NULL )
	//	{
	//		// Application is running in current user's session
	//		if (IsIconic(hWndOtherInstance))
	//			ShowWindow(hWndOtherInstance, SW_RESTORE);
	//		SetForegroundWindow(hWndOtherInstance);
	//	}
	//	else
	//	{
	//		//MessageBox(NULL, TEXT("An instance of this app is running"), l_cWINDOW_TITLE, MB_OK);
	//	}
	//	return false;
	//}

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

//WXGUI
/*
	twxAppUtil::SetInstance(hInstance, nCmdShow);

	// Get command line arguments in format that wxWidgets likes
	//int argc = 0;
    //wxChar **argv = NULL;
	//twxAppUtil::ParseCommandLine(argc, argv);
	// wxWidgets can't handle our /lastfile command line arguments, so don't even 
	// bother given them the correct ones.
	int argc = 1;
    char *argv[2];
	argv[0] = new char[::strlen(mnmConstants::c_EXECUTABLE)+1];
	strcpy(argv[0], mnmConstants::c_EXECUTABLE);
	argv[1] = NULL;

	// Initialize wxWidgets
	if (!twxAppUtil::Init(argc, argv))
	{
		return -1;
	}
*/

	std::string cmdline( lpCmdLine );
	mnmApp::PreParseCommandLine(cmdline);

	//	set-up the XML stuff
	//
	l_pXMLTextReaderImpl = new guiXMLTextReaderStd;
	guiXMLTextReader::SetImplementation(l_pXMLTextReaderImpl);

	l_pXMLTextWriterImpl = new guiXMLTextWriterStd;
	guiXMLTextWriter::SetImplementation(l_pXMLTextWriterImpl);

	//
	gfPackage::Init();
	fsLocator appPath = gfPaths::GetPath(gfPaths::e_ExePath);
	mnmPaths::SetupPaths( appPath );
//	muiRegistryUtil::Init("OrthoRender");

	PrefsMgr::ReadPrefs();

#ifdef _MANAGED
	tmaRegistryUtil::Init(mnmConstants::c_COMPANY, mnmConstants::c_PRODUCT);

	StudioFramework::MainForm ^ main_form = gcnew StudioFramework::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );
#else // _MANAGED

//WXGUI
/*
		// create the main application window
	wxMainForm *main_frame = new wxMainForm(_T(mnmConstants::c_PRODUCT));
	appApplicationPAC::SetHWND( (HWND)(main_frame->GetHandle()) );

	// and show it (the frames, unlike simple controls, are not shown when
	// created initially)
	//main_frame->Show(true);
*/


	// TODO: The width and height values here need to be based on some XML file
	// or some consideration of the command line arguments so that we capture
	// at the correct resolution.
	const int app_width = 256;	//800;
	const int app_height = 256;	//600;
	const int app_x = 100;
	const int app_y = 100;
	appApplication::CreateMainWindow( app_width, app_height, app_x, app_y, itString("OrthoRender") );

	// This line causes the capture mode to use the main application window we just opened
	// instead of opening a new sub-window.
	//orthoModeRender::SetUseApplicationWindow(true);
#endif // _MANAGED

	//SplashScreen::SetStatus("Initializing Library"); System::Threading::Thread::Sleep(1);

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	mnmApp *pApp = new mnmApp();
	mnmApp::SetActive( false );

	//
	initialize_Terawatt_postApp();

	dbgSystemInfo::logSystemInfo();

	//initialize task execution system
	orthoRequestTaskUtil::Initialize();

	//initialize messaging system
	orthoRemoteCommandMgr::Initialize();

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

		main_form->GetRenderPane(0)->SetRenderView(pApp->GetRenderView(0));
#else // _MANAGED

//WXGUI
/*
		//wxSize size = main_frame->GetSize();
		//wxSize client_size = main_frame->GetClientSize();
		wxRect rect = main_frame->GetRenderWindow()->GetRect();
		pApp->InitializeRender( (void*)main_frame->GetRenderWindow()->GetHandle(),
			(void*)main_frame->GetRenderWindow()->GetHandle(),
			rect.GetWidth(), rect.GetHeight());

		tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
			maPoint2d( rect.GetWidth(), rect.GetHeight() ) );

		main_frame->GetRenderWindow()->SetRenderView(pApp->GetRenderView(0));
*/
		pApp->InitializeRender( 0, 0, app_width, app_height );
		tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
										maPoint2d( app_width, app_height ) );
#endif // _MANAGED
	}
	catch (const g3dShaderLoadX& ex)
	{
		std::string message("Failed to load shader: ");
		message += ex.GetShaderName();
		guiMessageBox::Show(message.c_str(), "Critical Error");
	}
	catch( const g2dScreenInitX& )
	{
		guiMessageBox::Show("Failed to initialize renderer. Invalid window.", "Error");
	}
	

#ifdef _MANAGED
	// Create other render views and hook up the render panes to the render views
	for (int i=1; i<4; ++i)
	{
		pApp->CreateRenderView( (void*)main_form->GetRenderPane(i)->Handle );
		main_form->GetRenderPane(i)->SetRenderView(pApp->GetRenderView(i));
	}

	prtyCallbackMgr::Initialize();
	prtyControlMgr::Initialize();
	prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryBase() );
	prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryTMC() );
	prtyControlMgr::AddControlFactory( gcnew ptmControlFactoryTimeline() );
#else
//WXGUI
/*
	// Create other render views and hook up the render panes to the render views
	for (int i=1; i<4; ++i)
	{
		pApp->CreateRenderView( main_frame->GetRenderPane(i)->GetHandle() );
		main_frame->GetRenderPane(i)->SetRenderView(pApp->GetRenderView(i));
	}
//	pwxCallbackMgr::Initialize();
	pwxControlMgr::Initialize();
	pwxControlMgr::AddControlFactory( new pwxControlFactoryBase() );
	pwxControlMgr::AddControlFactory( new pwxControlFactoryCustom() );
	pwxControlMgr::AddControlFactory( new ptmControlFactoryTimeline() );
*/
	pwxControlMgr::Initialize();
#endif // _MANAGED
	DBG_TRACE("Property controls Init complete.");

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
		guiMessageBox::Show("No sound card installed!", "Critical Error");
	}
	snSoundManager::Initialize();

	//SplashScreen::SetStatus("Initializing Effects, Model"); System::Threading::Thread::Sleep(1);

	//
	//	read the render prefs data
	//
	//rndrPrefsMgr::ReadPrefs(rndrPrefsMgr::e_ViewportPrefs);
	//	rndrPrefsData& rdata = rndrPrefsMgr::Data(rndrPrefsMgr::e_ViewportPrefs);
	//	rdata.m_RendererType.SetValue(1);
	//	rdata.m_bMatteMode = false;
	rndrPrefsMgr::ReadPrefs(rndrPrefsMgr::e_ViewportPrefs);
	rndrPrefsMgr::ApplyPrefs(rndrPrefsMgr::e_ViewportPrefs);

	GraphicsLayer::InitGraphics();
	GraphicsDX9Layer::InitGraphics();
	initialize_objects();

	DBG_TRACE("Graphics Init complete.");

	// Initalize here for our document type
	//
	fsLocator mru_file( gfPaths::GetPath( mnmPaths::e_Configs ) );
	mru_file.Push("RecentFiles.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	PrefsData& prefsdata = PrefsMgr::Data();
	docSingleTypeMgr::Init( prefsdata.m_MRUHistory.GetValue() );
	docSingleTypeMgr::SetFilter(".mab", "Scene Files");

	// grab the top item off of the MRU list and use that directory
	// as the default.  If the MRU is empty then set MS dir.
	//
	std::vector<fsLocator>	mru_list;
	docSingleTypeMgr::GetMRUList( mru_list );
	if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
	{
		fsLocator dir = mru_list[0];
		dir.Pop();	// remove the filename
		guiSingleDocHandler::SetInitialDirectory( dir );
	}
	else
	{
		guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
	}
	guiSingleDocHandler::SetInitialDirectoryToBeLast(true);

	//SplashScreen::SetStatus("Initializing Layers"); System::Threading::Thread::Sleep(1);

	//
	//
//	cmaCommandMgr::Initialize();			//Init cmaCommand manager before cmmSystem
	cmmSystem::Init();						// Init common first
	DBG_TRACE("Common System Init complete.");
	SupportLayer::Init(pApp->GetSystem());
	DBG_TRACE("Support Init complete.");
	FeaturesLayer::Init(pApp->GetSystem());
	DBG_TRACE("Features Init complete.");

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
	prtclSystem::Init(app_dir, itString("Effects"));
	billSystem::Init(app_dir, itString("Effects"));
	ptltSystem::Init();
	prjltSystem::Init(app_dir, itString("Effects"));
	cmraSystem::Init(app_dir, itString("Sets")); // could have Cameras dir later?
	dcutSystem::Init(pApp->GetSystem());
	fogSystem::Init();
	aoSystem::Init();
	sbrdSystem::Init(app_dir, itString("Storyboards"));
	envtSystem::Init(app_dir, itString("Effects"));
	grupSystem::Init();	
	// Putting these last makes sure that the lights and objects 
	// are written to file before the light sets and layers
	lsetSystem::Init();	
	lyrsSystem::Init();	

	//	add interest here so don't have to change chtr at all.
	orthoAvatarDataUtil::Initialize();
	orthoAvatarDataUtil::AddInterest( new chtrAvatarDataInterest );
	orthoAvatarDataUtil::AddInterest( new cmraAvatarDataInterest );
#endif

	// After all of the systems have initialized and created the
	// python commands they will need, we can now submit these
	// python commands to the interpretor. No more commands can be
	// created after this point.
	pythModules::SubmitModules();

	// Add ResourceTracker document interest
	//
	docSingleTypeMgr::AddDocumentInterest(new rstkDocumentInterest());

	// Add the "version" document chunk LAST so it gets written at the end.
	std::string exestr;
#ifdef _MANAGED
	tmaManagedStringUtils::ManagedStringToStdString(main_form->GetExecutableVersion(), exestr);
#else
	exestr = mainConstants::mc_ExecutableVersion;
	DBG_LOG("version %s", exestr.c_str());
#endif
	// Use "Prepend" here to make sure that we write this chunk first into the files
	docSingleTypeMgr::PrependDocumentInterest(new mainDocumentInterest(exestr));

	//	set the format for writing the files
	//
	//const int WRITE_FILE_FORMAT = docDocument::eDocBinary;
	const int WRITE_FILE_FORMAT = docDocument::eDocXML;
	//const int WRITE_FILE_FORMAT = docDocument::eDocAll;
	docSingleDocumentMgr::SetWriteFormat(WRITE_FILE_FORMAT);

	// Make NewDocument here after document interests have been registered
	guiSingleDocHandler::New();

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

#if defined(PYTHON_ENABLED)
	// Set up python command line arguments just so Tkinter is happy.
	const int fake_argc = 1;
	char* fake_argv[2] = { "", NULL };
	PySys_SetArgv(fake_argc, fake_argv);

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

	pytFeature::Init();
#endif

	//
	//	parse the command line and configure the app accordingly
	//
	cmdline = lpCmdLine;
	mnmApp::ParseCommandLine(cmdline);

	//	set the app active right before the app runs
	mnmApp::SetActive( true );

	//
	//	the "run" loop
	//
#ifdef _MANAGED
	Application::Run( main_form );
#else // _MANAGED
//WXGUI
	/*
			// Add toolbars should be full of buttons by now,
		// so create the toolbars and set their size
		twxToolbarMgr::RealizeAllToolbars();

		// Read in the preferences
		main_frame->prefs_ReadAndApply();

		// Show the main window
		main_frame->Show(true);
			
		// Update interface
		cmmSystemDialogUtil::UpdateDialog();

		twxAppUtil::Run();

		// Write out the preferences
		main_frame->prefs_UpdateAndWrite();
*/
		pApp->Run();

#endif // _MANAGED
	//
	//

	//PrefsMgr::WritePrefs();
 
	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	guiSingleDocHandler::Exit();
#ifdef _MANAGED
	tmaSplashMgr::CleanUp();
#endif // _MANAGED

#ifndef NO_SYSTEMS
	orthoAvatarDataUtil::DeInitialize();

	//
	PrefsMgr::CleanUp();

	// Clean up systems
	lyrsSystem::CleanUp();
	lsetSystem::CleanUp();
	grupSystem::CleanUp();
	sbrdSystem::CleanUp();
	envtSystem::CleanUp();
//	rcdSystem::CleanUp();
//	skySystem::CleanUp();
	//todSystem::CleanUp();
	aoSystem::CleanUp();
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
	pytFeature::CleanUp();
#endif
	
	cmmSystem::CleanUp();
	cmaCommandMgr::DeInitialize();

	FeaturesLayer::CleanUp();
	SupportLayer::CleanUp();

	docSingleTypeMgr::Close();

	//	close it
	//
	deinitialize_objects();

	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	snSoundManager::DeInitialize();
	snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	security_cleanup();

#ifdef _MANAGED
	prtyControlMgr::DeInitialize();
	prtyCallbackMgr::DeInitialize();
#else
	pwxControlMgr::DeInitialize();
#endif // _MANAGED

	pApp->DeInitializeRender();
	delete pApp;

	//
	gfPackage::CleanUp(); // because we explicitly did an Init() of gf above (makes ref count match)

	//shut down task execution system
	orthoRequestTaskUtil::DeInitialize();

	//shut down messaging system
	orthoRemoteCommandMgr::DeInitialize();

	deinitialize_Terawatt();

	//
	delete l_pXMLTextReaderImpl;
	l_pXMLTextReaderImpl = NULL;
	guiXMLTextReader::SetImplementation(NULL);

	delete l_pXMLTextWriterImpl;
	l_pXMLTextWriterImpl = NULL;
	guiXMLTextWriter::SetImplementation(NULL);

//WXGUI
/*
	// clean up wxWidgets
	wuiPackage::DeInitialize();
	twxAppUtil::CleanUp();

	// Only have to free args if we used the txwAppUtil to parse them above
	//twxAppUtil::FreeArgs(argc, argv);
	delete [] argv[0];
*/

	if (g_hMutexAppRunning != NULL )
	{
		CloseHandle(g_hMutexAppRunning);
		g_hMutexAppRunning = NULL;
	}

	return 0;
}

