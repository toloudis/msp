/*****************************************************************************
**  mtrlRefl.hpp
**
**      mtrlRefl is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_REFL_HPP
#error mtrlRefl.hpp multiply included
#endif
#define MTRL_REFL_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_REFLDATA_HPP
#include "Support/mtrl/GUI/mtrlReflData.hpp"
#endif

class effReflectionMap;
class mdlMaterialInfo;
class gpxEffectReflection;

//============================================================================
//============================================================================
class mtrlRefl : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlRefl(mdlMaterialInfo& i_Data, 
			 effReflectionMap* i_pShaderData,
			 const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlRefl() {}

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
	mtrlReflData m_Data;
	effReflectionMap* m_pShaderData;
	shared_ptr<gpxEffectReflection> m_pShaderDataProxy;
	mdlMaterialInfo& m_Material;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData(const fsLocator& i_TextureDir);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateReflectionMap(prtyProperty *i_pProperty, bool i_bDirty);
};
