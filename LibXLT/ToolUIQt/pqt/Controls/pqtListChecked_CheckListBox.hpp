/****************************************************************************\
**	pqtListChecked_CheckListBox.hpp
**
**		Intermediate class between the property (prtyListChecked) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_LISTCHECKED_LISTBOX_HPP
#error pqtListChecked_CheckListBox.hpp multiply included
#endif
#define PQT_LISTCHECKED_LISTBOX_HPP

#ifndef PQT_TEMPLATE_CHECKLISTBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_CheckListBox.hpp"
#endif
#ifndef PRTY_LISTCHECKED_HPP
#include "Core/prty/prtyListChecked.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtListChecked_CheckListBox_Converter 
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
	static void SetValueIntoControl(wxCheckListBox* i_pActualControl,
									 const checked_list_type& i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static checked_list_type GetValueFromControl(wxCheckListBox* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_CheckListBox<prtyListChecked, checked_list_type, pqtListChecked_CheckListBox_Converter> pqtListChecked_CheckListBox;

