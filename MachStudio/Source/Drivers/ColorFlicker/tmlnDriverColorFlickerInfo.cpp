/*****************************************************************************
**	tmlnDriverColorFlickerInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#include "Drivers/ColorFlicker/tmlnDriverColorFlickerInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorFlickerInfo::tmlnDriverColorFlickerInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_StartColor1(1.0f,1.0f,1.0f,1.0f),
	m_StartColor2(1.0f,1.0f,1.0f,1.0f),
	m_EndColor1(1.0f,1.0f,1.0f,1.0f),
	m_EndColor2(1.0f,1.0f,1.0f,1.0f),
	m_fFrequency(1.0f)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorFlickerInfo::~tmlnDriverColorFlickerInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverColorFlickerInfo::Clone()
{
	return new tmlnDriverColorFlickerInfo(*this);
}
