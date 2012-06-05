/********************************************************************************************\
**  dirltData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "dirltData.hpp"

#include "envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltData::dirltData()
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Filename("File Name"),
	m_Position("Position"),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", false),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_Color("Color", maFloatRGBA(0.7f, 0.7f, 0.7f, 1.0f)),
	m_Direction("Direction", maVector3d(0.0f, 1.0f, 0.0f))
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltData::dirltData(const dirltData& i_Data)
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Filename("File Name"),
	m_Position("Position"),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", false),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_Color("Color", maFloatRGBA(0.7f, 0.7f, 0.7f, 1.0f)),
	m_Direction("Direction", maVector3d(0.0f, 1.0f, 0.0f))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltData::~dirltData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool dirltData::operator == (const dirltData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
dirltData& dirltData::operator=(const dirltData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_Name			= i_Data.m_Name;
	this->m_Filename		= i_Data.m_Filename;
	this->m_Position		= i_Data.m_Position;
	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_Color			= i_Data.m_Color;
	this->m_Enabled			= i_Data.m_Enabled;
	this->m_ShadowSource	= i_Data.m_ShadowSource;
	this->m_Direction		= i_Data.m_Direction;
	this->m_bDiffuseEnabled	= i_Data.m_bDiffuseEnabled;
	this->m_bSpecularEnabled= i_Data.m_bSpecularEnabled;

	return *this;
}
