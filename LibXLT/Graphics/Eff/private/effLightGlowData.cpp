/*****************************************************************************
**  effLightGlowData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effLightGlowData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLightGlowData::effLightGlowData()
:	m_EdgeFuzzCutoff(0.3f),
	m_DistFalloffStart(20.0f),
	m_DistFalloffEnd(50.0f),
	m_GlowAlpha(0.35f),
	m_TextureDiffuse(NULL),
	m_pLight(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLightGlowData::~effLightGlowData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLightGlowData::effLightGlowData(const effLightGlowData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLightGlowData& effLightGlowData::operator = (const effLightGlowData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_EdgeFuzzCutoff = i_CopyFrom.m_EdgeFuzzCutoff ;
	m_DistFalloffStart = i_CopyFrom.m_DistFalloffStart ;
	m_DistFalloffEnd = i_CopyFrom.m_DistFalloffEnd ;
	m_GlowAlpha = i_CopyFrom.m_GlowAlpha ;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;
	m_pLight = i_CopyFrom.m_pLight;

	return *this;
}
