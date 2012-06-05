/*****************************************************************************
**	rcdDriverFloatInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Record/Float/rcdDriverFloatInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
rcdDriverFloatInfo::rcdDriverFloatInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Keys(0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rcdDriverFloatInfo::~rcdDriverFloatInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* rcdDriverFloatInfo::Clone()
{
	return new rcdDriverFloatInfo(*this);
}
