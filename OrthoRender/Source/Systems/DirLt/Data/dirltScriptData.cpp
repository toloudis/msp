/********************************************************************************************\
**  dirltScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "dirltScriptData.hpp"

#include "tmlnDriverInfo.hpp"

#include "envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptData::dirltScriptData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptData::dirltScriptData(const dirltScriptData& i_Data)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptData::~dirltScriptData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool dirltScriptData::operator == (const dirltScriptData& i_Item)
{
	return (this->m_BaseData.m_Name == i_Item.m_BaseData.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltScriptData& dirltScriptData::operator=(const dirltScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;

	envSTLHelpers::DeleteContainer(this->m_Drivers);
	int num_drivers = i_Data.m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers.push_back(i_Data.m_Drivers[i]->Clone());
	}

	envSTLHelpers::DeleteContainer(this->m_Channels);
	int num_channels = i_Data.m_Channels.size();
	for (int i=0; i<num_channels; i++)
	{
		this->m_Channels.push_back(i_Data.m_Channels[i]->Clone());
	}

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void dirltDirLightsData::Clear()
{
	m_Items.clear();
}
