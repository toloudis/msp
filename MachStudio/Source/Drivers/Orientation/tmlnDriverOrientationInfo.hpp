/********************************************************************************************\
**  tmlnDriverOrientationInfo.hpp
**
**	Data structure for parsing sound drivers
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERORIENTATIONINFO_HPP
#error tmlnDriverOrientationInfo.hpp multiply included
#endif
#define TMLN_DRIVERORIENTATIONINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
//#ifndef MA_ROTATION_HPP
//#include "Core/ma/maRotation.hpp"
//#endif


class tmlnDriverOrientationInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverOrientationInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverOrientationInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	float m_X, m_Y, m_Z; // Euler angles
	bool m_bEulerInterpolation;
};

