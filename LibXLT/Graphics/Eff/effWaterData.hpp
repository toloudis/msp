/*****************************************************************************
**  effWaterData.hpp
**
**      effWaterData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_WATERDATA_HPP
#error effWaterData.hpp multiply included
#endif
#define EFF_WATERDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effWaterData : public effShaderData
{
public:
	effWaterData();
	virtual ~effWaterData();
	effWaterData(const effWaterData& i_CopyFrom);
	effWaterData& operator = (const effWaterData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effWaterData(*this);}
	virtual chDefs::Name GetChunkName() const;
	virtual maFloatRGBA GetDiffuse() const {return m_WaterColor;}

	float m_FadeBias;
	float m_FadeExp;
	float m_NoiseBumpFactor;
	float m_NoiseSpeed;
	float m_RingBumpFactor;
	float m_RingFreq;
	float m_RingSpeed;
	float m_TimeOffset;
	float m_WaveSpeed;
	maVector4d m_RingCenter;
	maFloatRGBA m_WaterColor;

	void AddToParams(effShaderParams& o_Params);
};
