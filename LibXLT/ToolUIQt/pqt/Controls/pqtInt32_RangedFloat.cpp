/****************************************************************************\
**	pqtInt32_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt32_RangedFloat.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt32_RangedFloat_Converter::SetValueIntoControl(tqcRangedFloat* i_pActualControl,
													 int i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pqtInt32_RangedFloat_Converter::GetValueFromControl(tqcRangedFloat* i_pActualControl)
{
	return (int) i_pActualControl->GetValue();
}

#endif
