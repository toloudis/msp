/****************************************************************************\
**	pwxTextureFileName_TextureFilePicker.hpp
**
**		Intermediate class between the property (prtyTextureFileName) and 
**	the control (TextureFilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/Controls/pwxTextureFileName_TextureFilePicker.hpp"

#include "Core/fs/fsFileUtil.hpp"

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
// Get list of string choices for combo box
//----------------------------------------------------------------------------
void pwxTextureFileName_TextureFilePicker_Converter::GetChoices(prtyPropertyUIInfo* i_pUIInfo,
								  std::vector<std::string>& o_Choices)
{
	prtyTextureFileChooserUIInfo* pUII = static_cast<prtyTextureFileChooserUIInfo*>(i_pUIInfo);
	o_Choices = pUII->m_List;
}

//----------------------------------------------------------------------------
// Set value into control
//----------------------------------------------------------------------------
void pwxTextureFileName_TextureFilePicker_Converter::SetValueIntoControl(twcTextureFilePicker* i_pActualControl,
											   fsLocator i_Value)
{
	i_pActualControl->SetFullpath(i_Value);
}

//----------------------------------------------------------------------------
// Get value from control
//----------------------------------------------------------------------------
fsLocator pwxTextureFileName_TextureFilePicker_Converter::GetValueFromControl(twcTextureFilePicker* i_pActualControl)
{
	return i_pActualControl->GetFullpath();
}

#endif
