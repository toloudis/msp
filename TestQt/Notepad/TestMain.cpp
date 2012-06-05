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


//============================================================================
//============================================================================
int main( int argc, char **argv )
{
	TestApp theApp( argc, argv );

	TestMainWindow theMainWindow;
	theMainWindow.show();

	return theApp.exec();
}
