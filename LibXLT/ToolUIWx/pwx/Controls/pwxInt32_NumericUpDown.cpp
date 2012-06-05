/****************************************************************************\
**	pwxInt32_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt32_NumericUpDown.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt32_NumericUpDown_Converter::SetValueIntoControl(twcNumericUpDown* i_pActualControl,
													 int i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pwxInt32_NumericUpDown_Converter::GetValueFromControl(twcNumericUpDown* i_pActualControl)
{
	return (int) i_pActualControl->GetValue();
}

#endif
