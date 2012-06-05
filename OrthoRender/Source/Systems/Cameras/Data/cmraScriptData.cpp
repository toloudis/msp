/********************************************************************************************\
**  cmraCamerasData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Cameras/Data/cmraScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptData::cmraScriptData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptData::cmraScriptData(const cmraCameraData& i_Data)
: m_BaseData(i_Data)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptData::cmraScriptData(const cmraScriptData& i_Data)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptData::~cmraScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool cmraScriptData::operator == (const cmraScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraScriptData& cmraScriptData::operator=(const cmraScriptData& i_Data)
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
//cmraCueFormData::cmraCueFormData()
//: m_Cues(0)
//{
//	m_Index[0] = -1;
//	m_Index[1] = -1;
//	m_Index[2] = -1;
//	m_Index[3] = -1;
//}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
cmraCamerasData::cmraCamerasData()
{

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void cmraCamerasData::Clear()
{
	m_Items.clear();
}
