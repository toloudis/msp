/********************************************************************************************\
**  mtrlNormalsData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlNormalsData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlNormalsData::mtrlNormalsData()
:	m_NormalMap("Normal Map", fsLocator()),
	m_BumpScale("Bump Scale", 0.5)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlNormalsData::mtrlNormalsData(const mtrlNormalsData& i_Data)
:	m_NormalMap("Normal Map", fsLocator()),
	m_BumpScale("Bump Scale", 0.5)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlNormalsData::~mtrlNormalsData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlNormalsData& mtrlNormalsData::operator=(const mtrlNormalsData& i_Data)
{
	if (this == &i_Data) return *this;

	m_BumpScale = i_Data.m_BumpScale;
	m_NormalMap = i_Data.m_NormalMap;

	return *this;
}

