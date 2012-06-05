/*****************************************************************************
**  muiProgressDialog.hpp
**
**      A non-managed interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_PROGRESSDIALOG_HPP
#error muiProgressDialog.hpp multiply included
#endif
#define MUI_PROGRESSDIALOG_HPP

#ifndef GUI_PROGRESSDIALOG_HPP
#include "Tool/gui/guiProgressDialog.hpp"
#endif

//============================================================================
//============================================================================
class muiProgressDialog : public guiProgressDialogImpl
{
	//--------------------------------------------------------------------
	//	Show a progress dialog with the given properties. 
	//--------------------------------------------------------------------
	virtual void Show(const char* i_DialogTitle, 
						const char* i_Message);

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away
	//--------------------------------------------------------------------
	virtual void Hide();

	//--------------------------------------------------------------------
	//	Set the percentage and poll for cancellation. Return false if cancelled.
	//--------------------------------------------------------------------
	virtual bool SetPercentage( float i_Percentage );
};

