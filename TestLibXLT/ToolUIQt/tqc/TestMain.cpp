/****************************************************************************\
**	TestMain.cpp
**
**	Main entry point for test app
**
**	Studio GPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "TestApp.hpp"

#include "TestMainWindow.hpp"
#include "ControlsTestMgr.hpp"

#include "Core/fs/fsResourceTracker.hpp"
#include "Core/CoreLayer.hpp"
#include "ToolUIQt/qui/quiPackage.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"
#include "ToolUIQt/pqt/pqtControlFactoryBase.hpp"
#include "ToolUIQt/pqt/pqtControlFactoryCustom.hpp"
#include "ToolUIQt/pqt/pqtControlMgr.hpp"

//#include "ToolUIQt/tqt/tqtWidgets.hpp"


//============================================================================
//============================================================================
int main( int argc, char **argv )
{
	TestApp theApp( argc, argv );
	TestMainWindow theMainWindow;
	tqtSystem::g_pMainForm = &theMainWindow;

	//	initialize systems
	quiPackage::Initialize();

	// does this have to be after the above code
	// because we have to have created an application window first?
	CoreLayer::Init();
	fsResourceTracker::Init(); // should go in the Layer Init function above

	pqtControlMgr::Initialize();
	pqtControlMgr::AddControlFactory( new pqtControlFactoryBase() );
	pqtControlMgr::AddControlFactory( new pqtControlFactoryCustom() );

	ControlsTestMgr::Init();

	//
	theMainWindow.show();

	int result = theApp.exec();

	ControlsTestMgr::CleanUp();
	pqtControlMgr::DeInitialize();

	return result;
}

