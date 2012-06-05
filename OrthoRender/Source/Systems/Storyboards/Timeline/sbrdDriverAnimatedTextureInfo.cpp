/*****************************************************************************
**	sbrdDriverAnimatedTextureInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Storyboards/Timeline/sbrdDriverAnimatedTextureInfo.hpp"

#include "Graphics/G3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdDriverAnimatedTextureInfo::sbrdDriverAnimatedTextureInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_NumberOfFrames(0),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverResizeByAnimLength(true),
	m_bLooping(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
sbrdDriverAnimatedTextureInfo::~sbrdDriverAnimatedTextureInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* sbrdDriverAnimatedTextureInfo::Clone()
{
	return new sbrdDriverAnimatedTextureInfo(*this);
}
