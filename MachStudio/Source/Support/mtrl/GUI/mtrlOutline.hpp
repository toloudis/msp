/*****************************************************************************
**  mtrlOutline.hpp
**
**      mtrlOutline is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_OUTLINE_HPP
#error mtrlOutline.hpp multiply included
#endif
#define MTRL_OUTLINE_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_OUTLINEDATA_HPP
#include "Support/mtrl/GUI/mtrlOutlineData.hpp"
#endif
#ifndef GPX_EFFECTOUTLINE_HPP
#include "Tool/gpx/gpxEffectOutline.hpp"
#endif 

class effOutlineData;
class mdlMaterialInfo;

//============================================================================
//============================================================================
class mtrlOutline : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlOutline(mdlMaterialInfo& i_Data, 
			 effOutlineData* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlOutline() {}

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
	mtrlOutlineData m_Data;
	effOutlineData* m_pShaderData;
	shared_ptr<gpxEffectOutline> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	itString m_CurrentOutlineMask;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effOutlineData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty);
};
