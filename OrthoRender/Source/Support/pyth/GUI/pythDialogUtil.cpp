/*****************************************************************************
**	pythDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/GUI/pythDialogUtil.hpp"

#include "Support/pyth/pythLog.hpp"
#include "Support/pyth/GUI/pythScriptDialog.h"
#include "Support/pyth/wxGUI/pythPythonDialog.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

#ifdef _MANAGED
using namespace StudioFramework;
#endif // _MANAGED

namespace pythDialogUtil
{
	namespace
	{
		//============================================================================
		// Capturing print and err messages from python
		//============================================================================
		void log_CaptureOutput(const char *i_LogStr)
		{
#ifdef _MANAGED
			System::String ^pStr = gcnew System::String(i_LogStr);
			pStr = pStr->Replace("\n", "\r\n");
			pythScriptDialog::AddToLog(pStr);
#endif // _MANAGED
#ifdef USE_WXWIDGETS
			pythPythonDialog::AddToLog(i_LogStr);
#endif // USE_WXWIDGETS
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
#ifdef _MANAGED
		// Create dialog
		if (!pythScriptDialog::FormInstance)
		{
			pythScriptDialog::FormInstance = gcnew pythScriptDialog();

			// add form to main form
			tmaSystem::g_pMainForm->AddOwnedForm(pythScriptDialog::FormInstance);
		}
#endif // _MANAGED
#ifdef USE_WXWIDGETS
//WXGUI
/*
		// Create dialog
		if (!pythPythonDialog::Instance)
		{
			DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			pythPythonDialog::Instance = new pythPythonDialog(twxSystem::g_pMainForm);
		}
*/
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
#ifdef _MANAGED
		if (pythScriptDialog::FormInstance != nullptr)
			delete pythScriptDialog::FormInstance;
#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	// ShowPythonDialog - display the python dialog
	//--------------------------------------------------------------------
	void  ShowPythonDialog()
	{
#ifdef _MANAGED
		if (!pythScriptDialog::FormInstance)
		{
			pythScriptDialog::FormInstance = gcnew pythScriptDialog();

			// add form to main form
			tmaSystem::g_pMainForm->AddOwnedForm(pythScriptDialog::FormInstance);
		}

		if (pythScriptDialog::FormInstance->WindowState == FormWindowState::Minimized)
			pythScriptDialog::FormInstance->WindowState = FormWindowState::Normal;
		pythScriptDialog::FormInstance->Show();
#endif // _MANAGED
#ifdef USE_WXWIDGETS
//WXGUI
/*
		twxPaneMgr::Show(pythPythonDialog::Instance);
*/
#endif // USE_WXWIDGETS
	}

}	// end of namespace
