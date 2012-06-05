/*****************************************************************************
**	cmraDriverCaptureInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverCaptureInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCaptureInfo::cmraDriverCaptureInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverCaptureInfo::~cmraDriverCaptureInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverCaptureInfo::Clone()
{
	return new cmraDriverCaptureInfo(*this);
}
