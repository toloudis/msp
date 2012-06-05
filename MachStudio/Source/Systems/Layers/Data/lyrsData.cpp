/********************************************************************************************\
**  lyrsData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Systems/Layers/Data/lyrsData.hpp"

#include "Core/env/envSTLHelpers.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyrsData::lyrsData()
:	m_Name("Name"),
	m_bVisible("Visible", true),
	m_bPickable("Pickable", true),
	m_bWireframe("Wireframe", false),
	m_bLowRes("Low Res", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
lyrsData::lyrsData(const lyrsData& i_Data)
:	m_Name("Name"),
	m_bVisible("Visible", true),
	m_bPickable("Pickable", true),
	m_bWireframe("Wireframe", false),
	m_bLowRes("Low Res", false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyrsData::~lyrsData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool lyrsData::operator == (const lyrsData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
lyrsData& lyrsData::operator=(const lyrsData& i_Data)
{
	if (this == &i_Data) return *this;

	m_Name			= i_Data.m_Name;

	m_bVisible		= i_Data.m_bVisible;
	m_bPickable		= i_Data.m_bPickable;
	m_bWireframe	= i_Data.m_bWireframe;
	m_bLowRes		= i_Data.m_bLowRes;
	
	m_Objects		= i_Data.m_Objects;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void lyrsLayersData::Clear()
{
	m_Items.clear();
}
