/*****************************************************************************
**  quiPropertyDialog.hpp
**
**      A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef QUI_PROPERTYDIALOG_HPP
#error quiPropertyDialog.hpp multiply included
#endif
#define QUI_PROPERTYDIALOG_HPP

#ifndef GUI_PROPERTYDIALOG_HPP
#include "Tool/gui/guiPropertyDialog.hpp"
#endif 

//============================================================================
// forward declaration
//============================================================================
class quiPropertyDialogImpl;

//============================================================================
// static functions define API
//============================================================================
class quiPropertyDialog : public guiPropertyDialogImpl
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


