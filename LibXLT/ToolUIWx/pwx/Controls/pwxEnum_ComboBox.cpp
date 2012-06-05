/****************************************************************************\
**	pwxEnum_ComboBox.hpp
**
**		Intermediate class between the property (prtyEnum) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxEnum_ComboBox.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pwxEnum_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	if (i_pUIInfo->GetNumberOfProperties() > 0)
	{
		prtyEnum* pActualProperty = static_cast<prtyEnum*>(i_pUIInfo->GetProperty(0));
		for (int j = 0; j < pActualProperty->m_EnumTags.size(); ++j)
		{
			o_Choices.push_back( pActualProperty->m_EnumTags[j] );
		}
	}
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxEnum_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
								 short i_Value)
{
	i_pActualControl->SetSelection(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
short pwxEnum_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	return i_pActualControl->GetSelection();
}

#endif
