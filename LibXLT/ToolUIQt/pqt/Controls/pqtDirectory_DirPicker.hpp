/****************************************************************************\
**	pqtDirectory_DirPicker.hpp
**
**		Intermediate class between the property (prtyDirectory) and 
**	the control (DirPicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_DIRECTORY_DIRPICKER_HPP
#error pqtDirectory_DirPicker.hpp multiply included
#endif
#define PQT_DIRECTORY_DIRPICKER_HPP

#ifndef PQT_TEMPLATE_DIRPICKER_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_DirPicker.hpp"
#endif
#ifndef PRTY_DIRECTORY_HPP
#include "Core/prty/prtyDirectory.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtDirectory_DirPicker_Converter 
{
#ifdef QT_FINISH_PORT
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
#endif
};

//============================================================================
//============================================================================
typedef pqtTemplate_DirPicker<prtyDirectory, fsLocator, pqtDirectory_DirPicker_Converter> pqtDirectory_DirPicker;
