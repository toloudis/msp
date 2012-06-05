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
													 envType::Int8 i_Value)
{
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
envType::Int8 pwxInt8_FloatEdit_Converter::GetValueFromControl(twcFloatEdit* i_pActualControl)
{
	return (envType::Int8) i_pActualControl->GetFloatValue();
}

#endif
