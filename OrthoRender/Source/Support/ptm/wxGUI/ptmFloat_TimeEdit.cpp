/****************************************************************************\
**	pwxFloat_TimeEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (TimeEdit).
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/wxGUI/ptmFloat_TimeEdit.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void ptmFloat_TimeEdit_Converter::SetValueIntoControl(ptmTimeEdit* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetTimeValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float ptmFloat_TimeEdit_Converter::GetValueFromControl(ptmTimeEdit* i_pActualControl)
{
	return i_pActualControl->GetTimeValue();
}

#endif
