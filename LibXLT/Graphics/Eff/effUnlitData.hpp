/*****************************************************************************
**  effUnlitData.hpp
**
**      effUnlitData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_UNLITDATA_HPP
#error effUnlitData.hpp multiply included
#endif
#define EFF_UNLITDATA_HPP

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effUnlitData : public effShaderData
{
public:
	effUnlitData();
	virtual ~effUnlitData();
	effUnlitData(const effUnlitData& i_CopyFrom);
	effUnlitData& operator = (const effUnlitData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effUnlitData(*this);}

//	maFloatRGBA m_Color;
};
