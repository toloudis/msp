/*****************************************************************************
**	tmlnDriverAnimatedFileNameInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/FileName/tmlnDriverAnimatedFileNameInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimatedFileNameInfo::tmlnDriverAnimatedFileNameInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_NumberOfFrames(0),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverToAnimLength(true),
	m_bLooping(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimatedFileNameInfo::~tmlnDriverAnimatedFileNameInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimatedFileNameInfo::Clone()
{
	return new tmlnDriverAnimatedFileNameInfo(*this);
}
