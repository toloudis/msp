/********************************************************************************************\
**  cmraDriverKeyPositionAndTargetInfo.hpp
**
**	Data structure for parsing drivers
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERKEYPOSITIONANDTARGETINFO_HPP
#error cmraDriverKeyPositionAndTargetInfo.hpp multiply included
#endif
#define CMRA_DRIVERKEYPOSITIONANDTARGETINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


class cmraDriverKeyPositionAndTargetInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverKeyPositionAndTargetInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverKeyPositionAndTargetInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	maPoint3d	m_CameraKeyPosition;
	maPoint3d	m_CameraKeyTarget;
};


