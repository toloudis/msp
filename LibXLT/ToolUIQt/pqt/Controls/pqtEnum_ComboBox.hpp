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



//============================================================================
//============================================================================
class pqtEnum_ComboBox_Converter
{
#ifdef QT_FINISH_PORT
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

#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_ComboBox<prtyEnum, short, pqtEnum_ComboBox_Converter> pqtEnum_ComboBox;

