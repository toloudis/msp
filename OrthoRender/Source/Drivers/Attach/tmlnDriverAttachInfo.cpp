/*****************************************************************************
**	tmlnDriverAttachInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachInfo::tmlnDriverAttachInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachInfo::~tmlnDriverAttachInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAttachInfo::Clone()
{
	return new tmlnDriverAttachInfo(*this);
}
