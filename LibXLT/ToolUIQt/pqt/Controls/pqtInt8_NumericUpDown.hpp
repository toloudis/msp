/****************************************************************************\
**	pqtInt8_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT8_NUMERICUPDOWN_HPP
#error pqtInt8_NumericUpDown.hpp multiply included
#endif
#define PQT_INT8_NUMERICUPDOWN_HPP

#ifndef PQT_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_NumericUpDown.hpp"
#endif
#ifndef PQT_INT8CONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtInt8Converter.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtInt8_NumericUpDown_Converter  : public pqtInt8Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(tqcNumericUpDown* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_NumericUpDown<prtyInt8, envType::Int8, pqtInt8_NumericUpDown_Converter> pqtInt8_NumericUpDown;

#endif // USE_QT
