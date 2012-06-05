/*****************************************************************************
**	cmraDriverKeyTargetInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverKeyTargetInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyTargetInfo::cmraDriverKeyTargetInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyTargetInfo::~cmraDriverKeyTargetInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverKeyTargetInfo::Clone()
{
	return new cmraDriverKeyTargetInfo(*this);
}
