/****************************************************************************\
**	pwxInt32_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT32_NUMERICUPDOWN_HPP
#error pwxInt32_NumericUpDown.hpp multiply included
#endif
#define PWX_INT32_NUMERICUPDOWN_HPP

#ifndef PWX_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_NumericUpDown.hpp"
#endif
#ifndef PWX_INT32CONVERTER_HPP
#include "ToolUIWx/pwx/Controls/pwxInt32Converter.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxInt32_NumericUpDown_Converter  : public pwxInt32Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcNumericUpDown* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(twcNumericUpDown* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_NumericUpDown<prtyInt32, int, pwxInt32_NumericUpDown_Converter> pwxInt32_NumericUpDown;

#endif // USE_WXWIDGETS
