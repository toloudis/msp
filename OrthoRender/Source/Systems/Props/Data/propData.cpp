/****************************************************************************\
**  propData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Data/propData.hpp"

#include "Core/env/envSTLHelpers.hpp"
//#include "Core/ma/maRotation.hpp"


//
//		propData
//

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propData::propData()
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Filename("Filename"),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bLockedMaterials("Locked Materials", false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propData::propData(const itString& i_Filename )
:	m_bEditorVisible("Visible in Editor", true),
	m_Name("Name"),
	m_Filename("Filename", i_Filename),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bLockedMaterials("Locked Materials", false)
{
	m_Filename = i_Filename;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propData::propData( const itString& i_Filename,
					const maPoint3d& i_Position,
					const maRotation& i_Orientation )
:	m_bEditorVisible("Visible in Editor", true),
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
bool propData::operator == (const propData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
propData& propData::operator=(const propData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_bVisible		= i_Data.m_bVisible;
	this->m_Name			= i_Data.m_Name;
	this->m_Filename		= i_Data.m_Filename;
	this->m_Orientation		= i_Data.m_Orientation;
	this->m_Position		= i_Data.m_Position;
	this->m_Scale			= i_Data.m_Scale;
	this->m_PivotPoint			= i_Data.m_PivotPoint;
	this->m_PivotCompensation	= i_Data.m_PivotCompensation;
	this->m_bLockedMaterials	= i_Data.m_bLockedMaterials;
	return *this;
}
