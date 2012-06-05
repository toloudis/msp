/*****************************************************************************
**	pythPythonDialog.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/wxGUI/pythPythonDialog.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif

#include <boost/algorithm/string.hpp>
#include <vector>

#ifdef USE_WXWIDGETS

namespace
{
	std::string l_LogString;
}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
pythPythonDialog* pythPythonDialog::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::pythPythonDialog( wxWindow* parent, 
								const std::string& i_Title )
: pythPythonDialogBase( parent )
{

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::~pythPythonDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing pythPythonDialog()");
	if (pythPythonDialog::Instance == this)
		pythPythonDialog::Instance = NULL;

}

//--------------------------------------------------------------------
// Add string to log textbox
//--------------------------------------------------------------------
void pythPythonDialog::AddToLogTextbox(const std::string& i_String)
{
	this->m_textCtrl_PythLog->AppendText( i_String );
}

//--------------------------------------------------------------------
// Add string to log. If dialog is not created yet, store the
//	string for later display.
//--------------------------------------------------------------------
//static 
void pythPythonDialog::AddToLog(const std::string& i_String)
{
	if (pythPythonDialog::Instance != NULL)
	{
		pythPythonDialog::Instance->AddToLogTextbox(i_String);
	}
	else 
	{
		l_LogString += i_String;
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pythPythonDialog::ExecuteCommand()
{
	std::string command_str = this->m_textCtrl_PythCommand->GetValue();
	boost::trim(command_str);

	std::string log_string = ">> ";
	log_string += boost::replace_all_copy(command_str, "\n", "\n> ");
	log_string += "\n";
	this->AddToLogTextbox(log_string);

	if (pythUtil::ExecuteCommand( command_str ))
	{
		// If successful, clear text
		this->m_textCtrl_PythCommand->SetValue("");
	}
}

//--------------------------------------------------------------------
// event handlers
//--------------------------------------------------------------------
void pythPythonDialog::textCtrl_PythCommand_KeyUp( wxKeyEvent& i_Event )
{ 
	bool bSkip = true;

	 // Handle CTRL-ENTER as "execute script"
	if ( i_Event.GetModifiers() == wxMOD_CONTROL )
	 {
		if (i_Event.GetKeyCode() == WXK_RETURN )
		{
			 bSkip = false;
			 this->ExecuteCommand();
		}
	 }

	i_Event.Skip(bSkip);
}
void pythPythonDialog::button_ExecutePyth_Click( wxCommandEvent& i_Event )
{ 
	this->ExecuteCommand();
}
void pythPythonDialog::button_ClearPyth_Click( wxCommandEvent& i_Event )
{ 
	 this->m_textCtrl_PythCommand->SetValue("");
	 this->m_textCtrl_PythLog->SetValue("");
}

#endif // USE_WXWIDGETS
