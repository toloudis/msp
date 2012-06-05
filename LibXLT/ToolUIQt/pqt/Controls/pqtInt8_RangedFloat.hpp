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


//===================================================================================
//===================================================================================
class pqtInt8_RangedFloat_Converter  : public pqtInt8Converter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcRangedFloat* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(tqcRangedFloat* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_RangedFloat<prtyInt8, envType::Int8, pqtInt8_RangedFloat_Converter> pqtInt8_RangedFloat;

