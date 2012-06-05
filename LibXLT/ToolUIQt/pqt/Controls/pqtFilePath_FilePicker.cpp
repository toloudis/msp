/****************************************************************************\
**	pqtFilePath_FilePicker.hpp
**
**		Intermediate class between the property (prtyFilePath) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/Controls/pqtFilePath_FilePicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef QT_FINISH_PORT

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFilePath_FilePicker_Converter::SetValueIntoControl(tqcFilePicker* i_pActualControl,
											   fsLocator i_Value)
{
	i_pActualControl->SetFullpath(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pqtFilePath_FilePicker_Converter::GetValueFromControl(tqcFilePicker* i_pActualControl)
{
	return i_pActualControl->GetFullpath();
}

#endif
