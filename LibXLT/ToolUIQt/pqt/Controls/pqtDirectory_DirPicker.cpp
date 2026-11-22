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

#ifdef USE_QT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtDirectory_DirPicker_Converter::SetValueIntoControl(tqcDirPicker* i_pActualControl,
											   fsLocator i_Value)
{
	std::string file_path;
	fsFileUtil::LocatorToUnicodeString(i_Value, file_path);
	i_pActualControl->SetPath(QString::fromStdString(file_path));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pqtDirectory_DirPicker_Converter::GetValueFromControl(tqcDirPicker* i_pActualControl)
{
	std::string fullpath(i_pActualControl->GetPath().toStdString());
	fsLocator locator;
	fsFileUtil::UnicodeStringToLocator(fullpath, locator);
	return locator;
}

#endif
