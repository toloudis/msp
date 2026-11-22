/****************************************************************************\
**	pqtFileName_ComboBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "pqtFileName_ComboBox.hpp"

#include "Core/prty/prtyComboBoxUIInfo.hpp"

#ifdef USE_QT

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
void pqtFileName_ComboBox_Converter::SetValueIntoControl(QComboBox* i_pActualControl,
											   const std::string& i_Value)
{
	pqtControlUtil::SelectString(i_pActualControl, QString::fromUtf8(i_Value.c_str()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pqtFileName_ComboBox_Converter::GetValueFromControl(QComboBox* i_pActualControl)
{
	return i_pActualControl->currentText().toStdString();
}

#endif
