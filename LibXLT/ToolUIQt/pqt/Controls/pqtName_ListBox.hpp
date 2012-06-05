/****************************************************************************\
**	pqtName_ListBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_NAME_LISTBOX_HPP
#error pqtName_ListBox.hpp multiply included
#endif
#define PQT_NAME_LISTBOX_HPP

#ifndef PQT_TEMPLATE_LISTBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_ListBox.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtName_ListBox_Converter 
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Get list of string choices for combo box
	//----------------------------------------------------------------------------
	static void GetChoices(prtyPropertyUIInfo* i_pUIInfo,
							std::vector<std::string>& o_Choices);

	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(wxListBox* i_pActualControl,
									 nameString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static nameString GetValueFromControl(wxListBox* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_ListBox<prtyName, nameString, pqtName_ListBox_Converter> pqtName_ListBox;

