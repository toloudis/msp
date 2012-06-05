/********************************************************************************************\
**  envtData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Environments/Data/envtData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
envtData::envtData()
:	m_Name("Name"),
	m_DiffuseColor("Diffuse Color", maFloatRGBA(0.5f,0.5f,0.5f,1)),
	m_DiffuseMapName("Diffuse Map", fsLocator()),
	m_DiffuseFactor("Diffuse Factor", 1),
	m_SpecularColor("Specular Color", maFloatRGBA(0,0,0,1)),
	m_SpecularMapName("Specular Map", fsLocator()),
	m_SpecularFactor("Specular Factor", 1),
	m_DiffuseAngle("Diffuse Angle", 0),
	m_SpecularAngle("Specular Angle", 0)
{
}

//-------------------------------------------------
//-------------------------------------------------
envtData::envtData(const envtData& i_Data)
:	m_Name("Name"),
	m_DiffuseColor("Diffuse Color", maFloatRGBA(0.5f,0.5f,0.5f,1)),
	m_DiffuseMapName("Diffuse Map", fsLocator()),
	m_DiffuseFactor("Diffuse Factor", 1),
	m_SpecularColor("Specular Color", maFloatRGBA(0,0,0,1)),
	m_SpecularMapName("Specular Map", fsLocator()),
	m_SpecularFactor("Specular Factor", 1),
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

	m_DiffuseColor		= i_Data.m_DiffuseColor;
	m_DiffuseMapName	= i_Data.m_DiffuseMapName;
	m_DiffuseFactor		= i_Data.m_DiffuseFactor;
	m_SpecularColor		= i_Data.m_SpecularColor;
	m_SpecularMapName	= i_Data.m_SpecularMapName;
	m_SpecularFactor	= i_Data.m_SpecularFactor;
	m_DiffuseAngle		= i_Data.m_DiffuseAngle;
	m_SpecularAngle		= i_Data.m_SpecularAngle;
	m_RampData			= i_Data.m_RampData;
	m_SwlData			= i_Data.m_SwlData;

	m_Objects = i_Data.m_Objects;

	return *this;
}
