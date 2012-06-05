/****************************************************************************\
**	pwxFilePath_FilePicker.hpp
**
**		Intermediate class between the property (prtyFilePath) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FILEPATH_FILEPICKER_HPP
#error pwxFilePath_FilePicker.hpp multiply included
#endif
#define PWX_FILEPATH_FILEPICKER_HPP

#ifndef PWX_TEMPLATE_FILEPICKER_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_FilePicker.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxFilePath_FilePicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcFilePicker* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(twcFilePicker* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_FilePicker<prtyFilePath, fsLocator, pwxFilePath_FilePicker_Converter> pwxFilePath_FilePicker;

#endif // USE_WXWIDGETS
