/****************************************************************************\
**  prtclScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Data/prtclScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		prtclScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclScriptData::prtclScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
prtclScriptData::prtclScriptData(const prtclScriptData& i_Data )
{
	(*this) = i_Data;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
prtclScriptData::prtclScriptData(const prtclData& i_BaseData)
: m_BaseData(i_BaseData)
{
}
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclScriptData::prtclScriptData(const itString& i_Filename )
: m_BaseData(i_Filename)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclScriptData::prtclScriptData(	const itString& i_Filename,
									const maPoint3d& i_Position,
									const maRotation& i_Orientation )
: m_BaseData(i_Filename, i_Position, i_Orientation)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
prtclScriptData::~prtclScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool prtclScriptData::operator == (const prtclScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclScriptData& prtclScriptData::operator=(const prtclScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_BaseData		= i_Data.m_BaseData;

	envSTLHelpers::DeleteContainer(this->m_Drivers);
	int num_drivers = i_Data.m_Drivers.size();
	this->m_Drivers.resize(num_drivers);
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers[i] = i_Data.m_Drivers[i]->Clone();
	}

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}


//
//		prtclParticlesData
//

void prtclParticlesData::Add(	const itString& i_Filename,
								const maPoint3d& i_Position,
								const maRotation& i_Orientation )
{
	m_Items.push_back(prtclScriptData(	i_Filename,
												i_Position,
												i_Orientation ));
}

void prtclParticlesData::Clear()
{
	m_Items.clear();
}
