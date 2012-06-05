/********************************************************************************************\
**  ptltData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2003 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PtLt/Data/ptltData.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Support/tmln/tmlnDriverInfo.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltData::ptltData()
:	m_Enabled("Enabled", true),
	m_bEditorVisible("Visible in Editor", true),
	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_ShadowSource("Shadow Source", false),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 100000.0f),
	m_Intensity("Intensity", 1.0),
	m_Name("Name"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true)
{
}

//-------------------------------------------------
//-------------------------------------------------
ptltData::ptltData(const ptltData& i_Data)
:	m_Enabled("Enabled", true),
	m_bEditorVisible("Visible in Editor", true),
	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_ShadowSource("Shadow Source", false),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 100000.0f),
	m_Intensity("Intensity", 1.0),
	m_Name("Name"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltData::~ptltData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool ptltData::operator == (const ptltData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ptltData& ptltData::operator=(const ptltData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_Color			= i_Data.m_Color;
	this->m_Enabled			= i_Data.m_Enabled;
	this->m_ShadowSource	= i_Data.m_ShadowSource;
	this->m_Falloff			= i_Data.m_Falloff;
	this->m_Range			= i_Data.m_Range;
	this->m_Intensity		= i_Data.m_Intensity;
	this->m_Position		= i_Data.m_Position;
	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_Name			= i_Data.m_Name;
	this->m_bDiffuseEnabled	= i_Data.m_bDiffuseEnabled;
	this->m_bSpecularEnabled = i_Data.m_bSpecularEnabled;
	this->m_bAffectsFur		= i_Data.m_bAffectsFur;
	this->m_bAffectsGlow	= i_Data.m_bAffectsGlow;

	return *this;
}
