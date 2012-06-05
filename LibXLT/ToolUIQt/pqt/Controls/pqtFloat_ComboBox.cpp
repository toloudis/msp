/****************************************************************************\
**	pqtFloat_ComboBox.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFloat_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#include <sstream>

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtFloat_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFloat_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
													 float i_Value)
{
	std::wostringstream str;
	str << i_Value;
	i_pActualControl->SetStringSelection(str.str());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pqtFloat_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	std::wstring text = i_pActualControl->GetStringSelection();
	std::wistringstream str(text);
	float value = 0;
	str >> value;
	return value;
}

#endif
