/****************************************************************************\
**	pwxInt8_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT8_NUMERICUPDOWN_HPP
#error pwxInt8_NumericUpDown.hpp multiply included
#endif
#define PWX_INT8_NUMERICUPDOWN_HPP

#ifndef PWX_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_NumericUpDown.hpp"
#endif
#ifndef PWX_INT8CONVERTER_HPP
#include "ToolUIWx/pwx/Controls/pwxInt8Converter.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxInt8_NumericUpDown_Converter  : public pwxInt8Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcNumericUpDown* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(twcNumericUpDown* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_NumericUpDown<prtyInt8, envType::Int8, pwxInt8_NumericUpDown_Converter> pwxInt8_NumericUpDown;

#endif // USE_WXWIDGETS
