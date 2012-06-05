/*****************************************************************************
**	effTexturedData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBillboardData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBillboardData::effBillboardData()
:	m_bCKActive(false),
	m_CKColor(maFloatRGBA(0,0,0,0)),
	m_CKTolerance(0.1f),
	m_bCKRemoveSpill(false),
	m_CKSpillType(0),
	m_CKSpillBias(0.0f),
	m_CKEdgeBlur(0),
	m_Brightness(1.0f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBillboardData::~effBillboardData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBillboardData::effBillboardData(const effBillboardData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBillboardData& effBillboardData::operator = (const effBillboardData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effTexturedData::operator =(i_CopyFrom);
	m_bCKActive = i_CopyFrom.m_bCKActive;
	m_CKColor = i_CopyFrom.m_CKColor;
	m_CKTolerance = i_CopyFrom.m_CKTolerance;
	m_bCKRemoveSpill = i_CopyFrom.m_bCKRemoveSpill;
	m_CKSpillType = i_CopyFrom.m_CKSpillType;
	m_CKSpillBias = i_CopyFrom.m_CKSpillBias;
	m_CKEdgeBlur = i_CopyFrom.m_CKEdgeBlur;
	m_Brightness = i_CopyFrom.m_Brightness;
	return *this;
}
