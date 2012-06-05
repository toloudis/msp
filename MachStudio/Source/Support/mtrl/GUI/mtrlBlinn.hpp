/*****************************************************************************
**  mtrlBlinn.hpp
**
**      mtrlBlinn is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_BLINN_HPP
#error mtrlBlinn.hpp multiply included
#endif
#define MTRL_BLINN_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_PHONGDATA_HPP
#include "Support/mtrl/GUI/mtrlBlinnData.hpp"
#endif

class effBlinnData;
class matMaterial;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlBlinn : public mtrlShaderObject
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlBlinn(mdlMaterialInfo& i_MatData,
		effBlinnData* i_pShaderData,
		const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual ~mtrlBlinn() {}

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
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlBlinnData m_Data;
	effBlinnData* m_pShaderData;
	mdlMaterialInfo& m_Material;
	
	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pDiffuseFileChooser;
	prtyFileChooserUIInfo* m_pSpecularFileChooser;
	prtyFileChooserUIInfo* m_pGlossFileChooser;
	prtyFileChooserUIInfo* m_pEnvironmentFileChooser;
	prtyFileChooserUIInfo* m_pNormalFileChooser;
	prtyFileChooserUIInfo* m_pReflectFactorFileChooser;
	prtyFileChooserUIInfo* m_pIORFileChooser;
	prtyFileChooserUIInfo* m_pTransparencyFileChooser;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void RegisterData(const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effBlinnData* i_pData);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpecular(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureGloss(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureEnvironment(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureReflectFactor(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureIOR(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty);
};
