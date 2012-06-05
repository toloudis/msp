/****************************************************************************\
**	pwxName_ComboBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_NAME_COMBOBOX_HPP
#error pwxName_ComboBox.hpp multiply included
#endif
#define PWX_NAME_COMBOBOX_HPP

#ifndef PWX_TEMPLATE_COMBOBOX_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxName_ComboBox_Converter 
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
									 nameString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static nameString GetValueFromControl(wxChoice* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_ComboBox<prtyName, nameString, pwxName_ComboBox_Converter> pwxName_ComboBox;

#endif // USE_WXWIDGETS
