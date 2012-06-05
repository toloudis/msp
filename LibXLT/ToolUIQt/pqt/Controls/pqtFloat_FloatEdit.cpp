/****************************************************************************\
**	pqtFloat_FloatEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFloat_FloatEdit.hpp"


#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFloat_FloatEdit_Converter::SetValueIntoControl(tqcFloatEdit* i_pActualControl,
														float i_Value)
{
	i_pActualControl->SetFloatValue(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
float pqtFloat_FloatEdit_Converter::GetValueFromControl(tqcFloatEdit* i_pActualControl)
{
	return i_pActualControl->GetFloatValue();
}

#endif
