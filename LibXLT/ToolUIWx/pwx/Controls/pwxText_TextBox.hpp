/****************************************************************************\
**	pwxText_TextBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEXT_TEXTBOX_HPP
#error pwxText_TextBox.hpp multiply included
#endif
#define PWX_TEXT_TEXTBOX_HPP

#ifndef PWX_TEMPLATE_TEXTBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_TextBox.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxText_TextBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcTextBox* i_pActualControl,
									 std::string i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static std::string GetValueFromControl(twcTextBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_TextBox<prtyText, std::string, pwxText_TextBox_Converter> pwxText_TextBox;

#endif // USE_WXWIDGETS
