/****************************************************************************\
**	pqtListChecked_CheckListBox.hpp
**
**		Intermediate class between the property (prtyListChecked) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtListChecked_CheckListBox.hpp"

#include "Core/prty/prtyListBoxUIInfo.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtListChecked_CheckListBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyListBoxUIInfo* pUII = static_cast<prtyListBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtListChecked_CheckListBox_Converter::SetValueIntoControl(wxCheckListBox* i_pActualControl,
											   const checked_list_type& i_Value)
{
	if (i_Value.size() == i_pActualControl->GetCount())
	{
		for (int i=0; i<i_Value.size(); i++)
			i_pActualControl->Check(i, i_Value[i].m_bChecked);
	}
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
checked_list_type pqtListChecked_CheckListBox_Converter::GetValueFromControl(wxCheckListBox* i_pActualControl)
{
	int num_items = i_pActualControl->GetCount();
	checked_list_type items(num_items);
	for (int i=0; i<num_items; i++)
	{
		items[i].m_Text = i_pActualControl->GetString(i).utf8_str();
		items[i].m_bChecked = i_pActualControl->IsChecked(i);
	}

	return items;
}

#endif
