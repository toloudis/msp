/****************************************************************************\
**	pqtName_ComboBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtName_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtName_ComboBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyComboBoxUIInfo* pUII = static_cast<prtyComboBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtName_ComboBox_Converter::SetValueIntoControl(wxChoice* i_pActualControl,
													nameString i_Value)
{
	wxString name_str(i_Value.GetString().c_str(), wxConvUTF8);
	if (!i_pActualControl->SetStringSelection(name_str))
		i_pActualControl->SetSelection(wxNOT_FOUND);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_ComboBox_Converter::GetValueFromControl(wxChoice* i_pActualControl)
{
	std::string sel_text = i_pActualControl->GetStringSelection().utf8_str();
	return nameString(sel_text);
}

#endif
