/****************************************************************************\
**	pwxName_TextBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_NAME_TEXTBOX_HPP
#error pwxName_TextBox.hpp multiply included
#endif
#define PWX_NAME_TEXTBOX_HPP

#ifndef PWX_TEMPLATE_TEXTBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_TextBox.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxName_TextBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcTextBox* i_pActualControl,
									 nameString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static nameString GetValueFromControl(twcTextBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_TextBox<prtyName, nameString, pwxName_TextBox_Converter> pwxName_TextBox;

#endif // USE_WXWIDGETS
