#include <stdio.h>
#include <windows.h>
#include <ddraw.h>
#include <math.h>

#include "envBoostTest.hpp"
#include "envSharedAssetTest.hpp"
#include "envSTLHelpersTest.hpp"
#include "envThreadTest.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/dbg/dbgPackage.hpp"
#include "Core/env/envError.hpp"
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


	//envSTLHelpersTest::RunTest();
	//envBoostTest::RunTest();
	//envThreadTest::RunTest();
	envSharedAssetTest::RunTest();

	dbgPackage::CleanUp();
	envPackage::CleanUp();
	return 0;
}