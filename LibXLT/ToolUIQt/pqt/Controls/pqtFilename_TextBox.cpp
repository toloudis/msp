/****************************************************************************\
**	pqtFileName_TextBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "pqtFileName_TextBox.hpp"

#include "Core/it/itStringUtil.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFileName_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
														  const std::string& i_Value)
{
	i_pActualControl->SetValue(QString::fromUtf8(i_Value.c_str()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pqtFileName_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	return i_pActualControl->GetValue().toStdString();
}

#endif
