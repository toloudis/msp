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
														  const std::string& i_Value)
{
	i_pActualControl->SetValue(wxString::FromUTF8(i_Value.c_str()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pwxFileName_TextBox_Converter::GetValueFromControl(twcTextBox* i_pActualControl)
{
	return i_pActualControl->GetValue().utf8_string();
}

#endif
