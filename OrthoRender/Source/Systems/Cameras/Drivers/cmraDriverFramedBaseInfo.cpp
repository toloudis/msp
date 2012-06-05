/*****************************************************************************
**	cmraDriverFramedBaseInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverFramedBaseInfo.hpp"

#include "Core/dbg/dbgLog.hpp"


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
	DBG_LOG1( "Pos = %s", m_bPositionRestrict?"true":"false" );
	DBG_LOG3( "Pos   ( %6.3f, %6.3f, %6.3f )", m_PositionTolerance.GetX(), m_PositionTolerance.GetY(), m_PositionTolerance.GetZ() );
	DBG_LOG1( "Ang = %s", m_bAngleRestrict?"true":"false" );
	DBG_LOG3( "Ang   ( %6.3f, %6.3f, %6.3f )", m_AngleTolerance.GetX(), m_AngleTolerance.GetY(), m_AngleTolerance.GetZ() );
	DBG_LOG1( "Dis = %s", m_bDistanceRestrict?"true":"false" );
	DBG_LOG3( "Dis   ( %6.3f, %6.3f, %6.3f )", m_DistanceTolerance.GetX(), m_DistanceTolerance.GetY(), m_DistanceTolerance.GetZ() );
	DBG_LOG1( "Dir = %s", m_bDirectionRestrict?"true":"false" );
	DBG_LOG1( "Dir   ( %6.3f )", m_fDirectionTolerance );
}
