/****************************************************************************\
**	pwxFileName_TextBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "pwxFileName_TextBox.hpp"

#include "Core/it/itStringUtil.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFileName_TextBox_Converter::SetValueIntoControl(twcTextBox* i_pActualControl,
														  itString i_Value)
{
	i_pActualControl->SetValue(i_Value.GetString());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
itString pwxFileName_TextBox_Converter::GetValueFromControl(twcTextBox* i_pActualControl)
{
	itString sel_text((const char*)(i_pActualControl->GetValue().c_str()));
	return sel_text;
}

#endif
