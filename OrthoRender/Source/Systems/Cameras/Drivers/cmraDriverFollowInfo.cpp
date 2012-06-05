/*****************************************************************************
**	cmraDriverFollowInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFollowInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFollowInfo::cmraDriverFollowInfo(chDefs::Name i_ChunkName)
:	tmlnDriverInfo(i_ChunkName),
	m_Direction(0.0f,0.0f,0.0f)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFollowInfo::~cmraDriverFollowInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFollowInfo::Clone()
{
	return new cmraDriverFollowInfo(*this);
}
