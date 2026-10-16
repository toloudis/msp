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
														  itString i_Value)
{
	i_pActualControl->SetValue(QString::fromWCharArray(i_Value.GetString()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
itString pqtFileName_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	itString sel_text(i_pActualControl->GetValue().toStdWString().c_str());
	return sel_text;
}

#endif
