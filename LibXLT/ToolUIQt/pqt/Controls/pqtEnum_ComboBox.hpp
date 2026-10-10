/****************************************************************************\
**	pqtEnum_ComboBox.hpp
**
**		Intermediate class between the property (prtyEnum) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_ENUM_COMBOBOX_HPP
#error pqtEnum_ComboBox.hpp multiply included
#endif
#define PQT_ENUM_COMBOBOX_HPP

#ifndef PQT_TEMPLATE_COMBOBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif

#ifdef USE_QT


//============================================================================
//============================================================================
class pqtEnum_ComboBox_Converter
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
									 short i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static short GetValueFromControl(QComboBox* i_pActualControl);

};

//============================================================================
//============================================================================
typedef pqtTemplate_ComboBox<prtyEnum, short, pqtEnum_ComboBox_Converter> pqtEnum_ComboBox;

#endif // USE_QT
