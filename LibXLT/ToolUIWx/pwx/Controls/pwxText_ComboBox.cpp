/****************************************************************************\
**	pwxText_ComboBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxText_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pwxText_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxText_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
													 std::string i_Value)
{
	wxString text_str(i_Value.c_str(), wxConvUTF8);
	i_pActualControl->SetStringSelection(text_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pwxText_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	std::string str = i_pActualControl->GetStringSelection().utf8_str();
	return str;
}

#endif
