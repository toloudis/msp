/*****************************************************************************
**	cmraDriverDataAttachNodeInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverDataAttachNodeInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataAttachNodeInfo::cmraDriverDataAttachNodeInfo(chDefs::Name i_ChunkName)
: chParsable(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverDataAttachNodeInfo::~cmraDriverDataAttachNodeInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
cmraDriverDataAttachNodeInfo* cmraDriverDataAttachNodeInfo::Clone()
{
	return new cmraDriverDataAttachNodeInfo(*this);
}
