/*****************************************************************************
**  twxAppUtil.hpp
**
**     Help for integrating our WinMain functions with wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef TWX_APPUTIL_HPP
#error twxAppUtil.hpp multiply included
#endif
#define TWX_APPUTIL_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace twxAppUtil
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

#endif // USE_WXWIDGETS
