/****************************************************************************\
**	pwxEnum_ComboBox.hpp
**
**		Intermediate class between the property (prtyEnum) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_ENUM_COMBOBOX_HPP
#error pwxEnum_ComboBox.hpp multiply included
#endif
#define PWX_ENUM_COMBOBOX_HPP

#ifndef PWX_TEMPLATE_COMBOBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
class pwxEnum_ComboBox_Converter
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
									 short i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static short GetValueFromControl(wxChoice* i_pActualControl);

};

//============================================================================
//============================================================================
typedef pwxTemplate_ComboBox<prtyEnum, short, pwxEnum_ComboBox_Converter> pwxEnum_ComboBox;

#endif // USE_WXWIDGETS
