/*****************************************************************************
**	tmlnDriverPositionInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Position/tmlnDriverPositionInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverPositionInfo::tmlnDriverPositionInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value(0,0,0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverPositionInfo::~tmlnDriverPositionInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverPositionInfo::Clone()
{
	return new tmlnDriverPositionInfo(*this);
}
