/*****************************************************************************
**  wuiProgressDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/wui/wuiProgressDialog.hpp"
#include "ToolUIWx/twx/twxWidgets.hpp"
#include "Core/dbg/dbgMsg.hpp"



//===========================================================================
//===========================================================================
namespace
{
#ifdef USE_WXWIDGETS
	wxProgressDialog* l_ProgressDialog = NULL;
#endif // USE_WXWIDGETS

}; // anonymous namespace



//===========================================================================
//	wuiProgressDialog functions
//===========================================================================

//--------------------------------------------------------------------
//	Show a progress dialog with the given properties. 
//--------------------------------------------------------------------
void wuiProgressDialog::Show(const char* i_DialogTitle, 
							 const char* i_Message)
{
#ifdef USE_WXWIDGETS
	l_ProgressDialog = new wxProgressDialog(wxString(i_DialogTitle, wxConvUTF8), 
											wxString(i_Message, wxConvUTF8), 
											100, 
											NULL, 
											wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
#else // USE_WXWIDGETS
	DBG_ERROR("MessageBox: "<<i_Message);
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Hide - the dialog is going away
//--------------------------------------------------------------------
void wuiProgressDialog::Hide()
{
#ifdef USE_WXWIDGETS
	delete l_ProgressDialog;
	l_ProgressDialog = NULL;
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//	Set the percentage and poll for cancellation. Return false if cancelled.
//--------------------------------------------------------------------
bool wuiProgressDialog::SetPercentage( float i_Percentage )
{
#ifdef USE_WXWIDGETS
	if (l_ProgressDialog != NULL)
	{
		return l_ProgressDialog->Update((int)(i_Percentage*100.0f));
	}
#endif // USE_WXWIDGETS
	return true;
}
