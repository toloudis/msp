/****************************************************************************\
**  mnmBaseData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Support/mnm/mnmBaseData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Support/tmln/tmlnBaseData.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmBaseData::mnmBaseData() 
:	m_bEditorVisible(true),
	m_Position( maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	m_Orientation( maRotation( 0.0f, 0.0f, 0.0f ) )
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmBaseData::mnmBaseData(const itString& i_Filename )
:	m_Filename(i_Filename),
	m_Position( maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	m_Orientation( maRotation( 0.0f, 0.0f, 0.0f ) ),
	m_bEditorVisible(true)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmBaseData::mnmBaseData(const itString& i_Filename,
							const maPoint3d& i_Position,
							const maRotation& i_Orientation )
:	m_Filename(i_Filename),
	m_Position( i_Position ),
	m_Orientation( i_Orientation ),
	m_bEditorVisible(true)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool mnmBaseData::operator == (const mnmBaseData& i_Item)
{
	return (this->m_Filename == i_Item.m_Filename);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmBaseData& mnmBaseData::operator=(const mnmBaseData& i_Data)
{
	if (this == &i_Data) return *this;

	envSTLHelpers::DeleteContainer(this->m_Drivers);

	this->m_Name		= i_Data.m_Name;
	this->m_Filename	= i_Data.m_Filename;
	this->m_Position	= i_Data.m_Position;
	this->m_Orientation = i_Data.m_Orientation;
	this->m_bEditorVisible = i_Data.m_bEditorVisible;

	int num_drivers = i_Data.m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers.push_back(i_Data.m_Drivers[i]->Clone());
	}

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mnmBaseData& mnmBaseData::operator=(const tmlnBaseData& i_Data)
{
	envSTLHelpers::DeleteContainer(this->m_Drivers);

	int num_drivers = i_Data.m_Drivers.size();
	for (int i=0; i<num_drivers; i++)
	{
		this->m_Drivers.push_back(i_Data.m_Drivers[i]->Clone());
	}

	return *this;
}

