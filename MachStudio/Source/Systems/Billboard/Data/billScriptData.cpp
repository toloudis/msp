/****************************************************************************\
**  billScriptData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Data/billScriptData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//
//		billScriptData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptData::billScriptData()
: m_BaseData()
{
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
billScriptData::billScriptData(const billScriptData& i_Data )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptData::billScriptData(const billData& i_BaseData)
: m_BaseData(i_BaseData)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptData::billScriptData(const itString& i_Filename )
: m_BaseData(i_Filename)
{
	m_BaseData.m_Filename	= i_Filename;
	m_BaseData.m_Scale		= 1.0f;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptData::billScriptData(	const itString& i_Filename,
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
billScriptData::~billScriptData()
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool billScriptData::operator == (const billScriptData& i_Item)
{
	return (this->m_BaseData == i_Item.m_BaseData);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
billScriptData& billScriptData::operator = (const billScriptData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BaseData		= i_Data.m_BaseData;
	this->m_ConnectionData		= i_Data.m_ConnectionData;

	xtraPropertyData::CloneProperties(i_Data.m_CustomProperties, this->m_CustomProperties);
	tmlnDriverInfo::CloneDrivers(i_Data.m_Drivers, this->m_Drivers);

	this->m_ChannelInfo	= i_Data.m_ChannelInfo;

	return *this;
}


//
//		billListData
//

void billListData::Add(	const itString& i_Filename,
						const maPoint3d& i_Position,
						float i_Scale )
{
	m_Items.push_back(billScriptData(	i_Filename,
												i_Position,
												i_Scale ));
}

void billListData::Clear()
{
	m_Items.clear();
}
