/****************************************************************************\
**	pwxInt32_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT32_FLOATEDIT_HPP
#error pwxInt32_FloatEdit.hpp multiply included
#endif
#define PWX_INT32_FLOATEDIT_HPP

#ifndef PWX_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_FloatEdit.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PWX_INT32CONVERTER_HPP
#include "ToolUIWx/pwx/Controls/pwxInt32Converter.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class pwxInt32_FloatEdit_Converter : public pwxInt32Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcFloatEdit* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(twcFloatEdit* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_FloatEdit<prtyInt32, int, pwxInt32_FloatEdit_Converter> pwxInt32_FloatEdit;

#endif // USE_WXWIDGETS
