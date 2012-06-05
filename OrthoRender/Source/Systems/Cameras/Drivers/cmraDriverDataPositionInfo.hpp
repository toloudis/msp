/********************************************************************************************\
**  cmraDriverDataPositionInfo.hpp
**
**		Data structure for cmra Position info.
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERDATAPOSITIONINFO_HPP
#error cmraDriverDataPositionInfo.hpp multiply included
#endif
#define CMRA_DRIVERDATAPOSITIONINFO_HPP

#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


//============================================================================
//============================================================================
class cmraDriverDataPositionInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverDataPositionInfo();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverDataPositionInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual cmraDriverDataPositionInfo* Clone();

	//--------------------------------------------------------------------
	//	calculate the position based on the yaw, pitch,
	//	and distance.
	//--------------------------------------------------------------------
	void Calculate_Position( maPoint3d& i_Target );

public:
	float	m_fYawAngle;		// in degrees NOT radians
	float	m_fPitchAngle;
	float	m_fDistance;
	float	m_fDeltaFromStartTime;
	float	m_fBeginTime;

	maPoint3d m_CamPosition;
	maPoint3d m_CamTarget;
};


