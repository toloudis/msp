/*****************************************************************************
**  wuiPropertyDialog.hpp
**
**      A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef WUI_PROPERTYDIALOG_HPP
#error wuiPropertyDialog.hpp multiply included
#endif
#define WUI_PROPERTYDIALOG_HPP

#ifndef GUI_PROPERTYDIALOG_HPP
#include "Tool/gui/guiPropertyDialog.hpp"
#endif 

//============================================================================
// forward declaration
//============================================================================
class wuiPropertyDialogImpl;

//============================================================================
// static functions define API
//============================================================================
class wuiPropertyDialog : public guiPropertyDialogImpl
{
public:

	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	virtual guiPropertyDialog::ReturnValue ShowModal(const char* i_DialogTitle, 
													 const prtyPropertyUIInfoContainer& i_PropertyContainer,
													 const char* i_Message);
};


