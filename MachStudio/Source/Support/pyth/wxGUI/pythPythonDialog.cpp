/*****************************************************************************
**	pythPythonDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/pyth/wxGUI/pythPythonDialog.hpp"
#include "Support/pyth/pythUtil.hpp"

#include "Core/Env/envString.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif

#include <boost/algorithm/string.hpp>
#include <vector>


#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
namespace
{
	itString l_LogString;
}	// end of namespace



//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
pythPythonDialog* pythPythonDialog::Instance = NULL;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::pythPythonDialog( wxWindow* parent, 
									const wxString& i_Title )
:	pythPythonDialogBase( parent )
{
	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
pythPythonDialog::~pythPythonDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing pythPythonDialog()");
	if (pythPythonDialog::Instance == this)
		pythPythonDialog::Instance = NULL;

}

//--------------------------------------------------------------------
// Add string to log textbox
//--------------------------------------------------------------------
void pythPythonDialog::AddToLogTextbox(const itString& i_String)
{
	this->m_textCtrl_PythLog->AppendText( i_String.GetString() );
}

//--------------------------------------------------------------------
// Add string to log. If dialog is not created yet, store the
//	string for later display.
//--------------------------------------------------------------------
//static 
void pythPythonDialog::AddToLog(const itString& i_String)
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
	std::wstring command_str = this->m_textCtrl_PythCommand->GetValue();
	boost::trim(command_str);

	std::wstring log_string = L">> ";
	log_string += boost::replace_all_copy(command_str, L"\n", L"\n> ");
	log_string += L"\n";
	this->AddToLogTextbox(itString(log_string.c_str()));

	if (pythUtil::ExecuteCommand( envString::WideCharToUTF8(command_str) ))
	{
		// If successful, clear text
		this->m_textCtrl_PythCommand->SetValue(L"");
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
	 this->m_textCtrl_PythCommand->SetValue(L"");
	 this->m_textCtrl_PythLog->SetValue(L"");
}

#endif // USE_WXWIDGETS
