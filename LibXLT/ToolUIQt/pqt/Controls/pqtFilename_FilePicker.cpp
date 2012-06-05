/****************************************************************************\
**	pqtFileName_FilePicker.hpp
**
**		Intermediate class between the property (prtyFileName) and 
**	the control (FilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_FINISH_PORT

#include "pqtFileName_FilePicker.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pqtFileName_FilePicker_Converter::SetValueIntoControl(tqcFilePicker* i_pActualControl,
											   itString i_Value)
{
	i_pActualControl->SetFullpath(fsLocator(i_Value));
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
itString pqtFileName_FilePicker_Converter::GetValueFromControl(tqcFilePicker* i_pActualControl)
{
	fsLocator fullpath = i_pActualControl->GetFullpath();
	if (fullpath.GetNumNames() > 0)
		return fullpath.GetLastName();
	else
		return itString();
}

#endif
