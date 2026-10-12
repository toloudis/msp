/****************************************************************************\
**	pqtFilePath_FilePicker.hpp
**
**		Intermediate class between the property (prtyFilePath) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FILEPATH_FILEPICKER_HPP
#error pqtFilePath_FilePicker.hpp multiply included
#endif
#define PQT_FILEPATH_FILEPICKER_HPP

#ifndef PQT_TEMPLATE_FILEPICKER_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_FilePicker.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtFilePath_FilePicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcFilePicker* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(tqcFilePicker* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_FilePicker<prtyFilePath, fsLocator, pqtFilePath_FilePicker_Converter> pqtFilePath_FilePicker;

#endif // USE_QT
