/********************************************************************************************\
**  envtScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
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

	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}

//------------------------------------------------------------------------
// always init the list with one default environment.
//------------------------------------------------------------------------
envtEnvironmentsData::envtEnvironmentsData() 
{
//	m_DefaultEnv.m_Name.SetValue(evmtEnvironmentMgr::GetDefaultEnvironmentName());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void envtEnvironmentsData::Clear()
{
	m_Items.clear();
	m_DefaultEnv = envtScriptData();
	m_SwlEnv = envtScriptData();
}
