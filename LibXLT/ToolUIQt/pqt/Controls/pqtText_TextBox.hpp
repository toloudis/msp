/****************************************************************************\
**	pqtText_TextBox.hpp
**
**		Intermediate class between the property (prtyText) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEXT_TEXTBOX_HPP
#error pqtText_TextBox.hpp multiply included
#endif
#define PQT_TEXT_TEXTBOX_HPP

#ifndef PQT_TEMPLATE_TEXTBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_TextBox.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtText_TextBox_Converter 
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcTextBox* i_pActualControl,
									 std::string i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static std::string GetValueFromControl(tqcTextBox* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_TextBox<prtyText, std::string, pqtText_TextBox_Converter> pqtText_TextBox;

