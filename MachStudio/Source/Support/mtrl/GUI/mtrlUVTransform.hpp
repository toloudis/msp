/*****************************************************************************
**  mtrlUVTransform.hpp
**
**      mtrlUVTransform is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_UVTRANSFORM_HPP
#error mtrlUVTransform.hpp multiply included
#endif
#define MTRL_UVTRANSFORM_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_UVTRANSFORMDATA_HPP
#include "Support/mtrl/GUI/mtrlUVTransformData.hpp"
#endif
#ifndef GPX_EFFECTUVTRANSFORM_HPP
#include "Tool/gpx/gpxEffectUVTransform.hpp"
#endif 

class effUVTransform;
class mdlMaterialInfo;

//============================================================================
//============================================================================
class mtrlUVTransform : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlUVTransform(mdlMaterialInfo& i_Data, 
			 effUVTransform* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlUVTransform() {}

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
	mtrlUVTransformData m_Data;
	effUVTransform* m_pShaderData;
	shared_ptr<gpxEffectUVTransform> m_pShaderDataProxy;
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
	void set_shader_data(effUVTransform& i_Data);
};
