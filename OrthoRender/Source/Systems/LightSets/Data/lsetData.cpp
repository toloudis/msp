/********************************************************************************************\
**  lsetData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Systems/LightSets/Data/lsetData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetData::lsetData()
:	m_Name("Name"),
	m_AmbientLight("Ambient Color", maFloatRGBA())
{
}

//-------------------------------------------------
//-------------------------------------------------
lsetData::lsetData(const lsetData& i_Data)
:	m_Name("Name"),
	m_AmbientLight("Ambient Color", maFloatRGBA())
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetData::~lsetData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool lsetData::operator == (const lsetData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lsetData& lsetData::operator=(const lsetData& i_Data)
{
	if (this == &i_Data) return *this;

	m_Name = i_Data.m_Name;

	m_AmbientLight	= i_Data.m_AmbientLight;
	
	m_Lights = i_Data.m_Lights;
	m_Objects = i_Data.m_Objects;

	return *this;
}
