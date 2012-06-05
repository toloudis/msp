/****************************************************************************\
**	ptmTime_TimeEdit.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (TimeEdit).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PTM_TIME_TIMEEDIT_HPP
#error ptmTime_TimeEdit.hpp multiply included
#endif
#define PTM_TIME_TIMEEDIT_HPP

#ifndef PTM_TEMPLATE_TIMEEDIT_HPP
#include "Support/ptm/wxGUI/ptmTemplate_TimeEdit.hpp"
#endif 
#ifndef PRTY_TIME_HPP
#include "Core/prty/prtyTime.hpp"
#endif

#ifdef USE_WXWIDGETS

//============================================================================
//============================================================================
class ptmTime_TimeEdit_Converter
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(ptmTimeEdit* i_pActualControl,
									 const maTime& i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static maTime GetValueFromControl(ptmTimeEdit* i_pActualControl);
};

//============================================================================
//============================================================================
typedef ptmTemplate_TimeEdit<prtyTime, maTime, ptmTime_TimeEdit_Converter> ptmTime_TimeEdit;

#endif // USE_WXWIDGETS
