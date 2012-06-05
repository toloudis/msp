/****************************************************************************\
**	pqtInt32_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt32_FloatEdit.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt32_FloatEdit_Converter::SetValueIntoControl(tqcFloatEdit* i_pActualControl,
														int i_Value)
{
	i_pActualControl->SetDecimalPlaces(0);
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pqtInt32_FloatEdit_Converter::GetValueFromControl(tqcFloatEdit* i_pActualControl)
{
	return (int)i_pActualControl->GetFloatValue();
}

#endif
