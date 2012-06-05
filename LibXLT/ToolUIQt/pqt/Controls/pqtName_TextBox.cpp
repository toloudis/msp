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

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtName_TextBox_Converter::SetValueIntoControl(tqcTextBox* i_pActualControl,
											   nameString i_Value)
{
	wxString name_str(i_Value.GetString().c_str(), wxConvUTF8);
	i_pActualControl->SetValue(name_str);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
nameString pqtName_TextBox_Converter::GetValueFromControl(tqcTextBox* i_pActualControl)
{
	std::string sel_text = i_pActualControl->GetValue().utf8_str();
	return nameString(sel_text);
}

#endif
