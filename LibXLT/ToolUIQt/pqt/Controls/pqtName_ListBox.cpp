/****************************************************************************\
**	pqtName_ListBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ListBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtName_ListBox.hpp"

#include "Core/prty/prtyListBoxUIInfo.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pqtName_ListBox_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
											std::vector<std::string>& o_Choices)
{
	prtyListBoxUIInfo* pUII = static_cast<prtyListBoxUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtName_ListBox_Converter::SetValueIntoControl(wxListBox* i_pActualControl,
													nameString i_Value)
{
	wxString name_str(i_Value.GetString().c_str(), wxConvUTF8);
	i_pActualControl->SetStringSelection(name_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_ListBox_Converter::GetValueFromControl(wxListBox* i_pActualControl)
{
	std::string sel_text = i_pActualControl->GetStringSelection().utf8_str();
	return nameString(sel_text);
}

#endif
