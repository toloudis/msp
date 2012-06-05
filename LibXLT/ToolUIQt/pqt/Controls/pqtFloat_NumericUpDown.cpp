/****************************************************************************\
**	pqtFloat_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFloat_NumericUpDown.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFloat_NumericUpDown_Converter::SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
											float i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pqtFloat_NumericUpDown_Converter::GetValueFromControl(tqcNumericUpDown* i_pActualControl)
{
	return i_pActualControl->GetValue();
}


#endif
