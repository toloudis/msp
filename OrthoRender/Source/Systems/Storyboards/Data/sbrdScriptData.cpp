/****************************************************************************\
**  sbrdScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Data/sbrdScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		sbrdScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptData::sbrdScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
sbrdScriptData::sbrdScriptData(const sbrdScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptData::sbrdScriptData(const sbrdObjectData& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptData::sbrdScriptData(const itString& i_Filename )
: m_BaseData(i_Filename)
{
	m_BaseData.m_Filename	= i_Filename;
	m_BaseData.m_Scale		= 1.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptData::sbrdScriptData(	const itString& i_Filename,
								const maPoint3d& i_Position,
								float i_Scale )
: m_BaseData(i_Filename, i_Position, i_Scale)
{
	m_BaseData.m_Filename	= i_Filename;
	m_BaseData.m_Position	= i_Position;
	m_BaseData.m_Scale		= i_Scale;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
sbrdScriptData::~sbrdScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool sbrdScriptData::operator == (const sbrdScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
sbrdScriptData& sbrdScriptData::operator = (const sbrdScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;

	// drivers
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


//
//		sbrdObjectsData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void sbrdObjectsData::Add(	const itString& i_Filename,
							const maPoint3d& i_Position,
							float i_Scale )
{
	m_Items.push_back(sbrdScriptData(	i_Filename,
										i_Position,
										i_Scale ));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void sbrdObjectsData::Clear()
{
	m_Items.clear();
}
