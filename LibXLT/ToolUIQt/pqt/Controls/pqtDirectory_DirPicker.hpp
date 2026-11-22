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

#ifdef USE_QT

//===================================================================================
//===================================================================================
class pqtDirectory_DirPicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcDirPicker* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(tqcDirPicker* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pqtTemplate_DirPicker<prtyDirectory, fsLocator, pqtDirectory_DirPicker_Converter> pqtDirectory_DirPicker;

#endif // USE_QT
