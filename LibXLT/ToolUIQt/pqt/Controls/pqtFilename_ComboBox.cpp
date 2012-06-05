/****************************************************************************\
**	pqtFileName_ComboBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_FINISH_PORT

#include "pqtFileName_ComboBox.hpp"

#include "Core/it/itStringUtil.hpp"
#include "Core/prty/prtyComboBoxUIInfo.hpp"

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtFileName_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFileName_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
											   itString i_Value)
{
	i_pActualControl->SetStringSelection(i_Value.GetString());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
itString pqtFileName_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	itString sel_text(i_pActualControl->GetStringSelection().c_str());
	return sel_text;
}

#endif
