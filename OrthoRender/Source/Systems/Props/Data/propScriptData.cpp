/****************************************************************************\
**  propScriptData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Data/propScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//
//		propScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propScriptData::propScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
propScriptData::propScriptData(const propScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propScriptData::propScriptData(const itString& i_Filename )
: m_BaseData(i_Filename)
{
	m_BaseData.m_Filename = i_Filename;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
propScriptData::propScriptData(const propData& i_BaseData )
: m_BaseData(i_BaseData)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propScriptData::propScriptData( const itString& i_Filename,
								const maPoint3d& i_Position,
								const maRotation& i_Orientation )
: m_BaseData(i_Filename, i_Position, i_Orientation)
{
	m_BaseData.m_Filename		= i_Filename;
	m_BaseData.m_Position		= i_Position;
	m_BaseData.m_Orientation	= i_Orientation;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
propScriptData::~propScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool propScriptData::operator == (const propScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propScriptData& propScriptData::operator=(const propScriptData& i_Data)
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

	this->m_ChannelInfo		= i_Data.m_ChannelInfo;
	this->m_Controls		= i_Data.m_Controls;				// FIX: (?)  is this right?  std::vector<dynControlData>
	this->m_Materials		= i_Data.m_Materials;
	this->m_Fragments		= i_Data.m_Fragments;
	this->m_AOData			= i_Data.m_AOData;

	return *this;
}

//
//		propPropsData
//

void propPropsData::Add(const itString& i_Filename,
						const maPoint3d& i_Position,
						const maRotation& i_Orientation )
{
	m_Items.push_back(propScriptData(	i_Filename,
											i_Position,
											i_Orientation ));
}

void propPropsData::Clear()
{
	m_Items.clear();
}
