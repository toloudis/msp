/********************************************************************************************\
**  lsetScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/LightSets/Data/lsetScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetScriptData::lsetScriptData()
{
}

//-------------------------------------------------
//-------------------------------------------------
lsetScriptData::lsetScriptData(const lsetScriptData& i_Data)
{
	(*this) = i_Data;
}

//-------------------------------------------------
//-------------------------------------------------
lsetScriptData::lsetScriptData(const lsetData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetScriptData::~lsetScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool lsetScriptData::operator == (const lsetScriptData& i_Item)
{
	return (m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetScriptData& lsetScriptData::operator=(const lsetScriptData& i_Data)
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
void lsetLightSetsData::Clear()
{
	m_Items.clear();
}
