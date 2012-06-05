/*****************************************************************************
**	tmlnDriverOrientationInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Orientation/tmlnDriverOrientationInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverOrientationInfo::tmlnDriverOrientationInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), 
	m_X(0), m_Y(0), m_Z(0),
	m_bEulerInterpolation(false)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverOrientationInfo::~tmlnDriverOrientationInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverOrientationInfo::Clone()
{
	return new tmlnDriverOrientationInfo(*this);
}
