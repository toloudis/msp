/********************************************************************************************\
**  mtrlOutlineData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlOutlineData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlOutlineData::mtrlOutlineData()
:	m_OutlineDepthScale("Depth Scale", 0.1f ),
	m_OutlineThreshold("Threshold", 1.0f ),
	m_OutlineThickness("Thickness", 1.0f ),
	m_OutlineColor("Color", maFloatRGBA(0,0,0,1)),
	m_bUseDepths("Use Depths", true),
	m_bUseNormals("Use Normals", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlOutlineData::mtrlOutlineData(const mtrlOutlineData& i_Data)
:	m_OutlineDepthScale("Depth Scale", 0.1f ),
	m_OutlineThreshold("Threshold", 1.0f ),
	m_OutlineThickness("Thickness", 1.0f ),
	m_OutlineColor("Color", maFloatRGBA(0,0,0,1)),
	m_bUseDepths("Use Depths", true),
	m_bUseNormals("Use Normals",false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlOutlineData::~mtrlOutlineData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlOutlineData& mtrlOutlineData::operator=(const mtrlOutlineData& i_Data)
{
	if (this == &i_Data) return *this;

	m_OutlineDepthScale = i_Data.m_OutlineDepthScale;
	m_OutlineThreshold = i_Data.m_OutlineThreshold;
	m_OutlineThickness = i_Data.m_OutlineThickness;
	m_OutlineColor = i_Data.m_OutlineColor;
	m_bUseDepths = i_Data.m_bUseDepths;
	m_bUseNormals = i_Data.m_bUseNormals;

	return *this;
}

