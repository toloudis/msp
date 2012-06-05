/*****************************************************************************
**  mtrlFur.hpp
**
**      mtrlFur is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_FUR_HPP
#error mtrlFur.hpp multiply included
#endif
#define MTRL_FUR_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_FURDATA_HPP
#include "Support/mtrl/GUI/mtrlFurData.hpp"
#endif

class effFurData;
class mdlMaterialInfo;

//============================================================================
//============================================================================
class mtrlFur : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlFur(mdlMaterialInfo& i_Data, effFurData* i_pShaderData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlFur() {}
private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlFurData m_Data;
	effFurData* m_pShaderData;
	mdlMaterialInfo& m_Material;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void RegisterData();

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void Update(prtyProperty *i_pProperty, bool i_bDirty);

	//----------------------------------------------------------------------------
	// Set shader data from material into shader data
	//----------------------------------------------------------------------------
	void set_shader_data(effFurData& i_Data);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateTextureFolder(prtyProperty *i_pProperty, bool i_bDirty);

};
