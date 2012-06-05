/****************************************************************************\
**	pqtInt32_NumericUpDown.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (NumericUpDown).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT32_NUMERICUPDOWN_HPP
#error pqtInt32_NumericUpDown.hpp multiply included
#endif
#define PQT_INT32_NUMERICUPDOWN_HPP

#ifndef PQT_TEMPLATE_NUMERICUPDOWN_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_NumericUpDown.hpp"
#endif
#ifndef PQT_INT32CONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtInt32Converter.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtInt32_NumericUpDown_Converter  : public pqtInt32Converter
{
#ifdef USE_QT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcNumericUpDown* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(tqcNumericUpDown* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_NumericUpDown<prtyInt32, int, pqtInt32_NumericUpDown_Converter> pqtInt32_NumericUpDown;

