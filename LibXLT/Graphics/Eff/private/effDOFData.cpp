/*****************************************************************************
**	effDOFData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effDOFData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDOFData::effDOFData()
:	m_pSharpTexture(NULL),
	m_pBlurryTexture(NULL),
	m_FilterWidthX(1.f/640.f),
	m_FilterWidthY(1.f/480.f),
	m_MaxCoC(5.0f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDOFData::~effDOFData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDOFData::effDOFData(const effDOFData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDOFData& effDOFData::operator = (const effDOFData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_pSharpTexture = i_CopyFrom.m_pSharpTexture;
	m_pBlurryTexture = i_CopyFrom.m_pBlurryTexture;
	m_FilterWidthX = i_CopyFrom.m_FilterWidthX;
	m_FilterWidthY = i_CopyFrom.m_FilterWidthY;
	m_MaxCoC = i_CopyFrom.m_MaxCoC;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effDOFData::operator == (const effDOFData& i_EffDOFData) const
{
	return (m_pSharpTexture == i_EffDOFData.m_pSharpTexture) &&
		(m_pBlurryTexture == i_EffDOFData.m_pBlurryTexture) &&
		(m_FilterWidthX == i_EffDOFData.m_FilterWidthX) &&
		(m_FilterWidthY == i_EffDOFData.m_FilterWidthY) &&
		(m_MaxCoC == i_EffDOFData.m_MaxCoC);
}
