/*****************************************************************************
**	tmlnDriverColorInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Color/tmlnDriverColorInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorInfo::tmlnDriverColorInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value(1,1,1,1)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverColorInfo::~tmlnDriverColorInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverColorInfo::Clone()
{
	return new tmlnDriverColorInfo(*this);
}
