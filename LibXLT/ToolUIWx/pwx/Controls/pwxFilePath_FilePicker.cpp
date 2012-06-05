/****************************************************************************\
**	pwxFilePath_FilePicker.hpp
**
**		Intermediate class between the property (prtyFilePath) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxFilePath_FilePicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxFilePath_FilePicker_Converter::SetValueIntoControl(twcFilePicker* i_pActualControl,
											   fsLocator i_Value)
{
	i_pActualControl->SetFullpath(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pwxFilePath_FilePicker_Converter::GetValueFromControl(twcFilePicker* i_pActualControl)
{
	return i_pActualControl->GetFullpath();
}

#endif
