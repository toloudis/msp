/*****************************************************************************
**  mtrlWater.hpp
**
**      mtrlWater is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_WATER_HPP
#error mtrlWater.hpp multiply included
#endif
#define MTRL_WATER_HPP

#ifndef MTRL_SHADEROBJECT_HPP
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#endif
#ifndef MTRL_WATERDATA_HPP
#include "Support/mtrl/GUI/mtrlWaterData.hpp"
#endif

class effWaterData;
class mdlMaterialInfo;

//============================================================================
//============================================================================
class mtrlWater : public mtrlShaderObject
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlWater(mdlMaterialInfo& i_Data, effWaterData* i_pShaderData);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~mtrlWater() {}
private:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	mtrlWaterData m_Data;
	effWaterData* m_pShaderData;
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
	void set_shader_data(effWaterData* i_pData);
};
