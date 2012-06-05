#include <stdio.h>
#include <windows.h>
#include <math.h>

#include "relReferenceTest.hpp"
#include "relRelationshipTest.hpp"

#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envPackage.hpp"

//====================================================================
//====================================================================
int WINAPI
WinMain(HINSTANCE hInstance,      // handle to current instance
		HINSTANCE hPrevInstance,  // handle to previous instance
		LPSTR lpCmdLine,          // command line
		int nCmdShow)             // show state
{
	envPackage::Init();
	dbgPackage::Init();


	//relReferenceTest::RunTest();
	relRelationshipTest::RunTest();

	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}