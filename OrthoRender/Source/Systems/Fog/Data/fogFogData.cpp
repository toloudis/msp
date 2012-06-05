/****************************************************************************\
**  fogFogData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Data/fogFogData.hpp"

//============================================================================
//============================================================================
fogFogData::fogFogData()
:	m_Mode("Type", e_None), 
	m_Color("Color", maFloatRGBA(0,0,0,1)), 
	m_Density("Density",0),
	m_Start("Start",1), 
	m_End("End",1000),
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
	m_Density("Density",0),
	m_Start("Start",1), 
	m_End("End",1000),
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
	m_Density = i_Data.m_Density;
	m_Start = i_Data.m_Start;
	m_End = i_Data.m_End;
	m_Name = i_Data.m_Name;

	return *this;
}
