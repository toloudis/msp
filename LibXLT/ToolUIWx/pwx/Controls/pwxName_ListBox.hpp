/****************************************************************************\
**	pwxName_ListBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_NAME_LISTBOX_HPP
#error pwxName_ListBox.hpp multiply included
#endif
#define PWX_NAME_LISTBOX_HPP

#ifndef PWX_TEMPLATE_LISTBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ListBox.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxName_ListBox_Converter 
{
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
};

//============================================================================
//============================================================================
typedef pwxTemplate_ListBox<prtyName, nameString, pwxName_ListBox_Converter> pwxName_ListBox;

#endif // USE_WXWIDGETS
