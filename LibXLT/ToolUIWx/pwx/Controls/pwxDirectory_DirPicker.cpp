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
	std::string file_path;
	fsFileUtil::LocatorToUnicodeString(i_Value, file_path);
	i_pActualControl->SetPath(wxString::FromUTF8(file_path));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pwxDirectory_DirPicker_Converter::GetValueFromControl(wxDirPickerCtrl* i_pActualControl)
{
	std::string fullpath(i_pActualControl->GetPath().utf8_string());
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(fullpath, locator);
	return locator;
}

#endif
