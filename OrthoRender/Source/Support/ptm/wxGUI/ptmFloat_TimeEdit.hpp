/****************************************************************************\
**	ptmFloat_TimeEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (TimeEdit).
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_FLOAT_TIMEEDIT_HPP
#error ptmFloat_TimeEdit.hpp multiply included
#endif
#define PTM_FLOAT_TIMEEDIT_HPP

#ifndef PTM_TEMPLATE_TIMEEDIT_HPP
#include "Support/ptm/wxGUI/ptmTemplate_TimeEdit.hpp"
#endif 
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class ptmFloat_TimeEdit_Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(ptmTimeEdit* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(ptmTimeEdit* i_pActualControl);
};

//============================================================================
//============================================================================
typedef ptmTemplate_TimeEdit<prtyFloat, float, ptmFloat_TimeEdit_Converter> ptmFloat_TimeEdit;

#endif // USE_WXWIDGETS
