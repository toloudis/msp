/********************************************************************************************\
**  dcutCuesData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#include "Systems/DirectorsCut/Data/dcutScriptData.hpp"

#include "Support/tmln/tmlnChannelInfo.hpp"
#include "Support/tmln/tmlnDriverInfo.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptData::dcutScriptData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptData::dcutScriptData(const dcutScriptData& i_Data)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptData::dcutScriptData(const dcutCueData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptData::~dcutScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool dcutScriptData::operator == (const dcutScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutScriptData& dcutScriptData::operator=(const dcutScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_BaseData = i_Data.m_BaseData;

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
dcutCueFormData::dcutCueFormData()
{
	m_Index[0] = -1;
	m_Index[1] = -1;
	m_Index[2] = -1;
	m_Index[3] = -1;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dcutCuesData::dcutCuesData()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void dcutCuesData::Clear()
{
	m_Items.clear();
}
