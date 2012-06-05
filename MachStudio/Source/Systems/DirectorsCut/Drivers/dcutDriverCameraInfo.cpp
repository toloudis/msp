/*****************************************************************************
**	dcutDriverCameraInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/DirectorsCut/Drivers/dcutDriverCameraInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCameraInfo::dcutDriverCameraInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dcutDriverCameraInfo::~dcutDriverCameraInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* dcutDriverCameraInfo::Clone()
{
	return new dcutDriverCameraInfo(*this);
}
