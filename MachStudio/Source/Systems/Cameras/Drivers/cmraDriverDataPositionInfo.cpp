/*****************************************************************************
**	cmraDriverDataPositionInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverDataPositionInfo.hpp"

#include "Core/ma/maConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataPositionInfo::cmraDriverDataPositionInfo()
:	m_fYawAngle( 0.0f ),
	m_fPitchAngle( 0.0f ),
	m_fDistance( 0.0f ),
	m_fDeltaFromStartTime( 0.0f ),
	m_CamPosition( 0.0f, 0.0f, 0.0f ),
	m_CamTarget( 0.0f, 0.0f, 0.0f )
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataPositionInfo::~cmraDriverDataPositionInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
cmraDriverDataPositionInfo* cmraDriverDataPositionInfo::Clone()
{
	return new cmraDriverDataPositionInfo(*this);
}

//--------------------------------------------------------------------
//	calculate the position based on the yaw, pitch,
//	and distance.
//--------------------------------------------------------------------
void cmraDriverDataPositionInfo::Calculate_Position( maPoint3d& i_Target )
{
	// convert pitch + yaw to dir
	//
	float yaw, pitch;
	yaw		= m_fYawAngle * maConstants::c_fAngleToRad;
	pitch	= m_fPitchAngle * maConstants::c_fAngleToRad;

	float x = float(sin(yaw) * cos(pitch));
	float y = float(sin(pitch));
	float z = float(cos(yaw) * cos(pitch));
	maVector3d dir(x,y,z);

	//DBG_LOG3( "dir (%6.3f,%6.3f,%6.3f)", x,y,z );
	//DBG_LOG1( "     dist = %6.3f", m_fDistance );

	//	set the position
	//
	m_CamTarget.Set( i_Target.GetX(), i_Target.GetY(), i_Target.GetZ() );
	m_CamPosition	= m_CamTarget + dir * m_fDistance;

	//DBG_LOG3( "tar (%6.3f,%6.3f,%6.3f)", i_Target.GetX(), i_Target.GetY(), i_Target.GetZ() );
	//DBG_LOG4( " dir(%6.3f,%6.3f,%6.3f) dist %6.3f", dir.GetX(), dir.GetY(), dir.GetZ(), m_fDistance );
	//DBG_LOG3( "pos (%6.3f,%6.3f,%6.3f)", m_CamPosition.GetX(), m_CamPosition.GetY(), m_CamPosition.GetZ() );
}
