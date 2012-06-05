/*****************************************************************************
**	prtclDriverAnimationInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Particles/Timeline/prtclDriverAnimationInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnAnimationInfo::tmlnAnimationInfo()
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
prtclDriverAnimationInfo::prtclDriverAnimationInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverAnimationInfo::~prtclDriverAnimationInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* prtclDriverAnimationInfo::Clone()
{
	return new prtclDriverAnimationInfo(*this);
}
