/****************************************************************************\
**	pqtInt32_RangedFloat.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT32_RANGEDFLOAT_HPP
#error pqtInt32_RangedFloat.hpp multiply included
#endif
#define PQT_INT32_RANGEDFLOAT_HPP

#ifndef PQT_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_RangedFloat.hpp"
#endif
#ifndef PQT_INT32CONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtInt32Converter.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtInt32_RangedFloat_Converter  : public pqtInt32Converter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcRangedFloat* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(tqcRangedFloat* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_RangedFloat<prtyInt32, int, pqtInt32_RangedFloat_Converter> pqtInt32_RangedFloat;

