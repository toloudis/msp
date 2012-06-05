/****************************************************************************\
**	pwxInt32_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt32_RangedFloat.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt32_RangedFloat_Converter::SetValueIntoControl(twcRangedFloat* i_pActualControl,
													 int i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pwxInt32_RangedFloat_Converter::GetValueFromControl(twcRangedFloat* i_pActualControl)
{
	return (int) i_pActualControl->GetValue();
}

#endif
