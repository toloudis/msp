/*****************************************************************************
**  muiPropertyDialog.hpp
**
**      A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef MUI_PROPERTYDIALOG_HPP
#error muiPropertyDialog.hpp multiply included
#endif
#define MUI_PROPERTYDIALOG_HPP

#ifndef GUI_PROPERTYDIALOG_HPP
#include "Tool/gui/guiPropertyDialog.hpp"
#endif 

//============================================================================
// forward declaration
//============================================================================
class muiPropertyDialogImpl;

//============================================================================
// static functions define API
//============================================================================
class muiPropertyDialog : public guiPropertyDialogImpl
{
public:

	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	virtual guiPropertyDialog::ReturnValue ShowModal(const char* i_DialogTitle, 
													const PropertyUIIList& i_List,
													const char* i_Message);
};


