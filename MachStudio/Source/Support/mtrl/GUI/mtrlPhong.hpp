/*****************************************************************************
**  mtrlPhong.hpp
**
**      mtrlPhong is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_PHONG_HPP
#error mtrlPhong.hpp multiply included
#endif
#define MTRL_PHONG_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_PHONGDATA_HPP
#include "Support/mtrl/GUI/mtrlPhongData.hpp"
#endif

class effPhongData;
class matMaterial;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlPhong : public mtrlShaderObject
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlPhong(mdlMaterialInfo& i_MatData,
		matMaterial* i_pMaterial,
		effPhongData* i_pShaderData,
		const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual ~mtrlPhong() {}

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
	mtrlPhongData m_Data;
	effPhongData* m_pShaderData;
	mdlMaterialInfo& m_Material;
	matMaterial* m_pActualMaterial;
	
	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pDiffuseFileChooser;
	prtyFileChooserUIInfo* m_pSpecularFileChooser;
	prtyFileChooserUIInfo* m_pGlossFileChooser;
	prtyFileChooserUIInfo* m_pEnvironmentFileChooser;
	prtyFileChooserUIInfo* m_pNormalFileChooser;
	prtyFileChooserUIInfo* m_pReflectFactorFileChooser;
	prtyFileChooserUIInfo* m_pTransparencyFileChooser;
	prtyFileChooserUIInfo* m_pDisplacementFileChooser;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void RegisterData(const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effPhongData* i_pData);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureSpecular(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureGloss(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureEnvironment(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureReflectFactor(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureDisplacement(prtyProperty *i_pProperty, bool i_bDirty);

	//---------------------------------------------------------------------------
	// select appropriate shader name 
	//---------------------------------------------------------------------------
	void SelectShader(bool i_UseMtrlTemplate);
};
