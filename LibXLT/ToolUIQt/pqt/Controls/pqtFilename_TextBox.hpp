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


//===================================================================================
//===================================================================================
class pqtFileName_TextBox_Converter 
{
#ifdef QT_FINISH_PORT
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
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_TextBox<prtyFileName, itString, pqtFileName_TextBox_Converter> pqtFileName_TextBox;

