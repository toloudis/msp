/****************************************************************************\
**	pqtName_TextBox.hpp
**
**		Intermediate class between the property (prtyName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_NAME_TEXTBOX_HPP
#error pqtName_TextBox.hpp multiply included
#endif
#define PQT_NAME_TEXTBOX_HPP

#ifndef PQT_TEMPLATE_TEXTBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_TextBox.hpp"
#endif
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtName_TextBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcTextBox* i_pActualControl,
									 nameString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static nameString GetValueFromControl(tqcTextBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_TextBox<prtyName, nameString, pqtName_TextBox_Converter> pqtName_TextBox;

#endif // USE_QT
