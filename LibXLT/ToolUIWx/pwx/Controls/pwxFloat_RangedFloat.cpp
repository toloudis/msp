/****************************************************************************\
**	pwxFloat_RangedFloat.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFloat_RangedFloat.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFloat_RangedFloat_Converter::SetValueIntoControl(twcRangedFloat* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pwxFloat_RangedFloat_Converter::GetValueFromControl(twcRangedFloat* i_pActualControl)
{
	return i_pActualControl->GetValue();
}


#endif
