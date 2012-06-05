/*****************************************************************************
**	tmlnDriverFileNameInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/FileName/tmlnDriverFileNameInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFileNameInfo::tmlnDriverFileNameInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFileNameInfo::~tmlnDriverFileNameInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFileNameInfo::Clone()
{
	return new tmlnDriverFileNameInfo(*this);
}
