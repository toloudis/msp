/*****************************************************************************
**	cmraDriverKeyPositionAndTargetInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverKeyPositionAndTargetInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyPositionAndTargetInfo::cmraDriverKeyPositionAndTargetInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyPositionAndTargetInfo::~cmraDriverKeyPositionAndTargetInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverKeyPositionAndTargetInfo::Clone()
{
	return new cmraDriverKeyPositionAndTargetInfo(*this);
}
