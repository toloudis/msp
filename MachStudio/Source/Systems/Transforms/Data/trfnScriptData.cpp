/****************************************************************************\
**  trfnScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Data/trfnScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		trfnScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnScriptData::trfnScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
trfnScriptData::trfnScriptData(const trfnScriptData& i_Data )
{
	(*this) = i_Data;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
trfnScriptData::trfnScriptData(const trfnData& i_BaseData )
: m_BaseData(i_BaseData)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
trfnScriptData::~trfnScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool trfnScriptData::operator == (const trfnScriptData& i_Item)
{
	return (m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnScriptData& trfnScriptData::operator=(const trfnScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_BaseData		= i_Data.m_BaseData;
	this->m_ConnectionData		= i_Data.m_ConnectionData;

	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo		= i_Data.m_ChannelInfo;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void trfnTransformsData::Clear()
{
	m_Items.clear();
}
