/****************************************************************************\
**  fogFogData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Data/fogFogData.hpp"

//============================================================================
//============================================================================
fogFogData::fogFogData()
:	m_Mode("Type", e_None), 
	m_Color("Color", maFloatRGBA(0,0,0,1)), 
	m_Orientation("Orientation", maVector3d(0,0,0)),
	m_Density("Density",1),
	m_Start("Start",1), 
	m_End("End",1000),
	m_AltitudeStart("Height Start",0), 
	m_AltitudeEnd("Height End",10),
	m_AltitudeDensity("Height Density",1),
	m_bEnable("Enable Fog",0),
	m_bWorldOrientation("Use World Orientation", 0),
	m_Name("Name", nameString("Fog"))
{
	m_Mode.SetEnumTag(e_None, "None");
	m_Mode.SetEnumTag(e_Linear, "Linear");
	m_Mode.SetEnumTag(e_Exponential, "Exponential");
	m_Mode.SetEnumTag(e_ExponentialSq, "ExponentialSq");
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
fogFogData::fogFogData(const fogFogData& i_Data)
:	m_Mode("Type", e_None), 
	m_Color("Color", maFloatRGBA(0,0,0,1)), 
	m_Orientation("Orientation", maVector3d(0,0,0)),
	m_Density("Density",0),
	m_Start("Start",1), 
	m_End("End",1000),
	m_AltitudeStart("Height Start",0), 
	m_AltitudeEnd("Height End",10),
	m_AltitudeDensity("Height Density",1),
	m_bEnable("Enable Fog",0),
	m_bWorldOrientation("Use World Orientation", 0),
	m_Name("Name", nameString("Fog"))
{
	m_Mode.SetEnumTag(e_None, "None");
	m_Mode.SetEnumTag(e_Linear, "Linear");
	m_Mode.SetEnumTag(e_Exponential, "Exponential");
	m_Mode.SetEnumTag(e_ExponentialSq, "ExponentialSq");

	(*this) = i_Data;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
fogFogData::~fogFogData()
{
}

//-------------------------------------------------
bool fogFogData::operator == (const fogFogData& i_Item)
{
	// name comparison??
	// FIX ME
	// always equal or always unequal? 
	return true;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
fogFogData& fogFogData::operator=(const fogFogData& i_Data)
{
	if (this == &i_Data) return *this;

	m_Mode = i_Data.m_Mode;
	m_Color = i_Data.m_Color;
	m_Orientation = i_Data.m_Orientation;
	m_Density = i_Data.m_Density;
	m_Start = i_Data.m_Start;
	m_End = i_Data.m_End;
	m_AltitudeStart = i_Data.m_AltitudeStart;
	m_AltitudeEnd = i_Data.m_AltitudeEnd;
	m_AltitudeDensity = i_Data.m_AltitudeDensity;
	m_bEnable = i_Data.m_bEnable;
	m_bWorldOrientation = i_Data.m_bWorldOrientation;
	m_Name = i_Data.m_Name;

	return *this;
}
