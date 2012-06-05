/****************************************************************************\
**	pwxFileName_FilePicker.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_FILENAME_FILEPICKER_HPP
#error pwxFileName_FilePicker.hpp multiply included
#endif
#define PWX_FILENAME_FILEPICKER_HPP

#ifndef PWX_TEMPLATE_FILEPICKER_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_FilePicker.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxFileName_FilePicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcFilePicker* i_pActualControl,
									 itString i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static itString GetValueFromControl(twcFilePicker* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_FilePicker<prtyFileName, itString, pwxFileName_FilePicker_Converter> pwxFileName_FilePicker;

#endif // USE_WXWIDGETS
