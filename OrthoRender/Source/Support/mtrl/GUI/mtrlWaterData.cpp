/********************************************************************************************\
**  mtrlWaterData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlWaterData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlWaterData::mtrlWaterData()
:	m_FadeBias("Fade Bias", 0.3f),
	m_FadeExp("Fade Exp", 6.08f),
	m_NoiseBumpFactor("Noise Bump Factor", 1),
	m_NoiseSpeed("Noise Speed", 0.18f),
	m_RingBumpFactor("Ring Bump Factor", 1),
	m_RingFreq("Ring Frequency", 1),
	m_RingSpeed("Ring Speed", 10),
	m_TimeOffset("Time Offset", 100),
	m_WaveSpeed("Wave Speed", 0.34f),
	m_RingCenter("Ring Center", maPoint3d(0,0,0)),
	m_WaterColor("Water Color", maFloatRGBA(50.0f/255.0f, 88.0f/255.0f, 174.0f/255.0f, 1))
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlWaterData::mtrlWaterData(const mtrlWaterData& i_Data)
:	m_FadeBias("Fade Bias", 0.3f),
	m_FadeExp("Fade Exp", 6.08f),
	m_NoiseBumpFactor("Noise Bump Factor", 1),
	m_NoiseSpeed("Noise Speed", 0.18f),
	m_RingBumpFactor("Ring Bump Factor", 1),
	m_RingFreq("Ring Frequency", 1),
	m_RingSpeed("Ring Speed", 10),
	m_TimeOffset("Time Offset", 100),
	m_WaveSpeed("Wave Speed", 0.34f),
	m_RingCenter("Ring Center", maPoint3d(0,0,0)),
	m_WaterColor("Water Color", maFloatRGBA(50.0f/255.0f, 88.0f/255.0f, 174.0f/255.0f, 1))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlWaterData::~mtrlWaterData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlWaterData& mtrlWaterData::operator=(const mtrlWaterData& i_Data)
{
	if (this == &i_Data) return *this;

	m_FadeBias			= i_Data.m_FadeBias;
	m_FadeExp			= i_Data.m_FadeExp;
	m_NoiseBumpFactor	= i_Data.m_NoiseBumpFactor;
	m_NoiseSpeed		= i_Data.m_NoiseSpeed;
	m_RingBumpFactor	= i_Data.m_RingBumpFactor;
	m_RingFreq			= i_Data.m_RingFreq;
	m_RingSpeed			= i_Data.m_RingSpeed;
	m_TimeOffset		= i_Data.m_TimeOffset;
	m_WaveSpeed			= i_Data.m_WaveSpeed;
	m_RingCenter		= i_Data.m_RingCenter;
	m_WaterColor		= i_Data.m_WaterColor;

	return *this;
}

