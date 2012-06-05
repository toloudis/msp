/****************************************************************************\
**	pwxFloat_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFloat_NumericUpDown.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFloat_NumericUpDown_Converter::SetValueIntoControl(twcNumericUpDown* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pwxFloat_NumericUpDown_Converter::GetValueFromControl(twcNumericUpDown* i_pActualControl)
{
	return i_pActualControl->GetValue();
}


#endif
