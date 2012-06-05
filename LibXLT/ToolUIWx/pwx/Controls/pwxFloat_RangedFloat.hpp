/****************************************************************************\
**	pwxFloat_RangedFloat.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FLOAT_RANGEDFLOAT_HPP
#error pwxFloat_RangedFloat.hpp multiply included
#endif
#define PWX_FLOAT_RANGEDFLOAT_HPP

#ifndef PWX_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_RangedFloat.hpp"
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
class pwxFloat_RangedFloat_Converter : public pwxFloatConverter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcRangedFloat* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(twcRangedFloat* i_pActualControl);

};

//============================================================================
//============================================================================
typedef pwxTemplate_RangedFloat<prtyFloat, float, pwxFloat_RangedFloat_Converter> pwxFloat_RangedFloat;

#endif // USE_WXWIDGETS
