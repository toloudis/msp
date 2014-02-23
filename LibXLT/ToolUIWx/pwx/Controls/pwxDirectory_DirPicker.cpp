/****************************************************************************\
**	pwxDirectory_DirPicker.hpp
**
**		Intermediate class between the property (prtyDirectory) and 
**	the control (DirPicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxDirectory_DirPicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxDirectory_DirPicker_Converter::SetValueIntoControl(wxDirPickerCtrl* i_pActualControl,
											   fsLocator i_Value)
{
	itString file_path;
	fsFileUtil::LocatorToUnicodeString(i_Value, file_path);
	i_pActualControl->SetPath(file_path.GetString());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pwxDirectory_DirPicker_Converter::GetValueFromControl(wxDirPickerCtrl* i_pActualControl)
{
	itString fullpath((const char*)(i_pActualControl->GetPath().c_str()));
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(fullpath, locator);
	return locator;
}

#endif
