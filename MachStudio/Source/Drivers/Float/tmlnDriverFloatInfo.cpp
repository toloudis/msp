/*****************************************************************************
**	tmlnDriverFloatInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Float/tmlnDriverFloatInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFloatInfo::tmlnDriverFloatInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value(0.0f)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFloatInfo::~tmlnDriverFloatInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFloatInfo::Clone()
{
	return new tmlnDriverFloatInfo(*this);
}
