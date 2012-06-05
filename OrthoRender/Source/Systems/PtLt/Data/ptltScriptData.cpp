/********************************************************************************************\
**  ptltScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PtLt/Data/ptltScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltScriptData::ptltScriptData()
{
}

//-------------------------------------------------
//-------------------------------------------------
ptltScriptData::ptltScriptData(const ptltScriptData& i_Data)
{
	(*this) = i_Data;
}

//-------------------------------------------------
//-------------------------------------------------
ptltScriptData::ptltScriptData(const ptltData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltScriptData::~ptltScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool ptltScriptData::operator == (const ptltScriptData& i_Item)
{
	return (m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltScriptData& ptltScriptData::operator=(const ptltScriptData& i_Data)
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
void ptltPointLightsData::Clear()
{
	m_Items.clear();
}
