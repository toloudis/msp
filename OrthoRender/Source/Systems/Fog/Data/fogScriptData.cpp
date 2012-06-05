/****************************************************************************\
**  fogScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Data/fogScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		fogScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptData::fogScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
fogScriptData::fogScriptData(const fogScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptData::fogScriptData(const fogFogData& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
fogScriptData::~fogScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool fogScriptData::operator == (const fogScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
fogScriptData& fogScriptData::operator = (const fogScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;

	// drivers
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
