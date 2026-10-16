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

#ifdef USE_QT

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
void pqtListChecked_CheckListBox_Converter::SetValueIntoControl(QListWidget* i_pActualControl,
											   const checked_list_type& i_Value)
{
	if ((int)i_Value.size() == i_pActualControl->count())
	{
		for (int i=0; i<(int)i_Value.size(); i++)
			i_pActualControl->item(i)->setCheckState(i_Value[i].m_bChecked ? Qt::Checked : Qt::Unchecked);
	}
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
checked_list_type pqtListChecked_CheckListBox_Converter::GetValueFromControl(QListWidget* i_pActualControl)
{
	int num_items = i_pActualControl->count();
	checked_list_type items(num_items);
	for (int i=0; i<num_items; i++)
	{
		items[i].m_Text = tqtStdString(i_pActualControl->item(i)->text());
		items[i].m_bChecked = (i_pActualControl->item(i)->checkState() == Qt::Checked);
	}

	return items;
}

#endif
