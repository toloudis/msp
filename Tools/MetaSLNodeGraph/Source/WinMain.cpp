// Local Project includes
//#include "MainForm.h"
#include "wxMainForm.hpp"
#include "wxRenderCanvas.hpp"
#include "wxLayoutMgr.hpp"

// SourceMan/NonMan and System includes
#include "mspApp.hpp"
#include "mspCommands.hpp"
#include "mspDocument.hpp"
#include "mspModel.hpp"
#include "mspVersion.hpp"
#include "msl/mslMetaSLMgr.hpp"
#include "DebugConsole/dbgConsole.hpp"

// Tool includes
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docCustomDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "ToolUIWx/twx/twxAppUtil.hpp"
#include "ToolUIWx/wui/wuiPackage.hpp"

// Terawatt includes
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/Env/envString.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/Gf/gfPackage.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Input/In/inPackage.hpp"

// Layers for init and cleanup
#include "Core/CoreLayer.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX11/GraphicsDX11Layer.hpp"
#include "ImportExport/ImportExportLayer.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"

#define LOAD_SHADERBALL

//
//	terawatt functions
//
void initialize_Terawatt_base()
{
	// initialize everything
	//
	CoreLayer::Init();
}

void initialize_Terawatt_postApp(wxMainForm* main_frame)
{
	// initialize everything else
	//
	GraphicsLayer::Init();
	GraphicsDX11Layer::Init(main_frame->GetRenderWindow()->GetHandle());
	GraphicsLayer::InitGraphics(GraphicsDX11Layer::GetSystem2D(), GraphicsDX11Layer::GetSystem3D());
	GraphicsDX11Layer::InitGraphics();

	// For Autodesk FBX SDK importing...
	ImportExportLayer::Init();
}

//
void deinitialize_Terawatt()
{
	// clean-up everything
	//

	cmaCommandMgr::DeInitialize();

	ImportExportLayer::CleanUp();

	GraphicsDX11Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	GraphicsDX11Layer::CleanUp();
	GraphicsLayer::CleanUp();

	CoreLayer::CleanUp();
}


//====================================================================
// Main entry point
//====================================================================
#ifdef _MANAGED
[STAThread] 
#endif // _MANAGED
int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	// In Debug wxWidgets non-managed version, the CRT will print out messages when it 
	// detects memory leaks. If the report is repeatable (same id number), then you can
	// put the id code number into the call to _CrtSetBreakAlloc and then the debugger
	// will stop at the line when the memory that is never freed is allocated.
	// This sample report:
	//{55870} normal block at 0x043982A8, 1792 bytes long.
	// would be debugged with this line:
	//	_CrtSetBreakAlloc(55870); 
//	_CrtSetBreakAlloc(1831);

	// debug log needs a gfPath now
	gfPackage::Init();

	//
	initialize_Terawatt_base();

	//DBG_LOG1("Command line: %s", lpCmdLine);

	appApplicationPAC::SetHINSTANCE(hInstance);
	
#ifdef USE_WXWIDGETS
	twxAppUtil::SetInstance(hInstance, nCmdShow);

	// Get command line arguments in format that wxWidgets likes
	//int argc = 0;
 //   wxChar **argv = NULL;
	//twxAppUtil::ParseCommandLine(argc, argv);
	// wxWidgets can't handle our command line arguments, so don't even 
	// bother given them the correct ones.
	int argc = 1;
    TCHAR *argv[2];
	argv[0] = L"ModelInspector.exe";
	argv[1] = NULL;

	// Initialize wxWidgets
	if (!twxAppUtil::Init(argc, argv))
	{
		return -1;
	}
#endif // USE_WXWIDGETS


#ifdef _MANAGED
	tmaRegistryUtil::Init("SimpleViewer");

	SimpleViewer::MainForm ^ main_form = gcnew SimpleViewer::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );
#else // _MANAGED
		
	#ifdef USE_WXWIDGETS
		// create the main application window
		wxMainForm *main_frame = new wxMainForm(_T(c_AssemblyTitle));
		appApplicationPAC::SetHWND( (HWND)(main_frame->GetHandle()) );

		// and show it (the frames, unlike simple controls, are not shown when
		// created initially)
		main_frame->Show(true);
	#else // USE_WXWIDGETS
		appApplication::CreateMainWindow( 800, 600, 100, 100, itString(c_AssemblyTitle) );
	#endif // USE_WXWIDGETS

#endif // _MANAGED

	// Find MetaSL Library Data - searching for node graph data in multiple places
	fsLocator metasl_data_dir = gfPaths::GetPath(gfPaths::e_ExePath);
	// 1 - ./MSLData
	metasl_data_dir.Push("MSLData");
	if (!fsFileUtil::DirectoryExists(metasl_data_dir))
	{
		// 2 - ../MSLData
		metasl_data_dir = gfPaths::GetPath(gfPaths::e_ExePath);
		if (metasl_data_dir.GetNumNames() >= 1)
		{
			metasl_data_dir.Pop();
		}
		metasl_data_dir.Push("MSLData");
		if (!fsFileUtil::DirectoryExists(metasl_data_dir))
		{
			// 3 - ./Data
			metasl_data_dir = gfPaths::GetPath(gfPaths::e_ExePath);
			metasl_data_dir.Push("Data");
		}
	}

	// needed before initialize_render()
	mspApp *pApp = new mspApp(metasl_data_dir);
	mspApp::SetActive( false );

	initialize_Terawatt_postApp(main_frame);

	//	set-up rendering window
	//
#ifdef _MANAGED
	pApp->InitializeRender( (void*)main_form->GetInitRenderWindow()->Handle,
		(void*)main_form->GetRenderWindow()->Handle,
		main_form->GetRenderWindow()->Width,
		main_form->GetRenderWindow()->Height);

	System::Drawing::Rectangle rect = main_form->GetRenderWindow()->ClientRectangle;
	tma3dScreenUtil::SetWindowSize( maPoint2d( rect.Left, rect.Top ),
		maPoint2d( rect.Width, rect.Height ) );
#else // _MANAGED

	#ifdef USE_WXWIDGETS
		wxSize size = main_frame->GetSize();
		wxSize client_size = main_frame->GetClientSize();
		wxRect rect = main_frame->GetRenderWindow()->GetRect();
		pApp->InitializeRender( (void*)main_frame->GetRenderWindow()->GetHandle(), // no longer init window
			(void*)main_frame->GetRenderWindow()->GetHandle(),
			rect.GetWidth(), rect.GetHeight());

		tma3dScreenUtil::SetWindowSize( maPoint2d( rect.GetLeft(), rect.GetTop() ),
			maPoint2d( rect.GetWidth(), rect.GetHeight() ) );
	#else // USE_WXWIDGETS
		pApp->InitializeRender( 0, 0, 800, 600 );
		tma3dScreenUtil::SetWindowSize( maPoint2d( 0, 0 ),
										maPoint2d( 800, 600 ) );
	#endif // USE_WXWIDGETS
#endif // _MANAGED
	

	DBG_ASSERT( pApp->GetWindow() != 0, "No main render window" );

	inPackage::Init();
	//mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick
	
	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();

	//EffectsLayer::Init();
	initialize_objects();

	// Initialize MetaSL Library - use directory found above
	mslMetaSLMgr::Initialize(metasl_data_dir);

	//	set-up the main menu items
	//
	mspCommands::SetupMenu();
	dbgConsole::AddToMenu();


	//	show the splash form
	//Splash::Loader::Close();

	// Set up the MRU file location
	//fsLocator mru_file( gfPaths::GetPath(gfPaths::e_ExePath) );
	fsLocator mru_file( gfPaths::GetPath(gfPaths::e_UserDataPath) );
	mru_file.Push("Configs");
	mru_file.Push("RecentFiles-NodeGraph.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	// Initalize Registry here for our document type
	//
	docSingleTypeMgr::Init();
	//docSingleTypeMgr::SetFilter(".*x*", "Model Files");
	docSingleTypeMgr::SetFilter(".xmsl", "MetaSL Graphs");

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( props_dir, dir );
	//DBG_LOG1( "props dir (%s)", dir.c_str() );

	// Initialize systems here
	//cmraSystem::Init(pApp->GetSystem());

	// Make custom document of our type here
	docCustomDocumentMgr::ManageDocument(new mspDocument());

#ifdef LOAD_SHADERBALL
	// Start the program off with the shader ball
	fsLocator shader_ball_loc = metasl_data_dir;
	shader_ball_loc.Push("ShaderBall.gxb");
	if (fsFileUtil::FileExists(shader_ball_loc))
	{
		//bga - This e_MabPath should not exist..
		gfPaths::SetPath(gfPaths::e_MabPath, shader_ball_loc);

		// Switching the "document" to the shader node graph and fix the model to the shader ball,
		// so, directly call the mspModel function instead of  guiCustomDocHandler.
		mspModel::LoadModel(shader_ball_loc);
	//	guiCustomDocHandler::Open(shader_ball_loc);
		
		// A good camera location for viewing the shader ball
		maPoint3d camPos(58.311f, 115.497f, 84.529f);
		maPoint3d camTarg(1.524f, 75.954f, 1.806f);
		cam3dMgr::SetManipPositionAndTarget(camPos, camTarg);
		cam3dMgr::GetEditorCamera().SetFOV( 65 );
	}
#endif

	//	parse the command line and configure the app accordingly
	//
	//bga - This is not good...
	std::string cmdline( envString::WideCharToUTF8(lpCmdLine) );
	mspApp::ParseCommandLine(cmdline);

	//	run the app
	//
#ifdef _MANAGED
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
#endif // _MANAGED

	mspApp::SetActive( true );

	//
	//	the "run" loop
	//
#ifdef _MANAGED
	Application::Run( main_form );
#else // _MANAGED
	#ifdef USE_WXWIDGETS
		// get last layout of panes
		wxLayoutMgr::LoadLastLayout();

		twxAppUtil::Run();
	#else // USE_WXWIDGETS
	
		// Put in temporary code here to set up scene since we don't have a 
		// menu here yet...
		//std::string filename("E:\\Test-Data\\Data\\STOCK\\Props\\General\\Models\\YAguitar2.mhx");
		std::string filename("C:\\Projects\\PTE01\\Data\\Stock\\Characters\\Dawn\\Models\\Dawn_Cloak_MAT_CURRENT.chx");
		fsLocator test_loc;
		fsFileUtil::ANSIFilenameToLocator(filename, test_loc);
		guiCustomDocHandler::Open(test_loc);
		// end temp code

		pApp->Run();
	#endif // USE_WXWIDGETS
#endif // _MANAGED
	//
	//

	mspApp::SetActive( false );

	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	guiCustomDocHandler::Exit();

	// Release the shader ball
	mspModel::Clear();

	// Clean up systems
	//cmraSystem::CleanUp();

	//	close it
	//
	deinitialize_objects();
	
	// DeInitialize MetaSL Library
	mslMetaSLMgr::DeInitialize();

	//EffectsLayer::CleanUp();
	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
	inPackage::CleanUp();

#ifdef _MANAGED
	prtyControlMgr::DeInitialize();
	prtyCallbackMgr::DeInitialize();
#endif // _MANAGED
#ifdef USE_WXWIDGETS
//	pwxCallbackMgr::DeInitialize();
	pwxControlMgr::DeInitialize();
#endif // USE_WXWIDGETS

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();	
	gfPackage::CleanUp(); // because we explicitly did an Init() of gf above (makes ref count match)


#ifdef USE_WXWIDGETS
	// clean up wxWidgets
	wuiPackage::DeInitialize();
	twxAppUtil::CleanUp();

	// Only have to free args if we used the txwAppUtil to parse them above
	//twxAppUtil::FreeArgs(argc, argv);
#endif // USE_WXWIDGETS

	return 0;
}
