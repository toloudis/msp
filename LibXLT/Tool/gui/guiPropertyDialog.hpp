/*****************************************************************************
**	guiPropertyDialog.hpp
**
**		A non-managed interface to the tabbed dialog manager.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef GUI_PROPERTYDIALOG_HPP
#error guiPropertyDialog.hpp multiply included
#endif
#define GUI_PROPERTYDIALOG_HPP

#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif 
#ifndef ENV_ABSTRACTION_HPP
#include "Core/env/envAbstraction.hpp"
#endif


//============================================================================
// forward declaration
//============================================================================
class guiPropertyDialogImpl;


//============================================================================
// static functions define API
//============================================================================
class guiPropertyDialog : public envAbstraction<guiPropertyDialogImpl>
{
public:
	enum ReturnValue
	{
		e_Cancel = 0,
		e_OK = 1
	};

	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	static ReturnValue ShowModal(	const char* i_DialogTitle, 
							const prtyPropertyUIInfoContainer& i_PropertyContainer,
							const char* i_Message = NULL);
};

//============================================================================
// Implementation class, needs to be derived in implementation library
//============================================================================
class guiPropertyDialogImpl
{
public:
	//--------------------------------------------------------------------
	//	Show a modal dialog with the given properties. 
	//	Returns a dialog result, OK or Cancel.
	//--------------------------------------------------------------------
	virtual guiPropertyDialog::ReturnValue ShowModal(	const char* i_DialogTitle, 
														const prtyPropertyUIInfoContainer& i_PropertyContainer,	
														const char* i_Message) = 0;

};


