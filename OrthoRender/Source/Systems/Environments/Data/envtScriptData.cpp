/********************************************************************************************\
**  envtScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Environments/Data/envtScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptData::envtScriptData()
{
}

//-------------------------------------------------
//-------------------------------------------------
envtScriptData::envtScriptData(const envtScriptData& i_Data)
{
	(*this) = i_Data;
}

//-------------------------------------------------
//-------------------------------------------------
envtScriptData::envtScriptData(const envtData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptData::~envtScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool envtScriptData::operator == (const envtScriptData& i_Item)
{
	return (m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtScriptData& envtScriptData::operator=(const envtScriptData& i_Data)
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

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtEnvironmentsData::Clear()
{
	m_Items.clear();
}
