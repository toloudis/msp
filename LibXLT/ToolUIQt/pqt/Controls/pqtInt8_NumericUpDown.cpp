/****************************************************************************\
**	pqtInt8_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt8_NumericUpDown.hpp"

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt8_NumericUpDown_Converter::SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
													 envType::Int8 i_Value)
{
	i_pActualControl->SetValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
envType::Int8 pqtInt8_NumericUpDown_Converter::GetValueFromControl(tqcNumericUpDown* i_pActualControl)
{
	return (envType::Int8) i_pActualControl->GetValue();
}

#endif
