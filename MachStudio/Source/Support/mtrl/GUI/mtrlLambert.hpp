/*****************************************************************************
**  mtrlLambert.hpp
**
**      mtrlLambert is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_LAMBERT_HPP
#error mtrlLambert.hpp multiply included
#endif
#define MTRL_LAMBERT_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_LAMBERTDATA_HPP
#include "Support/mtrl/GUI/mtrlLambertData.hpp"
#endif

class effLambertData;
class matMaterial;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlLambert : public mtrlShaderObject
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlLambert(mdlMaterialInfo& i_MatData,
		effLambertData* i_pShaderData,
		const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual ~mtrlLambert() {}

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
	mtrlLambertData m_Data;
	effLambertData* m_pShaderData;
	mdlMaterialInfo& m_Material;
	
	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pDiffuseFileChooser;
	prtyFileChooserUIInfo* m_pNormalFileChooser;
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
	void set_shader_data(effLambertData* i_pData);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureNormal(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty);
};
