/****************************************************************************\
**	pqtDirectory_DirPicker.hpp
**
**		Intermediate class between the property (prtyDirectory) and 
**	the control (DirPicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtDirectory_DirPicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtDirectory_DirPicker_Converter::SetValueIntoControl(wxDirPickerCtrl* i_pActualControl,
											   fsLocator i_Value)
{
	itString file_path;
	fsFileUtil::LocatorToUnicodeString(i_Value, file_path);
	i_pActualControl->SetPath(file_path.GetString());
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pqtDirectory_DirPicker_Converter::GetValueFromControl(wxDirPickerCtrl* i_pActualControl)
{
	itString fullpath(i_pActualControl->GetPath().c_str());
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(fullpath, locator);
	return locator;
}

#endif
