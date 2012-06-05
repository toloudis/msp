/*****************************************************************************
**  quiProgressDialog.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/qui/quiProgressDialog.hpp"
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#include "Core/dbg/dbgMsg.hpp"



//===========================================================================
//===========================================================================
namespace
{
#ifdef QT_FINISH_PORT
	wxProgressDialog* l_ProgressDialog = NULL;
#endif // USE_QT

}; // anonymous namespace



//===========================================================================
//	quiProgressDialog functions
//===========================================================================

//--------------------------------------------------------------------
//	Show a progress dialog with the given properties. 
//--------------------------------------------------------------------
void quiProgressDialog::Show(const char* i_DialogTitle, 
							 const char* i_Message)
{
#ifdef QT_FINISH_PORT
	l_ProgressDialog = new wxProgressDialog(wxString(i_DialogTitle, wxConvUTF8), 
											wxString(i_Message, wxConvUTF8), 
											100, 
											NULL, 
											wxPD_AUTO_HIDE | wxPD_APP_MODAL | wxPD_CAN_ABORT | wxPD_REMAINING_TIME);
#else // USE_WXWIDGETS
	DBG_ERROR("MessageBox: "<<i_Message);
#endif // USE_QT
}

//--------------------------------------------------------------------
//  Hide - the dialog is going away
//--------------------------------------------------------------------
void quiProgressDialog::Hide()
{
#ifdef QT_FINISH_PORT
	delete l_ProgressDialog;
	l_ProgressDialog = NULL;
#endif // USE_QT
}

//--------------------------------------------------------------------
//	Set the percentage and poll for cancellation. Return false if cancelled.
//--------------------------------------------------------------------
bool quiProgressDialog::SetPercentage( float i_Percentage )
{
#ifdef QT_FINISH_PORT
	if (l_ProgressDialog != NULL)
	{
		return l_ProgressDialog->Update((int)(i_Percentage*100.0f));
	}
#endif // USE_QT
	return true;
}
