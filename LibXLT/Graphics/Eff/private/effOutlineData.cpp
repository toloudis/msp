/*****************************************************************************
**  effOutlineData.cpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effOutlineData.hpp"

#include "Graphics/eff/effOutlineDataParser.hpp"

#include "Core/fs/fsResourceFinder.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOutlineData::effOutlineData()
:	m_pTexture(NULL),
	m_OutlineDepthScale(0.1f),
	m_OutlineMinAngle(90.0f),
	m_OutlineMaxAngle(90.0f),
	m_OutlineThickness(1.0f),
	m_OutlineMinWidth(1.0f),
	m_OutlineMaxWidth(1.0f),
	m_OutlineColor(0,0,0,0),
	m_OutlineViewSize(1,1),
	m_bUseDepths(true),
	m_bUseNormals(false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOutlineData::~effOutlineData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOutlineData::effOutlineData(const effOutlineData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOutlineData& effOutlineData::operator = (const effOutlineData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_pTexture = i_CopyFrom.m_pTexture ;
	m_OutlineDepthScale = i_CopyFrom.m_OutlineDepthScale;
	m_OutlineMinAngle = i_CopyFrom.m_OutlineMinAngle;
	m_OutlineMaxAngle = i_CopyFrom.m_OutlineMaxAngle;
	m_OutlineThickness = i_CopyFrom.m_OutlineThickness;
	m_OutlineMinWidth = i_CopyFrom.m_OutlineMinWidth;
	m_OutlineMaxWidth = i_CopyFrom.m_OutlineMaxWidth;
	m_OutlineColor = i_CopyFrom.m_OutlineColor;
	m_OutlineViewSize = i_CopyFrom.m_OutlineViewSize;
	m_bUseDepths = i_CopyFrom.m_bUseDepths;
	m_bUseNormals = i_CopyFrom.m_bUseNormals;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effOutlineData::operator == (const effOutlineData& i_EffOutlineData) const
{
	return (m_pTexture == i_EffOutlineData.m_pTexture) &&
		(m_OutlineDepthScale == i_EffOutlineData.m_OutlineDepthScale) &&
		(m_OutlineMinAngle == i_EffOutlineData.m_OutlineMinAngle) &&
		(m_OutlineMaxAngle == i_EffOutlineData.m_OutlineMaxAngle) &&
		(m_OutlineThickness == i_EffOutlineData.m_OutlineThickness) &&
		(m_OutlineMinWidth == i_EffOutlineData.m_OutlineMinWidth) &&
		(m_OutlineMaxWidth == i_EffOutlineData.m_OutlineMaxWidth) &&
		(m_OutlineColor == i_EffOutlineData.m_OutlineColor) &&
		(m_OutlineViewSize == i_EffOutlineData.m_OutlineViewSize) &&
		(m_bUseDepths == i_EffOutlineData.m_bUseDepths) &&
		(m_bUseNormals == i_EffOutlineData.m_bUseNormals);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOutlineData::RemoveTextures()
{
	m_pTexture = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effOutlineData::GetChunkName() const
{
	return effOutlineDataParser::GetChunkName();
}
