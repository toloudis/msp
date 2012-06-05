/*****************************************************************************
**	billDriverAnimatedTextureInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Timeline/billDriverAnimatedTextureInfo.hpp"

#include "Graphics/G3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
billDriverAnimatedTextureInfo::billDriverAnimatedTextureInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_NumberOfFrames(0),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_bDriverResizeByAnimLength(true),
	m_bLooping(false)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
billDriverAnimatedTextureInfo::~billDriverAnimatedTextureInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* billDriverAnimatedTextureInfo::Clone()
{
	return new billDriverAnimatedTextureInfo(*this);
}
