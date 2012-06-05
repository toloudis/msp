/****************************************************************************\
**	pqtFloat_RangedFloat.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (RangedFloat).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FLOAT_RANGEDFLOAT_HPP
#error pqtFloat_RangedFloat.hpp multiply included
#endif
#define PQT_FLOAT_RANGEDFLOAT_HPP

#ifndef PQT_TEMPLATE_RANGEDFLOAT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_RangedFloat.hpp"
#endif
#ifndef PQT_FLOATCONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtFloatConverter.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class pqtFloat_RangedFloat_Converter : public pqtFloatConverter
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcRangedFloat* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(tqcRangedFloat* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_RangedFloat<prtyFloat, float, pqtFloat_RangedFloat_Converter> pqtFloat_RangedFloat;

