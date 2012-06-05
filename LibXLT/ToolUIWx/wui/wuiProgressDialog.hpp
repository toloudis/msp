/*****************************************************************************
**  wuiProgressDialog.hpp
**
**      A non-managed interface to a message box.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef WUI_PROGRESSDIALOG_HPP
#error wuiProgressDialog.hpp multiply included
#endif
#define WUI_PROGRESSDIALOG_HPP

#ifndef GUI_PROGRESSDIALOG_HPP
#include "Tool/gui/guiProgressDialog.hpp"
#endif

//============================================================================
//============================================================================
class wuiProgressDialog : public guiProgressDialogImpl
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

