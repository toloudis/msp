/****************************************************************************\
**	pqtInt8_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt8_RangedFloat.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt8_RangedFloat_Converter::SetValueIntoControl(tqcRangedFloat* i_pActualControl,
													 envType::Int8 i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
envType::Int8 pqtInt8_RangedFloat_Converter::GetValueFromControl(tqcRangedFloat* i_pActualControl)
{
	return (envType::Int8) i_pActualControl->GetValue();
}

#endif
