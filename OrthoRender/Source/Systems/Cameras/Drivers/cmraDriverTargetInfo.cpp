/*****************************************************************************
**	cmraDriverTargetInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverTargetInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverTargetInfo::cmraDriverTargetInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverTargetInfo::~cmraDriverTargetInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverTargetInfo::Clone()
{
	return new cmraDriverTargetInfo(*this);
}
