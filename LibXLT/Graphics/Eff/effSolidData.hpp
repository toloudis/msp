/*****************************************************************************
**  effSolidData.hpp
**
**      effSolidData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_SOLIDDATA_HPP
#error effSolidData.hpp multiply included
#endif
#define EFF_SOLIDDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effSolidData : public effShaderData
{
public:
	effSolidData();
	virtual ~effSolidData();
	effSolidData(const effSolidData& i_CopyFrom);
	effSolidData& operator = (const effSolidData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effSolidData(*this);}

	maFloatRGBA m_Color;
};
