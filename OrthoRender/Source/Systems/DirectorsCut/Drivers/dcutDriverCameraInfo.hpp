/********************************************************************************************\
**  dcutDriverCameraInfo.hpp
**
**	Data structure for parsing capture drivers
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#ifdef DCUT_DRIVERCAMERAINFO_HPP
#error dcutDriverCameraInfo.hpp multiply included
#endif
#define DCUT_DRIVERCAMERAINFO_HPP

#ifndef TMLN_DRIVERINFO_HPP
#include "Support/tmln/tmlnDriverInfo.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif


//============================================================================
//============================================================================
class dcutDriverCameraInfo : public tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	dcutDriverCameraInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~dcutDriverCameraInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone();

	nameString m_CameraName;
};

