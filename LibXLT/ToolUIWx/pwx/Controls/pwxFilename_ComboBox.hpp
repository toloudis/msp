/****************************************************************************\
**	pwxFileName_ComboBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FILENAME_COMBOBOX_HPP
#error pwxFileName_ComboBox.hpp multiply included
#endif
#define PWX_FILENAME_COMBOBOX_HPP

#ifndef PWX_TEMPLATE_COMBOBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxFileName_ComboBox_Converter 
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
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(wxChoice* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_ComboBox<prtyFileName, itString, pwxFileName_ComboBox_Converter> pwxFileName_ComboBox;

#endif // USE_WXWIDGETS
