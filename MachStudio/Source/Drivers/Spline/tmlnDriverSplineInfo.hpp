/********************************************************************************************\
**  tmlnDriverSplineInfo.hpp
**
**	Data structure for parsing spline drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERSPLINEINFO_HPP
#error tmlnDriverSplineInfo.hpp multiply included
#endif
#define TMLN_DRIVERSPLINEINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef MA_POINT3D_HPP
#include "Core/ma/maPoint3d.hpp"
#endif

#include <vector>


class tmlnSplineInfo
{
public:
	std::vector<maPoint3d> m_Points;

};

class tmlnDriverSplineInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverSplineInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverSplineInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	tmlnSplineInfo m_SplineInfo;

};

