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

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtText_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
													 std::string i_Value)
{
	i_pActualControl->SetValue(tqtQString(i_Value));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pqtText_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	std::string str = tqtStdString(i_pActualControl->GetValue());
	return str;
}

#endif
