/*****************************************************************************
**  effRampData.hpp
**
**      effRampData is the data for a ramp shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_RAMPDATA_HPP
#error effRampData.hpp multiply included
#endif
#define EFF_RAMPDATA_HPP

#ifndef MA_GRADIENT_HPP
#include "Core/ma/maGradient.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class matTexture;


//============================================================================
//============================================================================
class effRampData : public effShaderData
{
public:
	effRampData();
	virtual ~effRampData();
	effRampData(const effRampData& i_CopyFrom);
	effRampData& operator = (const effRampData& i_CopyFrom);

	virtual bool HasTransparency();

	virtual effShaderData* Clone() const {return new effRampData(*this);}

	maGradient m_Gradient;
	matTexture* m_Texture;
	matTexture* m_NoiseTexture;
	int m_RampShape;
	int m_RampInterpolation;
	float m_UWave;
	float m_UWaveFreq;
	float m_VWave;
	float m_VWaveFreq;
	float m_Noise;
	float m_NoiseFreq;
};
