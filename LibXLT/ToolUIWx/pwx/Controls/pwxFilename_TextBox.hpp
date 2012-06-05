/****************************************************************************\
**	pwxFileName_TextBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FILENAME_TEXTBOX_HPP
#error pwxFileName_TextBox.hpp multiply included
#endif
#define PWX_FILENAME_TEXTBOX_HPP

#ifndef PWX_TEMPLATE_TEXTBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_TextBox.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxFileName_TextBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcTextBox* i_pActualControl,
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(twcTextBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_TextBox<prtyFileName, itString, pwxFileName_TextBox_Converter> pwxFileName_TextBox;

#endif // USE_WXWIDGETS
