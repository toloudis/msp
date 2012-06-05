/****************************************************************************\
**	pqtFileName_FilePicker.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FILENAME_FILEPICKER_HPP
#error pqtFileName_FilePicker.hpp multiply included
#endif
#define PQT_FILENAME_FILEPICKER_HPP

#ifndef PQT_TEMPLATE_FILEPICKER_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_FilePicker.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtFileName_FilePicker_Converter 
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcFilePicker* i_pActualControl,
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(tqcFilePicker* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_FilePicker<prtyFileName, itString, pqtFileName_FilePicker_Converter> pqtFileName_FilePicker;

