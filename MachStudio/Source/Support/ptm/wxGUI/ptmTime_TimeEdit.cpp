/****************************************************************************\
**	ptmTime_TimeEdit.hpp
**
**		Intermediate class between the property (prtyTime) and 
**	the control (TimeEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Support/ptm/wxGUI/ptmTime_TimeEdit.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void ptmTime_TimeEdit_Converter::SetValueIntoControl(ptmTimeEdit* i_pActualControl,
											const maTime& i_Value)
{
	i_pActualControl->SetTimeValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
maTime ptmTime_TimeEdit_Converter::GetValueFromControl(ptmTimeEdit* i_pActualControl)
{
	return i_pActualControl->GetTimeValue();
}

#endif
