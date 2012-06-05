/*****************************************************************************
**	tmlnDriverInfo.cpp
**
**	Base class for parsing data structures for drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriverInfo.hpp"
#include "Core/Env/envSTLHelpers.hpp"

//--------------------------------------------------------------------
// Assigns output list with clones of drivers in input list.
//--------------------------------------------------------------------
void tmlnDriverInfo::CloneDrivers(const std::vector<tmlnDriverInfo*> &i_Drivers,
								  std::vector<tmlnDriverInfo*> &o_Drivers)
{
	envSTLHelpers::DeleteContainer(o_Drivers);
	const int num_drivers = i_Drivers.size();
	o_Drivers.resize(num_drivers);
	for (int i=0; i<num_drivers; i++)
	{
		o_Drivers[i] = i_Drivers[i]->Clone();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverInfo::tmlnDriverInfo(chDefs::Name i_ChunkName)
: m_BaseChunkName(i_ChunkName), m_Name(""), m_BeginTime(0.0f), m_EndTime(0.0f),
	m_BlendType(0), m_BlendTime(0.0f), 
	m_EaseInWeight(1.0f), m_EaseOutWeight(1.0f), 
	m_bRestoreOriginal(false),
	m_DriverId(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverInfo::~tmlnDriverInfo()
{

}
