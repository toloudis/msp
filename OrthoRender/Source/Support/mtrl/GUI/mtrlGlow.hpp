/*****************************************************************************
**  mtrlGlow.hpp
**
**      mtrlGlow is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_GLOW_HPP
#error mtrlGlow.hpp multiply included
#endif
#define MTRL_GLOW_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_GLOWDATA_HPP
#include "Support/mtrl/GUI/mtrlGlowData.hpp"
#endif

class effGlowData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlGlow : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlGlow(mdlMaterialInfo& i_Data, 
			 effGlowData* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlGlow() {}

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
	mtrlGlowData m_Data;
	effGlowData* m_pShaderData;
	mdlMaterialInfo& m_Material;

	itString m_CurrentGlowMask;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pGlowFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effGlowData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty);
};
