/****************************************************************************\
**  chtrScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Data/chtrScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		chtrScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrScriptData::chtrScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrScriptData::chtrScriptData(const chtrScriptData& i_Data )
{
	(*this) = i_Data;
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrScriptData::chtrScriptData(const chtrData& i_BaseData )
: m_BaseData(i_BaseData)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrScriptData::chtrScriptData(const itString& i_Filename )
: m_BaseData(i_Filename)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrScriptData::chtrScriptData(	const itString& i_Filename,
								const maPoint3d& i_Position,
								const maRotation& i_Orientation )
: m_BaseData(i_Filename, i_Position, i_Orientation)
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
chtrScriptData::~chtrScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
	envSTLHelpers::DeleteContainer(this->m_Expressions);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool chtrScriptData::operator == (const chtrScriptData& i_Item)
{
	return (m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrScriptData& chtrScriptData::operator=(const chtrScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_BaseData		= i_Data.m_BaseData;
	this->m_ConnectionData		= i_Data.m_ConnectionData;

	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo		= i_Data.m_ChannelInfo;
	this->m_Controls		= i_Data.m_Controls;
	this->m_Materials		= i_Data.m_Materials;
	this->m_Fragments		= i_Data.m_Fragments;
	this->m_AOData			= i_Data.m_AOData;

	envSTLHelpers::DeleteContainer(this->m_Expressions);
	int num_expressions = i_Data.m_Expressions.size();
	this->m_Expressions.resize(num_expressions);
	for (int i=0; i<num_expressions; ++i)
	{
		this->m_Expressions[i] = i_Data.m_Expressions[i]->Clone();
	}

	return *this;
}

//----------------------------------------------------------------------------
//		chtrCharactersData
//----------------------------------------------------------------------------
void chtrCharactersData::Add(	const itString& i_Filename,
								const maPoint3d& i_Position,
								const maRotation& i_Orientation )
{
	m_Items.push_back(chtrScriptData( i_Filename,
									  i_Position,
									  i_Orientation ));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void chtrCharactersData::Clear()
{
	m_Items.clear();
}
