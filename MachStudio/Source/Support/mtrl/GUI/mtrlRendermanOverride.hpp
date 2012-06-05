/*****************************************************************************
**  mtrlRendermanOverride.hpp
**
**      mtrlRendermanOverride is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_RENDERMANOVERRIDE_HPP
#error mtrlRendermanOverride.hpp multiply included
#endif
#define MTRL_RENDERMANOVERRIDE_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_RENDERMANOVERRIDEDATA_HPP
#include "Support/mtrl/GUI/mtrlRendermanOverrideData.hpp"
#endif
#ifndef GPX_EFFECTRENDERMANOVERRIDE_HPP
#include "Tool/gpx/gpxEffectRendermanOverride.hpp"
#endif 

class effRendermanOverrideData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;
class prtyTextureFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlRendermanOverride : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlRendermanOverride(mdlMaterialInfo& i_Data, 
			 effRendermanOverrideData* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlRendermanOverride() {}

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
	mtrlRendermanOverrideData m_Data;
	effRendermanOverrideData* m_pShaderData;
	shared_ptr<gpxEffectRendermanOverride> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyFileChooserUIInfo* m_pRendermanOverrideFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effRendermanOverrideData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty);
};
