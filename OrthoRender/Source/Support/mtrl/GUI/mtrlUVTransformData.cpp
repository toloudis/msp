/********************************************************************************************\
**  mtrlUVTransformData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlUVTransformData.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlUVTransformData::mtrlUVTransformData()
:	m_UScale("U Scale", 1),
	m_VScale("V Scale", 1),
	m_UTrans("U Translate", 0),
	m_VTrans("V Translate", 0),
	m_UVAngle("UV Rotation", 0)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlUVTransformData::mtrlUVTransformData(const mtrlUVTransformData& i_Data)
:	m_UScale("U Scale", 1),
	m_VScale("V Scale", 1),
	m_UTrans("U Translate", 0),
	m_VTrans("V Translate", 0),
	m_UVAngle("UV Rotation", 0)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlUVTransformData::~mtrlUVTransformData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlUVTransformData& mtrlUVTransformData::operator=(const mtrlUVTransformData& i_Data)
{
	if (this == &i_Data) return *this;

	m_UScale = i_Data.m_UScale;
	m_VScale = i_Data.m_VScale;
	m_UTrans = i_Data.m_UTrans;
	m_VTrans = i_Data.m_VTrans;
	m_UVAngle = i_Data.m_UVAngle;

	return *this;
}

