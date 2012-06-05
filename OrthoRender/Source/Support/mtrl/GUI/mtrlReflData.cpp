/********************************************************************************************\
**  mtrlReflData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlReflData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlReflData::mtrlReflData()
:	m_bAutoGenEnvMap("Dynamic Reflection", false),
	m_ReflMapResolution("ReflectionMap Size", 256),
	m_NearPlane("Near Plane", 0.1f),
	m_bIsPlanar("Is Planar", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlReflData::mtrlReflData(const mtrlReflData& i_Data)
:	m_bAutoGenEnvMap("Dynamic Reflection", false),
	m_ReflMapResolution("ReflectionMap Size", 256),
	m_NearPlane("Near Plane", 0.1f),
	m_bIsPlanar("Is Planar", false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlReflData::~mtrlReflData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlReflData& mtrlReflData::operator=(const mtrlReflData& i_Data)
{
	if (this == &i_Data) return *this;

	m_bAutoGenEnvMap		= i_Data.m_bAutoGenEnvMap;
	m_ReflMapResolution		= i_Data.m_ReflMapResolution;
	m_NearPlane				= i_Data.m_NearPlane;
	m_bIsPlanar				= i_Data.m_bIsPlanar;

	return *this;
}

