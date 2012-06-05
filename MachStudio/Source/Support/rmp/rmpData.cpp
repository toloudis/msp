/****************************************************************************\
**  rmpData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004-7 - All Rights Reserved
\****************************************************************************/
#include "Support/rmp/rmpData.hpp"


#include "Core/ch/chChunkParserUtil.hpp"
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rmpData::rmpData()
:	m_Shape("Ramp Shape", 0),
	m_Interpolation("Ramp Interpolation", 0),
	m_TexSize("Ramp Texture Size", 512),
	m_Gradient("Ramp", maGradient()),
	m_UWave("Ramp U Wave", 0.0f),
	m_UWaveFreq("Ramp U Wave Frequency", 0.5f),
	m_VWave("Ramp V Wave", 0.0f),
	m_VWaveFreq("Ramp V Wave Frequency", 0.5f),
	m_Noise("Ramp Noise", 0.0f),
	m_NoiseFreq("Ramp Noise Frequency", 0.5f),
	m_Copy("Copy Ramp data"),
	m_Paste("Paste Ramp data")
{
	m_Shape.SetEnumTag(0, "Linear U");
	m_Shape.SetEnumTag(1, "Linear V");
	m_Shape.SetEnumTag(2, "Circular");
	m_Shape.SetEnumTag(3, "Square");

	m_Interpolation.SetEnumTag(RT_LINEAR, "Linear");
	m_Interpolation.SetEnumTag(RT_NONE, "None");
	m_Interpolation.SetEnumTag(RT_SMOOTH, "Smooth");
	m_Interpolation.SetEnumTag(RT_BUMP, "Bump");
	m_Interpolation.SetEnumTag(RT_SPIKE, "Spike");

	/*m_TexSize.SetEnumTag(0, "512");
	m_TexSize.SetEnumTag(1, "1024");
	m_TexSize.SetEnumTag(2, "2048");
	m_TexSize.SetEnumTag(3, "4096");
	m_TexSize.SetEnumTag(4, "8192");*/
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rmpData::RegisterCallback(shared_ptr<prtyPropertyCallback> i_pCallback)
{
	m_Shape.AddCallback(i_pCallback);
	m_Interpolation.AddCallback(i_pCallback);
	m_TexSize.AddCallback(i_pCallback);
	m_Gradient.AddCallback(i_pCallback);
	m_UWave.AddCallback(i_pCallback);
	m_UWaveFreq.AddCallback(i_pCallback);
	m_VWave.AddCallback(i_pCallback);
	m_VWaveFreq.AddCallback(i_pCallback);
	m_Noise.AddCallback(i_pCallback);
	m_NoiseFreq.AddCallback(i_pCallback);
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
const rmpData& rmpData::operator = (const rmpData& i_CopyFrom)
{
	m_Shape.SetValue(i_CopyFrom.m_Shape.GetValue());
	m_Interpolation.SetValue(i_CopyFrom.m_Interpolation.GetValue());
	m_TexSize = i_CopyFrom.m_TexSize;
	m_Gradient = i_CopyFrom.m_Gradient;

	m_UWave = i_CopyFrom.m_UWave;
	m_UWaveFreq = i_CopyFrom.m_UWaveFreq;
	m_VWave = i_CopyFrom.m_VWave;
	m_VWaveFreq = i_CopyFrom.m_VWaveFreq;
	m_Noise = i_CopyFrom.m_Noise;
	m_NoiseFreq = i_CopyFrom.m_NoiseFreq;

	return *this;
}


