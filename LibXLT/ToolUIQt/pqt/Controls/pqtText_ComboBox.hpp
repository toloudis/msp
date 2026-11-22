/****************************************************************************\
**	pqtText_ComboBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEXT_COMBOBOX_HPP
#error pqtText_ComboBox.hpp multiply included
#endif
#define PQT_TEXT_COMBOBOX_HPP

#ifndef PQT_TEMPLATE_COMBOBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtText_ComboBox_Converter 
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
	static void SetValueIntoControl(QComboBox* i_pActualControl,
									 std::string i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static std::string GetValueFromControl(QComboBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_ComboBox<prtyText, std::string, pqtText_ComboBox_Converter> pqtText_ComboBox;

#endif // USE_QT
