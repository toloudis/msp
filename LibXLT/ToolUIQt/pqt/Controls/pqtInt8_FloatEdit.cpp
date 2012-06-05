/****************************************************************************\
**	pqtInt8_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtInt8_FloatEdit.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtInt8_FloatEdit_Converter::SetValueIntoControl(tqcFloatEdit* i_pActualControl,
													 envType::Int8 i_Value)
{
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
envType::Int8 pqtInt8_FloatEdit_Converter::GetValueFromControl(tqcFloatEdit* i_pActualControl)
{
	return (envType::Int8) i_pActualControl->GetFloatValue();
}

#endif
