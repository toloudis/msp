/****************************************************************************\
**  aoScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Data/aoScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		aoScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptData::aoScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
aoScriptData::aoScriptData(const aoScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptData::aoScriptData(const aoAOData& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
aoScriptData::~aoScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool aoScriptData::operator == (const aoScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
aoScriptData& aoScriptData::operator = (const aoScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;


	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}
