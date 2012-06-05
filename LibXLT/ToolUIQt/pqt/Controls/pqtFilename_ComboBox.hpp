/****************************************************************************\
**	pqtFileName_ComboBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (ComboBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FILENAME_COMBOBOX_HPP
#error pqtFileName_ComboBox.hpp multiply included
#endif
#define PQT_FILENAME_COMBOBOX_HPP

#ifndef PQT_TEMPLATE_COMBOBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_ComboBox.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtFileName_ComboBox_Converter 
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
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(wxChoice* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_ComboBox<prtyFileName, itString, pqtFileName_ComboBox_Converter> pqtFileName_ComboBox;

