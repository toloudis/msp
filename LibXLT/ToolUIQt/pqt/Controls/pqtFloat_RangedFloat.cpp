/****************************************************************************\
**	pqtFloat_RangedFloat.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFloat_RangedFloat.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFloat_RangedFloat_Converter::SetValueIntoControl(tqcRangedFloat* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pqtFloat_RangedFloat_Converter::GetValueFromControl(tqcRangedFloat* i_pActualControl)
{
	return i_pActualControl->GetValue();
}


#endif
