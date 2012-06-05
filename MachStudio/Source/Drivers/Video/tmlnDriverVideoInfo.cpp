/*****************************************************************************
**	tmlnDriverVideoInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/Video/tmlnDriverVideoInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverVideoInfo::tmlnDriverVideoInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverToAnimLength(true),
	m_bLooping(false),
	m_bReversing(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverVideoInfo::~tmlnDriverVideoInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverVideoInfo::Clone()
{
	return new tmlnDriverVideoInfo(*this);
}
