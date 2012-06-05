/*****************************************************************************
**	pythDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/GUI/pythDialogUtil.hpp"

#include "Support/pyth/pythLog.hpp"
#include "Support/pyth/pythUtil.hpp"
#include "Support/pyth/wxGUI/pythPythonDialog.hpp"

#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

#include <iostream>

namespace pythDialogUtil
{
	namespace
	{
		//============================================================================
		// Capturing print and err messages from python
		//============================================================================
		void log_CaptureOutput(const char *i_LogStr)
		{
			std::cout << i_LogStr << std::endl;

#ifdef USE_WXWIDGETS
			pythPythonDialog::AddToLog(itString(i_LogStr));
#endif // USE_WXWIDGETS
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{

#ifdef USE_WXWIDGETS
		// Create dialog
		if (!pythPythonDialog::Instance)
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			pythPythonDialog::Instance = new pythPythonDialog(twxSystem::g_pMainForm);
		}
#endif // USE_WXWIDGETS
	
		// Capture stdout and stderr from python interpretor
		// in order to display in our log textbox
		pythLog::Initialize(log_CaptureOutput, log_CaptureOutput);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{

	}

	//--------------------------------------------------------------------
	// ShowPythonDialog - display the python dialog
	//--------------------------------------------------------------------
	void  ShowPythonDialog()
	{

#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(pythPythonDialog::Instance);
#endif // USE_WXWIDGETS
	}

}	// end of namespace
