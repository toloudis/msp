/*****************************************************************************
**	tmlnDriverAnimatedFilePathInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/FilePath/tmlnDriverAnimatedFilePathInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimatedFilePathInfo::tmlnDriverAnimatedFilePathInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_NumberOfFrames(0),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverToAnimLength(true),
	m_bLooping(false),
	m_bReversing(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimatedFilePathInfo::~tmlnDriverAnimatedFilePathInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimatedFilePathInfo::Clone()
{
	return new tmlnDriverAnimatedFilePathInfo(*this);
}
