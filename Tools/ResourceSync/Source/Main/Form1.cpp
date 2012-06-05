//============================================================================
//============================================================================
#include "stdafx.h"
#include "Form1.h"
#include <windows.h>

#include "Core/gf/gfPaths.hpp"
#include "ToolUIManaged/mui/muiPackage.hpp"


//============================================================================
//============================================================================
using namespace Source;


//============================================================================
//============================================================================
[STAThread] int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	//	initialize packages
	//
	dbgLog::Init();
	gfPackage::Init();

	//tmaSystem::g_pMainForm = this;
	//tmaSystem::g_nNumMenuOffset = 2;
	//muiPackage::Initialize();

	//	setup the app path
	gfPaths::SetAppPath(gfPaths::GetPath(gfPaths::e_ExePath));

	// Initalize Registry here for our document type
	//
	fsLocator mru_file( gfPaths::GetAppPath() );
	mru_file.Push("RecentFiles-ResourceSync.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	//
	// Add ResourceTracker document interest
	docSingleTypeMgr::AddDocumentInterest(new rstkDocumentInterest());

	//	 run!
	//
	//System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;
	Application::Run(gcnew Form1());

	//	clean-up packages
	//
	//muiPackage::DeInitialize();
	gfPackage::CleanUp();
	dbgLog::CleanUp();

	return 0;
}
