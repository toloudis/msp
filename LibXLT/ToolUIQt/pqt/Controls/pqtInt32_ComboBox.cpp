/****************************************************************************\
**	pqtInt32_ComboBox.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt32_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#include <string>
#include <vector>

#ifdef USE_QT

namespace
{
	//	The control's items (its choices, as GetChoices gave them)
	std::vector<std::string> items_of(const QComboBox* i_pControl)
	{
		std::vector<std::string> items;
		for (int i = 0; i < i_pControl->count(); ++i)
			items.push_back(tqtStdString(i_pControl->itemText(i)));
		return items;
	}
}

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtInt32_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control: the item that shows it (numbered items: the one
// with that number; named items: the one at that index), else none
//----------------------------------------------------------------------------
void pqtInt32_ComboBox_Converter::SetValueIntoControl(QComboBox* i_pActualControl,
													 int i_Value)
{
	i_pActualControl->setCurrentIndex(prtyComboBoxUIInfo::ItemOfValue(items_of(i_pActualControl), i_Value));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pqtInt32_ComboBox_Converter::GetValueFromControl(QComboBox* i_pActualControl)
{
	return prtyComboBoxUIInfo::ValueOfItem(items_of(i_pActualControl), i_pActualControl->currentIndex());
}

#endif
