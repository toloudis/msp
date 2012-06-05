/****************************************************************************\
**	pwxListChecked_CheckListBox.hpp
**
**		Intermediate class between the property (prtyListChecked) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_LISTCHECKED_LISTBOX_HPP
#error pwxListChecked_CheckListBox.hpp multiply included
#endif
#define PWX_LISTCHECKED_LISTBOX_HPP

#ifndef PWX_TEMPLATE_CHECKLISTBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_CheckListBox.hpp"
#endif
#ifndef PRTY_LISTCHECKED_HPP
#include "Core/prty/prtyListChecked.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxListChecked_CheckListBox_Converter 
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
	static void SetValueIntoControl(wxCheckListBox* i_pActualControl,
									 const checked_list_type& i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static checked_list_type GetValueFromControl(wxCheckListBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_CheckListBox<prtyListChecked, checked_list_type, pwxListChecked_CheckListBox_Converter> pwxListChecked_CheckListBox;

#endif // USE_WXWIDGETS
