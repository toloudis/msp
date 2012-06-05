#include "stdafx.h"

// Local Project includes
#include "MainForm.h"
#include "ptclDialogUtil.hpp"
#include "ptclLevel.hpp"
#include "ptclApp.hpp"
#include "ptclDocument.hpp"

// Tool includes
#include "Tool/doc/docCustomDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

// Terawatt includes
#include "Core/app/private/appApplicationPAC.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/gf/gfPackage.hpp"
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

	gfPackage::Init();
	fsLocator appPath = gfPaths::GetPath(gfPaths::e_ExePath);
	gfPaths::SetAppPath( appPath );

	//mnmPaths::SetupPaths();
	tmaRegistryUtil::Init("ParticleStudio");

	ParticleStudio::MainForm ^ main_form = gcnew ParticleStudio::MainForm();
	appApplicationPAC::SetHWND( (HWND)(void*)(main_form->Handle) );

	//
	initialize_Terawatt_base();

	// needed before initialize_render()
	ptclApp *pApp = new ptclApp();
	ptclApp::SetActive( false );

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

	fsLocator mru_file( gfPaths::GetPath( gfPaths::e_AppPath ) );
	mru_file.Push("RecentFiles-Particles.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	ptclDialogUtil::Init();
	ptclLevel::Initialize();

	//	show the splash form
	//Splash::Loader::Close();

	// Initalize Registry here for our document type
	//
	docSingleTypeMgr::Init();
	docSingleTypeMgr::SetFilter(".tpr", "Particle Files");

	//std::string dir;
	//fsFileUtil::LocatorToANSIFilename( props_dir, dir );
	//DBG_LOG1( "props dir (%s)", dir.c_str() );

	// Initialize systems here
	//cmraSystem::Init(pApp->GetSystem());

	// Make custom document of our type here
	docCustomDocumentMgr::ManageDocument(new ptclDocument());

	//	run the app
	//
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	ptclApp::SetActive( true );

	ptclDialogUtil::ShowParticleDialog();

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
	guiCustomDocHandler::Exit();

	ptclDialogUtil::ShowParticleDialog( false );

	// Clean up systems
	//cmraSystem::CleanUp();

	//	close it
	//
	ptclLevel::DeInitialize();

	ptclDialogUtil::CleanUp();
//	ptclLevel::DeInitialize();

	GraphicsDX9Layer::CleanUpGraphics();
	GraphicsLayer::CleanUpGraphics();

	//snSoundManager::DeInitialize();
	//snSoundSystem::DeInitialize();
	inPackage::CleanUp();

	pApp->DeInitializeRender();
	delete pApp;

	//
	deinitialize_Terawatt();

	gfPackage::CleanUp();

	return 0;
}
