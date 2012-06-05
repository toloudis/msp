/********************************************************************************************\
**  cmraDriverCircleInfo.hpp
**
**	Data structure for camera driver Circle
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/

#ifdef CMRA_DRIVERCIRCLEINFO_HPP
#error cmraDriverCircleInfo.hpp multiply included
#endif
#define CMRA_DRIVERCIRCLEINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef IT_STRING_HPP
#include "Core/it/itString.hpp"
#endif


//============================================================================
//============================================================================
class cmraDriverCircleInfo : public tmlnDriverInfo
{
public:
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	cmraDriverCircleInfo(chDefs::Name i_ChunkName);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	virtual ~cmraDriverCircleInfo();

	//------------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//------------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

public:
	float	m_Revolutions;
	bool	m_bClockwise;
	float	m_fStartingYaw;
	float	m_fStartingPitch;
	float	m_fRadius;
};

