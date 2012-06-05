/*****************************************************************************
**  mtrlSkin.hpp
**
**      mtrlSkin is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_SKIN_HPP
#error mtrlSkin.hpp multiply included
#endif
#define MTRL_SKIN_HPP


#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_SKINDATA_HPP
#include "Support/mtrl/GUI/mtrlSkinData.hpp"
#endif

class effSkinData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;


//============================================================================
//============================================================================
class mtrlSkin : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlSkin(mdlMaterialInfo& i_Data, 
			 effSkinData* i_pShaderData,
			 const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlSkin() {}

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
	mtrlSkinData m_Data;
	effSkinData* m_pShaderData;
	mdlMaterialInfo& m_Material;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pDiffuseFileChooser;
	prtyFileChooserUIInfo* m_pNormalFileChooser;
	prtyFileChooserUIInfo* m_pSpecularColorFileChooser;
	prtyFileChooserUIInfo* m_pSpecularPowerFileChooser;
	prtyFileChooserUIInfo* m_pTranslucencyFileChooser;
	prtyFileChooserUIInfo* m_pMicroStructureFileChooser;
	prtyFileChooserUIInfo* m_pCubeMapFileChooser;
	prtyFileChooserUIInfo* m_pReflectFactorFileChooser;
	prtyFileChooserUIInfo* m_pTransparencyFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effSkinData* i_pData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTextureDiff(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpec(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpecPower(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTrans(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureMicro(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureCubeMap(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureReflectFactor(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty);
};
