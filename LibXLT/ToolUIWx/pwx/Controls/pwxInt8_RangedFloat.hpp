/****************************************************************************\
**	pwxInt8_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT8_RANGEDFLOAT_HPP
#error pwxInt8_RangedFloat.hpp multiply included
#endif
#define PWX_INT8_RANGEDFLOAT_HPP

#ifndef PWX_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_RangedFloat.hpp"
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
class pwxInt8_RangedFloat_Converter  : public pwxInt8Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcRangedFloat* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(twcRangedFloat* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_RangedFloat<prtyInt8, envType::Int8, pwxInt8_RangedFloat_Converter> pwxInt8_RangedFloat;

#endif // USE_WXWIDGETS
