/*****************************************************************************
**  mtrlHair.hpp
**
**      mtrlHair is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_HAIR_HPP
#error mtrlHair.hpp multiply included
#endif
#define MTRL_HAIR_HPP


#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_HAIRDATA_HPP
#include "Support/mtrl/GUI/mtrlHairData.hpp"
#endif

class effHairData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlHair : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlHair(mdlMaterialInfo& i_Data, 
			 effHairData* i_pShaderData,
			 const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlHair() {}

	//------------------------------------------------------------------------
	// Set a new directory for the material in order to find the textures.
	// Called when the material has been exported to the material library.
	//------------------------------------------------------------------------
	virtual void UpdateTextureDirectory(const fsLocator &i_TextureDir);

	//--------------------------------------------------------------------
	// Get list of used textures in order to support copying them
	// to the material library when exporting.
	//--------------------------------------------------------------------
	virtual void GetTextureList(std::vector<fsLocator>& o_TextureList);

private:

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlHairData m_Data;
	effHairData* m_pShaderData;
	mdlMaterialInfo& m_Material;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pBaseHairFileChooser;
	prtyFileChooserUIInfo* m_pAlphaFileChooser;
	prtyFileChooserUIInfo* m_pSpecularShiftFileChooser;
	prtyFileChooserUIInfo* m_pSpecularNoiseFileChooser;
	prtyFileChooserUIInfo* m_pNormalFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effHairData* i_pData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTextureBase(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureAlpha(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpecularShift(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpecularMask(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureNormalMap(prtyProperty *i_pProperty, bool i_bDirty);
};
