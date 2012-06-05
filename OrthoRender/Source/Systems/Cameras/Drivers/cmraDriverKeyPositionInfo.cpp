/*****************************************************************************
**	cmraDriverKeyPositionInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverKeyPositionInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyPositionInfo::cmraDriverKeyPositionInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverKeyPositionInfo::~cmraDriverKeyPositionInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverKeyPositionInfo::Clone()
{
	return new cmraDriverKeyPositionInfo(*this);
}
