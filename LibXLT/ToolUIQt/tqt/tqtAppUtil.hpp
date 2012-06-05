/*****************************************************************************
**  tqtAppUtil.hpp
**
**     Help for integrating our WinMain functions with wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef TQT_APPUTIL_HPP
#error tqtAppUtil.hpp multiply included
#endif
#define TQT_APPUTIL_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif


#ifdef QT_FINISH_PORT
//============================================================================
//============================================================================
namespace tqtAppUtil
{
	//--------------------------------------------------------------------
	// Pass in a few variables from WinMain into wxWidgets
	//--------------------------------------------------------------------
	void SetInstance( HINSTANCE i_hInstance,
					  int       i_nCmdShow );

	//--------------------------------------------------------------------
	//	ParseCommandLine fills in the argc and argv variables needed
	//	by wxWidgets setup
	//--------------------------------------------------------------------
	void ParseCommandLine(int &o_Argc, wxChar **&o_Argv);

	//--------------------------------------------------------------------
	// Frees the arguments allocated in ParseCommandLine
	//--------------------------------------------------------------------
	void FreeArgs(int &o_Argc, wxChar **&o_Argv);

	//--------------------------------------------------------------------
	// Initializes wxWidgets, returns true if successful.
	//--------------------------------------------------------------------
	bool Init(int i_Argc, wxChar **i_Argv);

	//--------------------------------------------------------------------
	// Runs the application loop
	//--------------------------------------------------------------------
	void Run();

	//--------------------------------------------------------------------
	// DeInitializes wxWidgets
	//--------------------------------------------------------------------
	void CleanUp();
}

#endif // QT_FINISH_PORT
