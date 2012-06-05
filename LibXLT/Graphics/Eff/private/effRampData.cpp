/*****************************************************************************
**  effRampData.cpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effRampData.hpp"
#include "Graphics/mat/matTexture.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRampData::effRampData()
:	m_Texture(NULL), m_NoiseTexture(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRampData::~effRampData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRampData::effRampData(const effRampData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRampData& effRampData::operator = (const effRampData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_Gradient = i_CopyFrom.m_Gradient;
	m_RampShape = i_CopyFrom.m_RampShape;
	m_RampInterpolation = i_CopyFrom.m_RampInterpolation;
	m_UWave = i_CopyFrom.m_UWave;
	m_UWaveFreq = i_CopyFrom.m_UWaveFreq;
	m_VWave = i_CopyFrom.m_VWave;
	m_VWaveFreq = i_CopyFrom.m_VWaveFreq;
	m_Noise = i_CopyFrom.m_Noise;
	m_NoiseFreq = i_CopyFrom.m_NoiseFreq;
	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effRampData::HasTransparency()
{
	for (int i = 0; i < m_Gradient.GetSize(); i++)
	{
		if (m_Gradient[i].second.GetAlpha() != 1.0f)
		{
			return false;
		}
	}

	return false;
}
