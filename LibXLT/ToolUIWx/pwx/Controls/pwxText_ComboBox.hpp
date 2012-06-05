/****************************************************************************\
**	pwxText_ComboBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEXT_COMBOBOX_HPP
#error pwxText_ComboBox.hpp multiply included
#endif
#define PWX_TEXT_COMBOBOX_HPP

#ifndef PWX_TEMPLATE_COMBOBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxText_ComboBox_Converter 
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
									 std::string i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static std::string GetValueFromControl(wxChoice* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_ComboBox<prtyText, std::string, pwxText_ComboBox_Converter> pwxText_ComboBox;

#endif // USE_WXWIDGETS
