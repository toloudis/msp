/*****************************************************************************
**  mtrlTextureFilter.hpp
**
**      mtrlTextureFilter is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_TEXTUREFILTER_HPP
#error mtrlTextureFilter.hpp multiply included
#endif
#define MTRL_TEXTUREFILTER_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_TEXTUREFILTERDATA_HPP
#include "Support/mtrl/GUI/mtrlTextureFilterData.hpp"
#endif
#ifndef GPX_EFFECTTEXTUREFILTER_HPP
#include "Tool/gpx/gpxEffectTextureFilter.hpp"
#endif 

class effTextureFilterData;
class mdlMaterialInfo;

//============================================================================
//============================================================================
class mtrlTextureFilter : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlTextureFilter(mdlMaterialInfo& i_Data,
			 effTextureFilterData* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlTextureFilter() {}

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
	mtrlTextureFilterData m_Data;
	effTextureFilterData* m_pShaderData;
	shared_ptr<gpxEffectTextureFilter> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effTextureFilterData& i_Data);
};
