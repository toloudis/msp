/*****************************************************************************
**	tmlnDriverFilePathInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/FilePath/tmlnDriverFilePathInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFilePathInfo::tmlnDriverFilePathInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverFilePathInfo::~tmlnDriverFilePathInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverFilePathInfo::Clone()
{
	return new tmlnDriverFilePathInfo(*this);
}
