/****************************************************************************\
**	pqtInt8_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT8_RANGEDFLOAT_HPP
#error pqtInt8_RangedFloat.hpp multiply included
#endif
#define PQT_INT8_RANGEDFLOAT_HPP

#ifndef PQT_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_RangedFloat.hpp"
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
class pqtInt8_RangedFloat_Converter  : public pqtInt8Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcRangedFloat* i_pActualControl,
									 int8_t i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int8_t GetValueFromControl(tqcRangedFloat* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_RangedFloat<prtyInt8, int8_t, pqtInt8_RangedFloat_Converter> pqtInt8_RangedFloat;

#endif // USE_QT
