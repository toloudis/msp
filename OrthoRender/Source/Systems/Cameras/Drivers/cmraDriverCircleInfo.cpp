/*****************************************************************************
**	cmraDriverCircleInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverCircleInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCircleInfo::cmraDriverCircleInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_Revolutions(1.0f),
	m_bClockwise(true),
	m_fStartingYaw(0.0f),
	m_fStartingPitch(45.0f),
	m_fRadius(10.0f)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCircleInfo::~cmraDriverCircleInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverCircleInfo::Clone()
{
	return new cmraDriverCircleInfo(*this);
}
