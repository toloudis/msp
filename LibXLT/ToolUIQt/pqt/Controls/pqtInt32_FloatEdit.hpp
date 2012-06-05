/****************************************************************************\
**	pqtInt32_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT32_FLOATEDIT_HPP
#error pqtInt32_FloatEdit.hpp multiply included
#endif
#define PQT_INT32_FLOATEDIT_HPP

#ifndef PQT_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_FloatEdit.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PQT_INT32CONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtInt32Converter.hpp"
#endif


//============================================================================
//============================================================================
class pqtInt32_FloatEdit_Converter : public pqtInt32Converter
{
#ifdef USE_QT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcFloatEdit* i_pActualControl,
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(tqcFloatEdit* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_FloatEdit<prtyInt32, int, pqtInt32_FloatEdit_Converter> pqtInt32_FloatEdit;

