/*****************************************************************************
**	tmlnDriverEnableInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Enable/tmlnDriverEnableInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverEnableInfo::tmlnDriverEnableInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Enabled(true)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverEnableInfo::~tmlnDriverEnableInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverEnableInfo::Clone()
{
	return new tmlnDriverEnableInfo(*this);
}
