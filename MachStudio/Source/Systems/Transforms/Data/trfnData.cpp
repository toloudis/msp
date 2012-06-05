/********************************************************************************************\
**  trfnData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Transforms/Data/trfnData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData::trfnData()
:	m_bVisible("Visible",true),
	m_Name("Name"),
	m_Position("Position", maPoint3d( 0.0f, 0.0f, 0.0f )),
	m_Orientation("Orientation", maRotation( 0.0f, 0.0f, 0.0f )),
	m_Scale("Scale", 1),
	m_PivotPoint("Local Pivot"),
	m_bPickable("Pickable", true),
	m_bWireframe("Wireframe", false),
	m_bInheritsTransform("Inherits Transform", true)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData::~trfnData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool trfnData::operator == (const trfnData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
trfnData& trfnData::operator=(const trfnData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_Name				= i_Data.m_Name;

	this->m_bVisible			= i_Data.m_bVisible;
	this->m_Name				= i_Data.m_Name;

	this->m_Orientation			= i_Data.m_Orientation;
	this->m_Position			= i_Data.m_Position;
	this->m_Scale				= i_Data.m_Scale;
	this->m_PivotPoint			= i_Data.m_PivotPoint;
	this->m_PivotCompensation	= i_Data.m_PivotCompensation;
	this->m_bPickable			= i_Data.m_bPickable;
	this->m_bWireframe			= i_Data.m_bWireframe;
	this->m_bInheritsTransform	= i_Data.m_bInheritsTransform;

	this->m_Objects				= i_Data.m_Objects;

	return *this;
}
