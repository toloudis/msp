#include "stdafx.h"

// Local Project includes
#include "MainForm.h"
#include "mtrDialogUtil.hpp"

// SourceMan/NonMan and System includes
#include "mtrApp.hpp"
#include "mtrDocument.hpp"
#include "mtrLevel.hpp"

// Tool includes
#include "docCustomDocumentMgr.hpp"
#include "docSingleTypeMgr.hpp"
#include "tmaCustomDocHandler.hpp"
#include "muiRegistryUtil.hpp"
#include "tma3dScreenUtil.hpp"

// Terawatt includes
#include "appApplicationPAC.hpp"
#include "dbgLog.hpp"
#include "gfPaths.hpp"
#include "inPackage.hpp"

// Layers for init and cleanup
#include "AppLayer.hpp"
//#include "AudioLayer.hpp"
#include "BaseLayer.hpp"
#include "GraphicsLayer.hpp"
#include "MathLayer.hpp"
#include "ModelLayer.hpp"



using namespace System::Windows::Forms;



//
//	terawatt functions
//
void initialize_Terawatt_base()
{
	// initialize everything
	//
	BaseLayer::Init();
	AppLayer::Init();
	MathLayer::Init();
}

void initialize_Terawatt_postApp()
{
	// initialize everything else
	//
	GraphicsLayer::Init();
	ModelLayer::Init();
//	AudioLayer::Init();

}

//
void deinitialize_Terawatt()
{
	// clean-up everything
	//

//	AudioLayer::CleanUp();
	ModelLayer::CleanUp();
	GraphicsLayer::CleanUp();
	MathLayer::CleanUp();
	AppLayer::CleanUp();
	BaseLayer::CleanUp();
}


//
// Main entry point
//
int APIENTRY _tWinMain( HINSTANCE hInstance,
						HINSTANCE hPrevInstance,
						LPTSTR    lpCmdLine,
						int       nCmdShow )
{
	appApplicationPAC::SetHINSTANCE(hInstance);

	//gfPackage::Init();
	//mnmPaths::SetupPaths();
	muiRegistryUtil::Init("MatStudio");

	MatStudio::MainForm * main_form = new MatStudio::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	mtrApp *pApp = new mtrApp();
	mtrApp::SetActive( false );

	initialize_Terawatt_postApp();

	//	set-up rendering window
	//
	pApp->InitializeRender( (void*)main_form->GetInitRenderWindow()->Handle,
		(void*)main_form->GetRenderWindow()->Handle,
		main_form->GetRenderWindow()->Width,
		main_form->GetRenderWindow()->Height);
	
	System::Drawing::Rectangle rect = main_form->GetRenderWindow()->get_ClientRectangle();
	tma3dScreenUtil::SetWindowSize( maPoint2d( (float)rect.Left, (float)rect.Top ),
		maPoint2d( (float)rect.get_Width(), (float)rect.get_Height() ) );

	DBG_ASSERT0( pApp->GetWindow() != 0, "No main render window" );

	inPackage::Init();
	//mnmVJoystick::Init();		//this will properly configure the inDeviceMgr's VirtualJoystick

	//snSoundSystem::Initialize();
	//snSoundManager::Initialize();

	ModelLayer::InitGraphics();
	//EffectsLayer::Init();
	mtrLevel::Initialize();
	mtrDialogUtil::Init();
	initialize_objects();

	// Load the templates directory, then, tell MainForm
	// to create menu items
	fsLocator template_dir; // = gfPaths::GetPath(gfPaths::e_ExePath);
	template_dir.Push("Templates");
	template_dir.Push("Models");
	mtrTemplateMgr::Initialize(template_dir);
	main_form->CreateTemplatesMenu();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
	docSingleTypeMgr::Init();
	docSingleTypeMgr::SetFilter(".*x", "Model Files");

	// grab the top item off of the MRU list and use that directory
	// as the default.  If the MRU is empty then set MS dir.
	//
	std::vector<fsLocator>	mru_list;
	docSingleTypeMgr::GetMRUList( mru_list );
	if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
	{
		fsLocator dir = mru_list[0];
		dir.Pop();	// remove the filename
		tmaCustomDocHandler::SetInitialDirectory( dir );
	}
	else
	{
		tmaCustomDocHandler::SetInitialDirectory( gfPaths::GetPath( gfPaths::e_ExePath ) );
	}
	tmaCustomDocHandler::SetInitialDirectoryToBeLast(true);

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( props_dir, dir );
	//DBG_LOG1( "props dir (%s)", dir.c_str() );

	// Initialize systems here
	//cmraSystem::Init(pApp->GetSystem());

	// Make custom document of our type here
	docCustomDocumentMgr::ManageDocument(new mtrDocument());

	//	run the app
	//
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	mtrApp::SetActive( true );

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
	tmaCustomDocHandler::Exit();

	// Clean up systems
	//cmraSystem::CleanUp();

	//	close it
	//
	deinitialize_objects();
	
	mtrTemplateMgr::DeInitialize();
	mtrDialogUtil::CleanUp();
	mtrLevel::DeInitialize();

	//EffectsLayer::CleanUp();
	ModelLayer::CleanUpGraphics();
	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();

	return 0;
}
