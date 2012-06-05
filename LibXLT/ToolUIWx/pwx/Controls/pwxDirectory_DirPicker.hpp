/****************************************************************************\
**	pwxDirectory_DirPicker.hpp
**
**		Intermediate class between the property (prtyDirectory) and 
**	the control (DirPicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_DIRECTORY_DIRPICKER_HPP
#error pwxDirectory_DirPicker.hpp multiply included
#endif
#define PWX_DIRECTORY_DIRPICKER_HPP

#ifndef PWX_TEMPLATE_DIRPICKER_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_DirPicker.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxDirectory_DirPicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(wxDirPickerCtrl* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(wxDirPickerCtrl* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_DirPicker<prtyDirectory, fsLocator, pwxDirectory_DirPicker_Converter> pwxDirectory_DirPicker;

#endif // USE_WXWIDGETS
