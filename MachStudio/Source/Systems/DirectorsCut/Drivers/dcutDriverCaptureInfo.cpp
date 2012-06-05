/*****************************************************************************
**	dcutDriverCaptureInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/DirectorsCut/Drivers/dcutDriverCaptureInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCaptureInfo::dcutDriverCaptureInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCaptureInfo::~dcutDriverCaptureInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* dcutDriverCaptureInfo::Clone()
{
	return new dcutDriverCaptureInfo(*this);
}
