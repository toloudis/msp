/*****************************************************************************
**	cmraDriverRenderPassInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Cameras/Drivers/cmraDriverRenderPassInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverRenderPassInfo::cmraDriverRenderPassInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmraDriverRenderPassInfo::~cmraDriverRenderPassInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* cmraDriverRenderPassInfo::Clone()
{
	return new cmraDriverRenderPassInfo(*this);
}
