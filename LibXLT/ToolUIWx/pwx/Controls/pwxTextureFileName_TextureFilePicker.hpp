/****************************************************************************\
**	pwxTextureFileName_TextureFilePicker.hpp
**
**		Intermediate class between the property (prtyTextureFileName) and 
**	the control (TextureFilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PWX_TEXTUREFILENAME_TEXTUREFILEPICKER_HPP
#error pwxTextureFileName_TextureFilePicker.hpp multiply included
#endif
#define PWX_TEXTUREFILENAME_TEXTUREFILEPICKER_HPP

#ifndef PWX_TEMPLATE_TEXTUREFILEPICKER_HPP
#include "ToolUIWx/pwx/Controls/pwxTemplate_TextureFilePicker.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef PRTY_TEXTUREFILECHOOSERUIINFO_HPP
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#endif

#ifdef USE_WXWIDGETS

//===================================================================================
//===================================================================================
class pwxTextureFileName_TextureFilePicker_Converter 
{
public:
	//----------------------------------------------------------------------------
	// Get list of string choices for choice box
	//----------------------------------------------------------------------------
	static void GetChoices(prtyPropertyUIInfo* i_pUIInfo,
							std::vector<std::string>& o_Choices);

	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(twcTextureFilePicker* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(twcTextureFilePicker* i_pActualControl);
};

//============================================================================
//============================================================================
typedef pwxTemplate_TextureFilePicker<prtyTextureFileName, fsLocator, pwxTextureFileName_TextureFilePicker_Converter> pwxTextureFileName_TextureFilePicker;

#endif // USE_WXWIDGETS
