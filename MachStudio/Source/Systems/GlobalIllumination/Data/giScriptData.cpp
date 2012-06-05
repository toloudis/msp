/****************************************************************************\
**  giScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/Data/giScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		giScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giScriptData::giScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
giScriptData::giScriptData(const giScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giScriptData::giScriptData(const giGIData& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
giScriptData::~giScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool giScriptData::operator == (const giScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
giScriptData& giScriptData::operator = (const giScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;


	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}
