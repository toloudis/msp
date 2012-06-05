/****************************************************************************\
**	pwxFloat_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FLOAT_NUMERICUPDOWN_HPP
#error pwxFloat_NumericUpDown.hpp multiply included
#endif
#define PWX_FLOAT_NUMERICUPDOWN_HPP

#ifndef PWX_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_NumericUpDown.hpp"
#endif
#ifndef PWX_FLOATCONVERTER_HPP
#include "ToolUIWx/pwx/Controls/pwxFloatConverter.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxFloat_NumericUpDown_Converter : public pwxFloatConverter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcNumericUpDown* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(twcNumericUpDown* i_pActualControl);

};

//============================================================================
//============================================================================
typedef pwxTemplate_NumericUpDown<prtyFloat, float, pwxFloat_NumericUpDown_Converter> pwxFloat_NumericUpDown;

#endif // USE_WXWIDGETS
