#include "stdafx.h"
#include "Form1.h"
#include <windows.h>

#include "plgLibraryMgr.hpp"
#include "plgPlugIn.hpp"

#include "aclass.hpp"

using namespace TestDLLs;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	//	create a plug-in object and set the filename
	plgPlugIn* pPlugIn = new plgPlugIn;
	pPlugIn->SetFileName( "testSystem.dll" );

	//	send it to the library manager so the DLL can be loaded
	plgLibraryMgr::LoadPlugIn( pPlugIn );

	//	call the library initialize functionality
	pPlugIn->LibraryInit(); 

	Application::Run(new Form1());

	//	call the library clea-up functionality
	pPlugIn->LibraryCleanUp(); 

	//	free all the plug-ins
	plgLibraryMgr::FreeAllPlugIns();

	//	test the static lib
	theclass* xkd = new theclass;

	return 0;
}

