/*****************************************************************************
**	tmlnDriverAnimatedTextureFileNameInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Drivers/TextureFileName/tmlnDriverAnimatedTextureFileNameInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimatedTextureFileNameInfo::tmlnDriverAnimatedTextureFileNameInfo(chDefs::Name i_ChunkName)
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
tmlnDriverAnimatedTextureFileNameInfo::~tmlnDriverAnimatedTextureFileNameInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimatedTextureFileNameInfo::Clone()
{
	return new tmlnDriverAnimatedTextureFileNameInfo(*this);
}
