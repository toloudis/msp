/*****************************************************************************
**  mtrlToon.hpp
**
**      mtrlToon is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_TOON_HPP
#error mtrlToon.hpp multiply included
#endif
#define MTRL_TOON_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_TOONDATA_HPP
#include "Support/mtrl/GUI/mtrlToonData.hpp"
#endif

class effToonData;
class matMaterial;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlToon : public mtrlShaderObject
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlToon(mdlMaterialInfo& i_MatData,
		effToonData* i_pShaderData,
		const fsLocator& i_TextureDir);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	virtual ~mtrlToon() {}

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
	mtrlToonData m_Data;
	effToonData* m_pShaderData;
	mdlMaterialInfo& m_Material;
	
	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pDiffuseFileChooser;
	prtyFileChooserUIInfo* m_pGradientFileChooser;
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
	void set_shader_data(effToonData* i_pData);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void UpdateTextureDiffuse(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureGradient(prtyProperty *i_pProperty, bool i_bDirty);
	void UpdateTextureTransparency(prtyProperty *i_pProperty, bool i_bDirty);
};
