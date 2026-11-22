/****************************************************************************\
**	pwxFileName_FilePicker.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "pwxFileName_FilePicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFileName_FilePicker_Converter::SetValueIntoControl(twcFilePicker* i_pActualControl,
											   const std::string& i_Value)
{
	i_pActualControl->SetFullpath(fsLocator(i_Value));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
std::string pwxFileName_FilePicker_Converter::GetValueFromControl(twcFilePicker* i_pActualControl)
{
	fsLocator fullpath = i_pActualControl->GetFullpath();
	if (fullpath.GetNumNames() > 0)
		return fullpath.GetLastName();
	else
		return std::string();
}

#endif
