/*****************************************************************************
**  mtrlDisplacement.hpp
**
**      mtrlDisplacement is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_DISPLACEMENT_HPP
#error mtrlDisplacement.hpp multiply included
#endif
#define MTRL_DISPLACEMENT_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_DISPLACEMENTDATA_HPP
#include "Support/mtrl/GUI/mtrlDisplacementData.hpp"
#endif
#ifndef GPX_EFFECTDISPLACEMENT_HPP
#include "Tool/gpx/gpxEffectDisplacement.hpp"
#endif 

class effDisplacementData;
class mdlMaterialInfo;
class prtyFileChooserUIInfo;
class prtyTextureFileChooserUIInfo;

//============================================================================
//============================================================================
class mtrlDisplacement : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlDisplacement(mdlMaterialInfo& i_Data, 
			 effDisplacementData* i_pShaderData,
			 const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlDisplacement() {}

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
	mtrlDisplacementData m_Data;
	effDisplacementData* m_pShaderData;
	shared_ptr<gpxEffectDisplacement> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	//---------------------------------------------------------------------------
	// Property UI Infos
	//---------------------------------------------------------------------------
	prtyTextureFileChooserUIInfo* m_pDisplacementFileChooser;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effDisplacementData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateDisplacementMap(prtyProperty *i_pProperty, bool i_bDirty);
};
