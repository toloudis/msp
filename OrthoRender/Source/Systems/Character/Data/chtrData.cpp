/****************************************************************************\
**  chtrData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Data/chtrData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrData::chtrData()
:	m_bEditorVisible("Visible in Editor",true),
	m_Name("Name"),
	m_Filename("Filename"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bLockedMaterials("Locked Materials", false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrData::chtrData(const itString& i_Filename )
:	m_bEditorVisible("Visible in Editor",true),
	m_Name("Name"),
	m_Filename("Filename", i_Filename),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bLockedMaterials("Locked Materials", false)
{
	m_Filename = i_Filename;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrData::chtrData(	const itString& i_Filename,
					const maPoint3d& i_Position,
					const maRotation& i_Orientation )
:	m_bEditorVisible("Visible in Editor",true),
	m_Name("Name"),
	m_Filename("Filename", i_Filename),
	m_Position("Position", i_Position),
	m_Orientation("Orientation", i_Orientation),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bLockedMaterials("Locked Materials", false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool chtrData::operator == (const chtrData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chtrData& chtrData::operator=(const chtrData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_bVisible		= i_Data.m_bVisible;
	this->m_Name			= i_Data.m_Name;
	this->m_Filename		= i_Data.m_Filename;
	//this->m_Filename.SetValueWithoutNotify(i_Data.m_Filename);

	this->m_Orientation		= i_Data.m_Orientation;
	this->m_Position		= i_Data.m_Position;
	this->m_Scale			= i_Data.m_Scale;
	this->m_PivotPoint			= i_Data.m_PivotPoint;
	this->m_PivotCompensation	= i_Data.m_PivotCompensation;
	this->m_bLockedMaterials	= i_Data.m_bLockedMaterials;

	return *this;
}

