/*****************************************************************************
**	effBlurData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBlurData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlurData::effBlurData()
:	m_pSceneTexture(NULL),
	m_pDownsampledTexture(NULL),
	m_pHorizontalBlurTexture(NULL),
	m_FilterWidthX(1.f/640.f),
	m_FilterWidthY(1.f/480.f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlurData::~effBlurData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlurData::effBlurData(const effBlurData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlurData& effBlurData::operator = (const effBlurData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_pSceneTexture = i_CopyFrom.m_pSceneTexture;
	m_pDownsampledTexture = i_CopyFrom.m_pDownsampledTexture;
	m_pHorizontalBlurTexture = i_CopyFrom.m_pHorizontalBlurTexture;
	m_FilterWidthX = i_CopyFrom.m_FilterWidthX;
	m_FilterWidthY = i_CopyFrom.m_FilterWidthY;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effBlurData::operator == (const effBlurData& i_EffBlurData) const
{
	return (m_pSceneTexture == i_EffBlurData.m_pSceneTexture) &&
		(m_pDownsampledTexture == i_EffBlurData.m_pDownsampledTexture) &&
		(m_pHorizontalBlurTexture == i_EffBlurData.m_pHorizontalBlurTexture) &&
		(m_FilterWidthX == i_EffBlurData.m_FilterWidthX) &&
		(m_FilterWidthY == i_EffBlurData.m_FilterWidthY);
}
