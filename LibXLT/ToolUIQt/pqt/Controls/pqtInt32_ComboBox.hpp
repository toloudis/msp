/****************************************************************************\
**	pqtInt32_ComboBox.hpp
**
**		Intermediate class between the property (prtyInt32) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_INT32_COMBOBOX_HPP
#error pqtInt32_ComboBox.hpp multiply included
#endif
#define PQT_INT32_COMBOBOX_HPP

#ifndef PQT_TEMPLATE_COMBOBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtInt32_ComboBox_Converter 
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
									 int i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static int GetValueFromControl(QComboBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_ComboBox<prtyInt32, int, pqtInt32_ComboBox_Converter> pqtInt32_ComboBox;

#endif // USE_QT
