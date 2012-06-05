/****************************************************************************\
**	pwxName_TextBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxName_TextBox.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxName_TextBox_Converter::SetValueIntoControl(twcTextBox* i_pActualControl,
											   nameString i_Value)
{
	wxString name_str(i_Value.GetString().c_str(), wxConvUTF8);
	i_pActualControl->SetValue(name_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pwxName_TextBox_Converter::GetValueFromControl(twcTextBox* i_pActualControl)
{
	std::string sel_text = i_pActualControl->GetValue().utf8_str();
	return nameString(sel_text);
}

#endif
