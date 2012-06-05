/****************************************************************************\
**	pwxInt8_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_INT8_FLOATEDIT_HPP
#error pwxInt8_FloatEdit.hpp multiply included
#endif
#define PWX_INT8_FLOATEDIT_HPP

#ifndef PWX_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_FloatEdit.hpp"
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
class pwxInt8_FloatEdit_Converter  : public pwxInt8Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcFloatEdit* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(twcFloatEdit* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_FloatEdit<prtyInt8, envType::Int8, pwxInt8_FloatEdit_Converter> pwxInt8_FloatEdit;

#endif // USE_WXWIDGETS
