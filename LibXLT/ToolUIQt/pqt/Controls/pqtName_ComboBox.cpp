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

#ifdef USE_QT

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
void pqtName_ComboBox_Converter::SetValueIntoControl(QComboBox* i_pActualControl,
													nameString i_Value)
{
	pqtControlUtil::SelectString(i_pActualControl, tqtQString(i_Value.GetString()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_ComboBox_Converter::GetValueFromControl(QComboBox* i_pActualControl)
{
	std::string sel_text = tqtStdString(i_pActualControl->currentText());
	return nameString(sel_text);
}

#endif
