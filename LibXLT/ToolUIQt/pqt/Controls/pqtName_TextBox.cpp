/****************************************************************************\
**	pqtName_TextBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtName_TextBox.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtName_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
											   nameString i_Value)
{
	i_pActualControl->SetValue(tqtQString(i_Value.GetString()));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	std::string sel_text = tqtStdString(i_pActualControl->GetValue());
	return nameString(sel_text);
}

#endif
