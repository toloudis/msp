/****************************************************************************\
**	pwxFloat_FloatEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FLOAT_FLOATEDIT_HPP
#error pwxFloat_FloatEdit.hpp multiply included
#endif
#define PWX_FLOAT_FLOATEDIT_HPP

#ifndef PWX_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_FloatEdit.hpp"
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
class pwxFloat_FloatEdit_Converter : public pwxFloatConverter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcFloatEdit* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(twcFloatEdit* i_pActualControl);

};

//============================================================================
//============================================================================
typedef pwxTemplate_FloatEdit<prtyFloat, float, pwxFloat_FloatEdit_Converter> pwxFloat_FloatEdit;

#endif // USE_WXWIDGETS
