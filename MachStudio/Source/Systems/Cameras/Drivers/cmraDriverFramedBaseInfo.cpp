/*****************************************************************************
**	cmraDriverFramedBaseInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedBaseInfo::cmraDriverFramedBaseInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo( i_ChunkName ),
	m_bPositionRestrict(false),
	m_PositionTolerance(0.0f,0.0f,0.0f),
	m_bAngleRestrict(false),
	m_AngleTolerance(0.0f,0.0f,0.0f),
	m_bDistanceRestrict(false),
	m_DistanceTolerance(0.0f,0.0f,0.0f),
	m_bDirectionRestrict(false),
	m_fDirectionTolerance(0.0f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFramedBaseInfo::~cmraDriverFramedBaseInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFramedBaseInfo::Clone()
{
	return new cmraDriverFramedBaseInfo(*this);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmraDriverFramedBaseInfo::display_values() const
{
	DBG_LOG( "Pos = " << (m_bPositionRestrict?"true":"false") );
	DBG_LOG( "Pos   ( " << m_PositionTolerance.GetX() << " , " << m_PositionTolerance.GetY() << " , " << m_PositionTolerance.GetZ() << " )" );
	DBG_LOG( "Ang = " << (m_bAngleRestrict?"true":"false") );
	DBG_LOG( "Ang   ( " << m_AngleTolerance.GetX() << " , "  << m_AngleTolerance.GetY() << " , " << m_AngleTolerance.GetZ() << " )" );
	DBG_LOG( "Dis = " << (m_bDistanceRestrict?"true":"false") );
	DBG_LOG( "Dis   ( " << m_DistanceTolerance.GetX() << " , " << m_DistanceTolerance.GetY() << " , " << m_DistanceTolerance.GetZ() << " )" );
	DBG_LOG( "Dir = " << (m_bDirectionRestrict?"true":"false") );
	DBG_LOG( "Dir   ( " << m_fDirectionTolerance << " )" );
}
