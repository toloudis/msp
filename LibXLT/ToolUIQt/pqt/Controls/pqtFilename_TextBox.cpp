/****************************************************************************\
**	pqtFileName_TextBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_FINISH_PORT
#include "pqtFileName_TextBox.hpp"

#include "Core/it/itStringUtil.hpp"



//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFileName_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
														  itString i_Value)
{
	i_pActualControl->SetValue(i_Value.GetString());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
itString pqtFileName_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	itString sel_text(i_pActualControl->GetValue().c_str());
	return sel_text;
}

#endif
