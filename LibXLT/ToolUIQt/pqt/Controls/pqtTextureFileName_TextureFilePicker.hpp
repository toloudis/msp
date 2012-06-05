/****************************************************************************\
**	pqtTextureFileName_TextureFilePicker.hpp
**
**		Intermediate class between the property (prtyTextureFileName) and 
**	the control (TextureFilePicker).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_TEXTUREFILENAME_TEXTUREFILEPICKER_HPP
#error pqtTextureFileName_TextureFilePicker.hpp multiply included
#endif
#define PQT_TEXTUREFILENAME_TEXTUREFILEPICKER_HPP

#ifndef PQT_TEMPLATE_TEXTUREFILEPICKER_HPP
#include "ToolUIQt/pqt/Controls/pqtTemplate_TextureFilePicker.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif
#ifndef PRTY_TEXTUREFILECHOOSERUIINFO_HPP
#include "Core/prty/prtyTextureFileChooserUIInfo.hpp"
#endif


//===================================================================================
//===================================================================================
class pqtTextureFileName_TextureFilePicker_Converter 
{
#ifdef QT_FINISH_PORT
public:
	//----------------------------------------------------------------------------
	// Get list of string choices for choice box
	//----------------------------------------------------------------------------
	static void GetChoices(prtyPropertyUIInfo* i_pUIInfo,
							std::vector<std::string>& o_Choices);

	//----------------------------------------------------------------------------
	// Set value into control
	//----------------------------------------------------------------------------
	static void SetValueIntoControl(tqcTextureFilePicker* i_pActualControl,
									 fsLocator i_Value);

	//----------------------------------------------------------------------------
	// Get value from control
	//----------------------------------------------------------------------------
	static fsLocator GetValueFromControl(tqcTextureFilePicker* i_pActualControl);
#endif // USE_QT
};

//============================================================================
//============================================================================
typedef pqtTemplate_TextureFilePicker<prtyTextureFileName, fsLocator, pqtTextureFileName_TextureFilePicker_Converter> pqtTextureFileName_TextureFilePicker;

