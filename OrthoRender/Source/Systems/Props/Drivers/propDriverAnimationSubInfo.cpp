/*****************************************************************************
**	propDriverAnimationSubInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Props/Drivers/propDriverAnimationSubInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
propAnimationSubInfo::propAnimationSubInfo()
: m_AnimIndex(0), m_AnimName(""), m_bDriverResizeByAnimLength(true)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAnimationSubInfo::propDriverAnimationSubInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
propDriverAnimationSubInfo::~propDriverAnimationSubInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* propDriverAnimationSubInfo::Clone()
{
	return new propDriverAnimationSubInfo(*this);
}
