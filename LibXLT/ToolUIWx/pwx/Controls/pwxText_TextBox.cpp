/****************************************************************************\
**	pwxText_TextBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxText_TextBox.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxText_TextBox_Converter::SetValueIntoControl(twcTextBox* i_pActualControl,
													 std::string i_Value)
{
	wxString text_str(i_Value.c_str(), wxConvUTF8);
	i_pActualControl->SetValue(text_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pwxText_TextBox_Converter::GetValueFromControl(twcTextBox* i_pActualControl)
{
	std::string str = i_pActualControl->GetValue().utf8_str();
	return str;
}

#endif
