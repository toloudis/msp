/****************************************************************************\
**	pqtInt8_FloatEdit.hpp
**
**		Intermediate class between the property (prtyInt8) and 
**	the control (FloatEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT8_FLOATEDIT_HPP
#error pqtInt8_FloatEdit.hpp multiply included
#endif
#define PQT_INT8_FLOATEDIT_HPP

#ifndef PQT_TEMPLATE_FLOATEDIT_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_FloatEdit.hpp"
#endif
#ifndef PQT_INT8CONVERTER_HPP
#include "ToolUIQt/pqt/Controls/pqtInt8Converter.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtInt8_FloatEdit_Converter  : public pqtInt8Converter
{
#ifdef USE_QT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcFloatEdit* i_pActualControl,
									 envType::Int8 i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static envType::Int8 GetValueFromControl(tqcFloatEdit* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_FloatEdit<prtyInt8, envType::Int8, pqtInt8_FloatEdit_Converter> pqtInt8_FloatEdit;

