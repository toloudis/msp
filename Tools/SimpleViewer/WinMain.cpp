#include "stdafx.h"

// Local Project includes
#include "MainForm.h"
#include "wxMainForm.hpp"
#include "wxRenderCanvas.hpp"

// SourceMan/NonMan and System includes
#include "mnmApp.hpp"
#include "mnmDocument.hpp"

// Tool includes
#include "Tool/doc/docCustomDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"
#include "ToolUIWx/twx/twxAppUtil.hpp"

// Terawatt includes
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "InputDI/in/inPackage.hpp"

// Layers for init and cleanup
#include "Core/CoreLayer.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"

//
//	terawatt functions
//
void initialize_Terawatt_base()
{
	// initialize everything
	//
	CoreLayer::Init();
}

void initialize_Terawatt_postApp()
{
	// initialize everything else
	//
	GraphicsLayer::Init();
	GraphicsDX9Layer::Init();

}

//
void deinitialize_Terawatt()
{
	// clean-up everything
	//

//	AudioLayer::CleanUp();
	GraphicsDX9Layer::CleanUp();
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
	appApplicationPAC::SetHINSTANCE(hInstance);
	
#ifdef USE_WXWIDGETS
	twxAppUtil::SetInstance(hInstance, nCmdShow);

	// Get command line arguments in format that wxWidgets likes
	int argc = 0;
    wxChar **argv = NULL;
	twxAppUtil::ParseCommandLine(argc, argv);

	// Initialize wxWidgets
	if (!twxAppUtil::Init(argc, argv))
	{
		return -1;
	}
#endif // USE_WXWIDGETS

	//gfPackage::Init();
	//mnmPaths::SetupPaths();

#ifdef _MANAGED
	tmaRegistryUtil::Init("SimpleViewer");

	SimpleViewer::MainForm ^ main_form = gcnew SimpleViewer::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );
#else // _MANAGED
		
	#ifdef USE_WXWIDGETS
		// create the main application window
		wxMainForm *main_frame = new wxMainForm(_T("SimpleViewer - wxWidgets"));
		appApplicationPAC::SetHWND( (HWND)(main_frame->GetHandle()) );

		// and show it (the frames, unlike simple controls, are not shown when
		// created initially)
		main_frame->Show(true);
	#else // USE_WXWIDGETS
		appApplication::CreateMainWindow( 800, 600, 100, 100, itString("Simple Viewer") );
	#endif // USE_WXWIDGETS

#endif // _MANAGED

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	mnmApp *pApp = new mnmApp();
	mnmApp::SetActive( false );

	initialize_Terawatt_postApp();

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
		pApp->InitializeRender( (void*)main_frame->GetInitRenderWindow()->GetHandle(),
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
	

	DBG_ASSERT0( pApp->GetWindow() != 0, "No main render window" );

	inPackage::Init();
	//mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick
	
	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();

	GraphicsLayer::InitGraphics();
	GraphicsDX9Layer::InitGraphics();
	//EffectsLayer::Init();
	initialize_objects();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
	docSingleTypeMgr::Init();
	docSingleTypeMgr::SetFilter(".*x", "Model Files");

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( props_dir, dir );
	//DBG_LOG1( "props dir (%s)", dir.c_str() );

	// Initialize systems here
	//cmraSystem::Init(pApp->GetSystem());

	// Make custom document of our type here
	docCustomDocumentMgr::ManageDocument(new mnmDocument());

	//	run the app
	//
#ifdef _MANAGED
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
#endif // _MANAGED

	mnmApp::SetActive( true );

	//
	//	the "run" loop
	//
#ifdef _MANAGED
	Application::Run( main_form );
#else // _MANAGED
	#ifdef USE_WXWIDGETS
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

	mnmApp::SetActive( false );

	// By putting the clean up of the document here
	// it handles multiple ways of exiting, though there
	// might be a way to register this on "Closing()" or 
	// some event like that
	guiCustomDocHandler::Exit();

	// Clean up systems
	//cmraSystem::CleanUp();

	//	close it
	//
	deinitialize_objects();

	//EffectsLayer::CleanUp();
	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();

#ifdef USE_WXWIDGETS
	// clean up wxWidgets
	twxAppUtil::CleanUp();

	twxAppUtil::FreeArgs(argc, argv);
#endif // USE_WXWIDGETS

	return 0;
}
