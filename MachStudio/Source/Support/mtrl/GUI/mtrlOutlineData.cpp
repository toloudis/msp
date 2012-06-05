/********************************************************************************************\
**  mtrlOutlineData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlOutlineData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlOutlineData::mtrlOutlineData()
:	m_OutlineDepthScale("Depth Scale", 0.1f ),
	m_OutlineMinAngle("Min Angle", 90.0f ),
	m_OutlineMaxAngle("Max Angle", 90.0f ),
	m_OutlineThickness("Thickness", 1.0f ),
	m_OutlineMinWidth("Min Width", 1.0f ),
	m_OutlineMaxWidth("Max Width", 1.0f ),
	m_OutlineColor("Color", maFloatRGBA(0,0,0,1)),
	m_bUseDepths("Enable Silhouette", true),
	m_bUseNormals("Enable Interior", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlOutlineData::mtrlOutlineData(const mtrlOutlineData& i_Data)
:	m_OutlineDepthScale("Depth Scale", 0.1f ),
	m_OutlineMinAngle("Min Angle", 90.0f ),
	m_OutlineMaxAngle("Max Angle", 90.0f ),
	m_OutlineThickness("Thickness", 1.0f ),
	m_OutlineMinWidth("Min Width", 1.0f ),
	m_OutlineMaxWidth("Max Width", 1.0f ),
	m_OutlineColor("Color", maFloatRGBA(0,0,0,1)),
	m_bUseDepths("Enable Silhouette", true),
	m_bUseNormals("Enable Interior",false)
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
	m_OutlineMinAngle = i_Data.m_OutlineMinAngle;
	m_OutlineMaxAngle = i_Data.m_OutlineMaxAngle;
	m_OutlineThickness = i_Data.m_OutlineThickness;
	m_OutlineMinWidth = i_Data.m_OutlineMinWidth;
	m_OutlineMaxWidth = i_Data.m_OutlineMaxWidth;
	m_OutlineColor = i_Data.m_OutlineColor;
	m_bUseDepths = i_Data.m_bUseDepths;
	m_bUseNormals = i_Data.m_bUseNormals;

	return *this;
}

