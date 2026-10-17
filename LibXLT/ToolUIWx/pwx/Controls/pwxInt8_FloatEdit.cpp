/****************************************************************************\
**	pwxInt8_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxInt8_FloatEdit.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxInt8_FloatEdit_Converter::SetValueIntoControl(twcFloatEdit* i_pActualControl,
													 int8_t i_Value)
{
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int8_t pwxInt8_FloatEdit_Converter::GetValueFromControl(twcFloatEdit* i_pActualControl)
{
	return (int8_t) i_pActualControl->GetFloatValue();
}

#endif
