/****************************************************************************\
**	pwxFloat_FloatEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFloat_FloatEdit.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFloat_FloatEdit_Converter::SetValueIntoControl(twcFloatEdit* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pwxFloat_FloatEdit_Converter::GetValueFromControl(twcFloatEdit* i_pActualControl)
{
	return i_pActualControl->GetFloatValue();
}


#endif
