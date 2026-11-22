/*****************************************************************************
**  effMaskAlphaData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effMaskAlphaData.hpp"


const float effMaskAlphaData::c_DefaultAlphaThreshold = 1.0f/255.0f;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effMaskAlphaData::effMaskAlphaData()
:	m_Color(1.0f, 1.0f, 1.0f, 1.0f),
	m_AlphaThreshold(c_DefaultAlphaThreshold),
	m_TextureTransparencyMap(NULL),
	m_bTransparencyMapAlpha(false),
	m_bDitherTranslucent(false),
	m_DitherAlphaBias(0.25f),
	m_Transparency(1.0f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effMaskAlphaData::~effMaskAlphaData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effMaskAlphaData::effMaskAlphaData(const effMaskAlphaData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effMaskAlphaData& effMaskAlphaData::operator = (const effMaskAlphaData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_Color = i_CopyFrom.m_Color;
	m_AlphaThreshold = i_CopyFrom.m_AlphaThreshold;
	m_TextureTransparencyMap = i_CopyFrom.m_TextureTransparencyMap;
	m_bTransparencyMapAlpha = i_CopyFrom.m_bTransparencyMapAlpha;
	m_bDitherTranslucent = i_CopyFrom.m_bDitherTranslucent;
	m_DitherAlphaBias = i_CopyFrom.m_DitherAlphaBias;
	m_Transparency = i_CopyFrom.m_Transparency;

	return *this;
}
