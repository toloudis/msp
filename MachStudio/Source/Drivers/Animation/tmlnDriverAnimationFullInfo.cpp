/*****************************************************************************
**	tmlnDriverAnimationFullInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Drivers/Animation/tmlnDriverAnimationFullInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnAnimationFullInfo::tmlnAnimationFullInfo()
:	m_bDriverToAnimLength(true),
	m_bUseAnimStart(false),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_StartFrame(-1.0f),
	m_EndFrame(-1.0f),
	m_bLooping(false)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimationFullInfo::tmlnDriverAnimationFullInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAnimationFullInfo::~tmlnDriverAnimationFullInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAnimationFullInfo::Clone()
{
	return new tmlnDriverAnimationFullInfo(*this);
}
