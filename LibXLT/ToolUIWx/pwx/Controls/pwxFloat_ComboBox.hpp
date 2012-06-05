/****************************************************************************\
**	pwxFloat_ComboBox.hpp
**
**		Intermediate class between the property (prtyFloat) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FLOAT_COMBOBOX_HPP
#error pwxFloat_ComboBox.hpp multiply included
#endif
#define PWX_FLOAT_COMBOBOX_HPP

#ifndef PWX_TEMPLATE_COMBOBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxFloat_ComboBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Get list of string choices for combo box
	//----------------------------------------------------------------------------
	static void GetChoices(prtyPropertyUIInfo* i_pUIInfo,
							std::vector<std::string>& o_Choices);

	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(wxChoice* i_pActualControl,
									 float i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static float GetValueFromControl(wxChoice* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_ComboBox<prtyFloat, float, pwxFloat_ComboBox_Converter> pwxFloat_ComboBox;

#endif // USE_WXWIDGETS
