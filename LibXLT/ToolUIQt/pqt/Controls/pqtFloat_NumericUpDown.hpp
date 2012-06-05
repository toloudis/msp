/****************************************************************************\
**	pqtFloat_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FLOAT_NUMERICUPDOWN_HPP
#error pqtFloat_NumericUpDown.hpp multiply included
#endif
#define PQT_FLOAT_NUMERICUPDOWN_HPP

#ifndef PQT_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_NumericUpDown.hpp"
#endif
#ifndef PQT_FLOATCONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtFloatConverter.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class pqtFloat_NumericUpDown_Converter : public pqtFloatConverter
{
#ifdef USE_QT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(tqcNumericUpDown* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_NumericUpDown<prtyFloat, float, pqtFloat_NumericUpDown_Converter> pqtFloat_NumericUpDown;

