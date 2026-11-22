/****************************************************************************\
**	pwxInt32_ComboBox.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt32_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#ifndef ENV_STRING_HPP
#include "Core/env/envString.hpp"
#endif

#include <string>
#include <vector>

#ifdef USE_WXWIDGETS

namespace
{
	//	The control's items (its choices, as GetChoices gave them)
	std::vector<std::string> items_of(const wxChoice* i_pControl)
	{
		std::vector<std::string> items;
		for (unsigned i = 0; i < i_pControl->GetCount(); ++i)
			items.push_back(envString::WideCharToUTF8(i_pControl->GetString(i).ToStdWstring()));
		return items;
	}
}

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pwxInt32_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control: the item that shows it (numbered items: the one
// with that number; named items: the one at that index), else none
//----------------------------------------------------------------------------
void pwxInt32_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
													 int i_Value)
{
	const int item = prtyComboBoxUIInfo::ItemOfValue(items_of(i_pActualControl), i_Value);
	i_pActualControl->SetSelection(item >= 0 ? item : wxNOT_FOUND);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pwxInt32_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	return prtyComboBoxUIInfo::ValueOfItem(items_of(i_pActualControl), i_pActualControl->GetSelection());
}

#endif
