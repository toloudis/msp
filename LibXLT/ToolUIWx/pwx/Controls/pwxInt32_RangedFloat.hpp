/****************************************************************************\
**	pwxInt32_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT32_RANGEDFLOAT_HPP
#error pwxInt32_RangedFloat.hpp multiply included
#endif
#define PWX_INT32_RANGEDFLOAT_HPP

#ifndef PWX_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_RangedFloat.hpp"
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
class pwxInt32_RangedFloat_Converter  : public pwxInt32Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcRangedFloat* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(twcRangedFloat* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_RangedFloat<prtyInt32, int, pwxInt32_RangedFloat_Converter> pwxInt32_RangedFloat;

#endif // USE_WXWIDGETS
