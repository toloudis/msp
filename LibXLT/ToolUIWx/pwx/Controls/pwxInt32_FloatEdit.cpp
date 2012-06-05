/****************************************************************************\
**	pwxInt32_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt32_FloatEdit.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt32_FloatEdit_Converter::SetValueIntoControl(twcFloatEdit* i_pActualControl,
											int i_Value)
{
	i_pActualControl->SetDecimalPlaces(0);
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pwxInt32_FloatEdit_Converter::GetValueFromControl(twcFloatEdit* i_pActualControl)
{
	return (int)i_pActualControl->GetFloatValue();
}

#endif
