/****************************************************************************\
**	pwxInt8_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt8_RangedFloat.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt8_RangedFloat_Converter::SetValueIntoControl(twcRangedFloat* i_pActualControl,
													 int8_t i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int8_t pwxInt8_RangedFloat_Converter::GetValueFromControl(twcRangedFloat* i_pActualControl)
{
	return (int8_t) i_pActualControl->GetValue();
}

#endif
