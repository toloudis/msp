/****************************************************************************\
**	pqtFloat_FloatEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FLOAT_FLOATEDIT_HPP
#error pqtFloat_FloatEdit.hpp multiply included
#endif
#define PQT_FLOAT_FLOATEDIT_HPP

#ifndef PQT_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_FloatEdit.hpp"
#endif
#ifndef PQT_FLOATCONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtFloatConverter.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif


//============================================================================
//============================================================================
class pqtFloat_FloatEdit_Converter : public pqtFloatConverter
{
#ifdef USE_QT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcFloatEdit* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(tqcFloatEdit* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_FloatEdit<prtyFloat, float, pqtFloat_FloatEdit_Converter> pqtFloat_FloatEdit;

