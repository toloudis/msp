/*****************************************************************************
**	effWaterData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effWaterData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/eff/effShaderParams.hpp"
#include "Graphics/eff/effWaterDataParser.hpp"
#include "Graphics/mat/matTexture.hpp"

#include <algorithm>
#include <functional>
#include <vector>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effWaterData::effWaterData()
:	m_FadeBias(0.3f),
	m_FadeExp(6.08f),
	m_NoiseBumpFactor(1),
	m_NoiseSpeed(0.18f),
	m_RingBumpFactor(1),
	m_RingFreq(1),
	m_RingSpeed(10),
	m_TimeOffset(100),
	m_WaveSpeed(0.34f),
	m_RingCenter(0,0,0,1),
	m_WaterColor(50.0f/255.0f, 88.0f/255.0f, 174.0f/255.0f, 1)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effWaterData::~effWaterData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effWaterData::effWaterData(const effWaterData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effWaterData& effWaterData::operator = (const effWaterData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_FadeBias = i_CopyFrom.m_FadeBias;
	m_FadeExp = i_CopyFrom.m_FadeExp;
	m_NoiseBumpFactor = i_CopyFrom.m_NoiseBumpFactor;
	m_NoiseSpeed = i_CopyFrom.m_NoiseSpeed;
	m_RingBumpFactor = i_CopyFrom.m_RingBumpFactor;
	m_RingFreq = i_CopyFrom.m_RingFreq ;
	m_RingSpeed = i_CopyFrom.m_RingSpeed; 
	m_TimeOffset = i_CopyFrom.m_TimeOffset ;
	m_WaveSpeed = i_CopyFrom.m_WaveSpeed ;
	m_RingCenter = i_CopyFrom.m_RingCenter; 
	m_WaterColor = i_CopyFrom.m_WaterColor ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effWaterData::GetChunkName() const
{
	return effWaterDataParser::GetChunkName();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effWaterData::AddToParams(effShaderParams& o_Params)
{
	o_Params.SetVersion(1);
//	o_Params.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", m_BumpMapScale));
//	o_Params.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", m_Reflectivity));
//	o_Params.AddParam(new effParamFloat("g_shininess", "g_shininess", m_SpecularPower));
//	o_Params.AddParam(new effParamColor("g_ambient", "g_ambient", m_ColorAmbient));
//	o_Params.AddParam(new effParamColor("g_diffuse", "g_diffuse", m_ColorDiffuse));
//	o_Params.AddParam(new effParamColor("g_specular", "g_specular", m_ColorSpecular));

//	o_Params.AddParam(new effParamTexture("diffuseTexture", "diffuseTexture", itString(m_NameDiffTex.c_str())));
//	o_Params.AddParam(new effParamTexture("cubeMap", "cubeMap", itString(m_NameCubeMapTex.c_str())));

	o_Params.AddParam(new effParamColor("waterColor", "waterColor", m_WaterColor));
	o_Params.AddParam(new effParamFloat("noiseBumpFactor", "noiseBumpFactor", m_NoiseBumpFactor));
	o_Params.AddParam(new effParamFloat("waveSpeed", "waveSpeed", m_WaveSpeed));
	o_Params.AddParam(new effParamFloat("noiseSpeed", "noiseSpeed", m_NoiseSpeed));
	o_Params.AddParam(new effParamFloat("fadeBias", "fadeBias", m_FadeBias));
	o_Params.AddParam(new effParamFloat("fadeExp", "fadeExp", m_FadeExp));
	o_Params.AddParam(new effParamFloat("timeOffset", "timeOffset", m_TimeOffset));
	o_Params.AddParam(new effParamFloat("ringBumpFactor", "ringBumpFactor", m_RingBumpFactor));
	o_Params.AddParam(new effParamFloat("ringFreq", "ringFreq", m_RingFreq));
	o_Params.AddParam(new effParamFloat("ringSpeed", "ringSpeed", m_RingSpeed));
//	o_Params.AddParam(new effParamVector("ringCenter", "ringCenter", m_RingCenter));
}
