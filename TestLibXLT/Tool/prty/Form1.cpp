#include "stdafx.h"
#include "Form1.h"

#include "Core/dbg/dbgPackage.hpp"

#include <windows.h>


//============================================================================
//============================================================================
using namespace prty;


//============================================================================
//============================================================================
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	System::Threading::Thread::CurrentThread->ApartmentState = System::Threading::ApartmentState::STA;

	dbgPackage::Init();

	System::Windows::Forms::Form ^ pForm = gcnew Form1();

	Application::Run( pForm );

	dbgPackage::CleanUp();

	return 0;
}
