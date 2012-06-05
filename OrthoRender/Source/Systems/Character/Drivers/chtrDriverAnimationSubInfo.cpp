/*****************************************************************************
**	chtrDriverAnimationSubInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/Drivers/chtrDriverAnimationSubInfo.hpp"

#include "Graphics/g3d/g3dConstants.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrAnimationSubInfo::chtrAnimationSubInfo()
:	m_bDriverToAnimLength(true),
	m_FrameRate(g3dConstants::c_fDefaultFrameRate),
	m_StartFrame(-1.0f),
	m_EndFrame(-1.0f),
	m_bLooping(false)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationSubInfo::chtrDriverAnimationSubInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
chtrDriverAnimationSubInfo::~chtrDriverAnimationSubInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* chtrDriverAnimationSubInfo::Clone()
{
	return new chtrDriverAnimationSubInfo(*this);
}
