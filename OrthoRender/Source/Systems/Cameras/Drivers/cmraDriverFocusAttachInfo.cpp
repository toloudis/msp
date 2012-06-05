/*****************************************************************************
**	cmraDriverFocusAttachInfo.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Drivers/cmraDriverFocusAttachInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusAttachInfo::cmraDriverFocusAttachInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverFocusAttachInfo::~cmraDriverFocusAttachInfo()
{
}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverFocusAttachInfo::Clone()
{
	return new cmraDriverFocusAttachInfo(*this);
}
