/********************************************************************************************\
**  cmraDriverCaptureInfo.hpp
**
**	Data structure for parsing capture drivers
**
**  StudioGPU
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef CMRA_DRIVERCAPTUREINFO_HPP
#error cmraDriverCaptureInfo.hpp multiply included
#endif
#define CMRA_DRIVERCAPTUREINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif


//============================================================================
//============================================================================
class cmraCaptureInfo
{
public:
	bool m_bActive;
};

//============================================================================
//============================================================================
class cmraDriverCaptureInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	cmraDriverCaptureInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~cmraDriverCaptureInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	cmraCaptureInfo m_Info;
};

