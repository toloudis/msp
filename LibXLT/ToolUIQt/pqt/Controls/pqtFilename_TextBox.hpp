/****************************************************************************\
**	pqtFileName_TextBox.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (TextBox).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FILENAME_TEXTBOX_HPP
#error pqtFileName_TextBox.hpp multiply included
#endif
#define PQT_FILENAME_TEXTBOX_HPP

#ifndef PQT_TEMPLATE_TEXTBOX_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_TextBox.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtFileName_TextBox_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcTextBox* i_pActualControl,
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(tqcTextBox* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_TextBox<prtyFileName, itString, pqtFileName_TextBox_Converter> pqtFileName_TextBox;

#endif // USE_QT
