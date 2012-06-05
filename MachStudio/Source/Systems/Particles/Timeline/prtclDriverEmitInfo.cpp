/*****************************************************************************
**	prtclDriverEmitInfo.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Particles/Timeline/prtclDriverEmitInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverEmitInfo::prtclDriverEmitInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
prtclDriverEmitInfo::~prtclDriverEmitInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* prtclDriverEmitInfo::Clone()
{
	return new prtclDriverEmitInfo(*this);
}
