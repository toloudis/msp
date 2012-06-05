/****************************************************************************\
**	pqtInt32_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt32_NumericUpDown.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt32_NumericUpDown_Converter::SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
													 int i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
int pqtInt32_NumericUpDown_Converter::GetValueFromControl(tqcNumericUpDown* i_pActualControl)
{
	return (int) i_pActualControl->GetValue();
}

#endif
