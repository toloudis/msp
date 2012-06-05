/********************************************************************************************\
**  tmlnDriverSplineInfo.hpp
**
**	Base class for parsing data structures for drivers
**
**  StudioGPU
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/

#ifdef TMLN_DRIVERINFO_HPP
#error tmlnDriverInfo.hpp multiply included
#endif
#define TMLN_DRIVERINFO_HPP

#ifndef TMLN_DRIVERIDMGR_HPP
#include "Support/tmln/tmlnDriverIdMgr.hpp"
#endif
#ifndef CH_DEFS_HPP
#include "Core/ch/chDefs.hpp"
#endif

#include <string>
#include <vector>

//============================================================================
//============================================================================
class tmlnDriverInfo
{
public:
	//--------------------------------------------------------------------
	// Assigns output list with clones of drivers in input list.
	//--------------------------------------------------------------------
	static void CloneDrivers(const std::vector<tmlnDriverInfo*> &i_Drivers,
							 std::vector<tmlnDriverInfo*> &o_Drivers);

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

	//--------------------------------------------------------------------
	//  GetBaseChunkName returns the parse type of this object as 
	//	set on construction
	//--------------------------------------------------------------------
	inline chDefs::Name GetBaseChunkName() const;
	inline void SetBaseChunkName(chDefs::Name i_Name);

public:
	chDefs::Name m_BaseChunkName;
	std::string m_Name;
	float m_BeginTime;
	float m_EndTime;

	int m_BlendType;
	float m_BlendTime;
	bool m_bRestoreOriginal;

	float m_EaseInWeight;
	float m_EaseOutWeight;

	std::string m_ChannelName;

public:
	// This data is not persistent and should not be written to file
	tmlnDriverId m_DriverId;
};

//--------------------------------------------------------------------
//  GetChunkName returns the parse type of this object 
//	as set on construction
//--------------------------------------------------------------------
inline chDefs::Name tmlnDriverInfo::GetBaseChunkName() const
{
	return m_BaseChunkName;
}
inline void tmlnDriverInfo::SetBaseChunkName(chDefs::Name i_Name)
{
	m_BaseChunkName = i_Name;
}

