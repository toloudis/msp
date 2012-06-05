/********************************************************************************************\
**  envtData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Environments/Data/envtData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtData::envtData()
:	m_Name("Name"),
	m_DiffuseMapName("Diffuse Map", itString("")),
	m_DiffuseFactor("Diffuse Factor", 0),
	m_SpecularMapName("Specular Map", itString("")),
	m_SpecularFactor("Specular Factor", 0),
	m_DiffuseAngle("Diffuse Angle", 0),
	m_SpecularAngle("Specular Angle", 0)
{
}

//-------------------------------------------------
//-------------------------------------------------
envtData::envtData(const envtData& i_Data)
:	m_Name("Name"),
	m_DiffuseMapName("Diffuse Map", itString("")),
	m_DiffuseFactor("Diffuse Factor", 0),
	m_SpecularMapName("Specular Map", itString("")),
	m_SpecularFactor("Specular Factor", 0),
	m_DiffuseAngle("Diffuse Angle", 0),
	m_SpecularAngle("Specular Angle", 0)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtData::~envtData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool envtData::operator == (const envtData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtData& envtData::operator=(const envtData& i_Data)
{
	if (this == &i_Data) return *this;

	m_Name = i_Data.m_Name;

	m_DiffuseMapName	= i_Data.m_DiffuseMapName;
	m_DiffuseFactor		= i_Data.m_DiffuseFactor;
	m_SpecularMapName	= i_Data.m_SpecularMapName;
	m_SpecularFactor	= i_Data.m_SpecularFactor;
	m_DiffuseAngle		= i_Data.m_DiffuseAngle;
	m_SpecularAngle		= i_Data.m_SpecularAngle;

	m_Objects = i_Data.m_Objects;

	return *this;
}
