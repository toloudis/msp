/*****************************************************************************
**	quiProgressDialog.hpp
**
**	An interface to a progress dialog.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_PROGRESSDIALOG_HPP
#error quiProgressDialog.hpp multiply included
#endif
#define QUI_PROGRESSDIALOG_HPP

#ifndef GUI_PROGRESSDIALOG_HPP
#include "Tool/gui/guiProgressDialog.hpp"
#endif


//============================================================================
//============================================================================
class quiProgressDialog : public guiProgressDialogImpl
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

