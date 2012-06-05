/********************************************************************************************\
**  cmraDriverKeyTargetInfo.hpp
**
**	Data structure for parsing drivers
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERKEYTARGETINFO_HPP
#error cmraDriverKeyTargetInfo.hpp multiply included
#endif
#define CMRA_DRIVERKEYTARGETINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif


class cmraDriverKeyTargetInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverKeyTargetInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverKeyTargetInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	maPoint3d	m_CameraKeyTarget;
};


