/****************************************************************************\
**	pqtText_TextBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtText_TextBox.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtText_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
													 std::string i_Value)
{
	wxString text_str(i_Value.c_str(), wxConvUTF8);
	i_pActualControl->SetValue(text_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pqtText_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	std::string str = i_pActualControl->GetValue().utf8_str();
	return str;
}

#endif
