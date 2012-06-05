/*****************************************************************************
**  mtrlNormals.hpp
**
**      mtrlNormals is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_NORMALS_HPP
#error mtrlNormals.hpp multiply included
#endif
#define MTRL_NORMALS_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_NORMALSDATA_HPP
#include "Support/mtrl/GUI/mtrlNormalsData.hpp"
#endif
#ifndef GPX_EFFECTNORMALS_HPP
#include "Tool/gpx/gpxEffectNormals.hpp"
#endif 

class effNormalsData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;
class prtyTextureFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlNormals : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlNormals(mdlMaterialInfo& i_Data, 
			 effNormalsData* i_pShaderData,
			 const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlNormals() {}

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
	mtrlNormalsData m_Data;
	effNormalsData* m_pShaderData;
	shared_ptr<gpxEffectNormals> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyTextureFileChooserUIInfo* m_pNormalsFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator &i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effNormalsData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateMaskTexture(prtyProperty *i_pProperty, bool i_bDirty);
};
