#include "stdafx.h"

// Local Project includes
#include "MainForm.h"
#include "chrDialogUtil.hpp"

// SourceMan/NonMan and System includes
#include "nonGUI/chrApp.hpp"
#include "nonGUI/chrDocument.hpp"
#include "nonGUI/chrLevel.hpp"

// Tool includes
#include "Tool/doc/docCustomDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

// Terawatt includes
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/gf/gfPaths.hpp"
#include "InputDI/in/inPackage.hpp"

// Layers for init and cleanup
#include "Core/CoreLayer.hpp"
#include "Graphics/GraphicsLayer.hpp"
#include "GraphicsDX9/GraphicsDX9Layer.hpp"



using namespace System::Windows::Forms;



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
//	AudioLayer::Init();

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


//
// Main entry point
//
[STAThread] int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	//gfPackage::Init();
	//mnmPaths::SetupPaths();
	tmaRegistryUtil::Init("Extra Large Technology", "CharacterStudio");

	CharacterStudio::MainForm ^ main_form = gcnew CharacterStudio::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	chrApp *pApp = new chrApp();
	chrApp::SetActive( false );

	initialize_Terawatt_postApp();

	//	set-up rendering window
	//
	pApp->InitializeRender( (void*)main_form->GetInitRenderWindow()->Handle,
		(void*)main_form->GetRenderWindow()->Handle,
		main_form->GetRenderWindow()->Width,
		main_form->GetRenderWindow()->Height);
	
	System::Drawing::Rectangle rect = main_form->GetRenderWindow()->ClientRectangle;
	tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
		maPoint2d( (float)rect.Width, (float)rect.Height ) );

	DBG_ASSERT0( pApp->GetWindow() != 0, "No main render window" );

	inPackage::Init();
	//mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick

	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();

	GraphicsLayer::InitGraphics();
	GraphicsDX9Layer::InitGraphics();
	//EffectsLayer::Init();
	chrLevel::Initialize();
	chrDialogUtil::Init();
	initialize_objects();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
	fsLocator mru_file( gfPaths::GetPath(gfPaths::e_ExePath) );
	mru_file.Push("Configs");
	mru_file.Push("RecentFiles-Character.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	docSingleTypeMgr::Init();
	//docSingleTypeMgr::SetFilter(".chd", "Character Data Files");
	// dual filter types for data file or new model file
	//docSingleTypeMgr::SetFilter(".chd|.*x", "Character Data Files|Model Files");
	docSingleTypeMgr::SetFilter(".*x*|.chd", "Model Files|Character Data Files");

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( props_dir, dir );
	//DBG_LOG1( "props dir (%s)", dir.c_str() );

	// Initialize systems here
	//cmraSystem::Init(pApp->GetSystem());

	// Make custom document of our type here
	docCustomDocumentMgr::ManageDocument(new chrDocument());

	//	run the app
	//
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	chrApp::SetActive( true );

	//
	//	the "run" loop
	//
	Application::Run( main_form );
	//
	//

	chrApp::SetActive( false );

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
	
	chrDialogUtil::CleanUp();
	chrLevel::DeInitialize();

	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();
	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();

	return 0;
}
