/*****************************************************************************
**	cmraDriverFocusDistanceAttachInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFocusDistanceAttachInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttachInfo::cmraDriverFocusDistanceAttachInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusDistanceAttachInfo::~cmraDriverFocusDistanceAttachInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFocusDistanceAttachInfo::Clone()
{
	return new cmraDriverFocusDistanceAttachInfo(*this);
}
