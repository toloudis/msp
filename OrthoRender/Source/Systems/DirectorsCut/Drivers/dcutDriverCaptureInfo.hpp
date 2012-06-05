/********************************************************************************************\
**  dcutDriverCaptureInfo.hpp
**
**	Data structure for parsing capture drivers
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef DCUT_DRIVERCAPTUREINFO_HPP
#error dcutDriverCaptureInfo.hpp multiply included
#endif
#define DCUT_DRIVERCAPTUREINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif


//============================================================================
//============================================================================
class dcutCaptureInfo
{
public:
	bool m_bActive;
};

//============================================================================
//============================================================================
class dcutDriverCaptureInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutDriverCaptureInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dcutDriverCaptureInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	dcutCaptureInfo m_Info;
};

