/********************************************************************************************\
**  prjltScriptData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PrjLt/Data/prjltScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptData::prjltScriptData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptData::prjltScriptData(const prjltScriptData& i_Data)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptData::prjltScriptData(const prjltData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptData::~prjltScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool prjltScriptData::operator == (const prjltScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltScriptData& prjltScriptData::operator=(const prjltScriptData& i_Data)
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
void prjltProjectLightsData::Clear()
{
	m_Items.clear();
}

