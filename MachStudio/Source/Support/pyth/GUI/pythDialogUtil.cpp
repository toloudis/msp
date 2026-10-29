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
#ifdef MSP_GUI_QT
#include "Support/pyth/qtGUI/pythPythonDialog.hpp"
#else
#include "Support/pyth/wxGUI/pythPythonDialog.hpp"
#endif

#ifdef USE_WXWIDGETS
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#else
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif
#elif defined(USE_QT)
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"
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

#if defined(USE_WXWIDGETS) || defined(USE_QT)
			pythPythonDialog::AddToLog(std::string(i_LogStr));
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
#elif defined(USE_QT)
		// Create dialog
		if (!pythPythonDialog::Instance)
		{
			DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
			pythPythonDialog::Instance = new pythPythonDialog(tqtSystem::g_pMainForm);
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
#elif defined(USE_QT)
		tqtPaneMgr::Show(pythPythonDialog::Instance);
#endif // USE_WXWIDGETS
	}

}	// end of namespace
