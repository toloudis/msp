/********************************************************************************************\
**  tmlnDriverSplineInfo.hpp
**
**	Base class for parsing data structures for drivers
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERINFO_HPP
#error tmlnDriverInfo.hpp multiply included
#endif
#define TMLN_DRIVERINFO_HPP

#ifndef TMLN_DRIVERIDMGR_HPP
#include "Support/tmln/tmlnDriverIdMgr.hpp"
#endif
#ifndef CH_PARSABLE_HPP
#include "Core/ch/chParsable.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class tmlnDriverInfo : public chParsable
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	tmlnDriverInfo(chDefs::Name i_ChunkName);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~tmlnDriverInfo();

	//--------------------------------------------------------------------
	// Return pointer to new equivalent driver info structure
	//--------------------------------------------------------------------
	virtual tmlnDriverInfo* Clone() = 0;

public:
	std::string m_Name;
	float m_BeginTime;
	float m_EndTime;

	int m_BlendType;
	float m_BlendTime;
	bool m_bRestoreOriginal;

	float m_EaseInWeight;
	float m_EaseOutWeight;

public:
	// This data is not persistent and should not be written to file
	tmlnDriverId m_DriverId;
};
