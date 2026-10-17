/****************************************************************************\
**	pwxInt8_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt8_NumericUpDown.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt8_NumericUpDown_Converter::SetValueIntoControl(twcNumericUpDown* i_pActualControl,
													 int8_t i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int8_t pwxInt8_NumericUpDown_Converter::GetValueFromControl(twcNumericUpDown* i_pActualControl)
{
	return (int8_t) i_pActualControl->GetValue();
}

#endif
