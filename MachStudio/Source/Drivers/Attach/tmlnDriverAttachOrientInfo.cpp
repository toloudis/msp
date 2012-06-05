/*****************************************************************************
**	tmlnDriverAttachOrientInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#include "Drivers/Attach/tmlnDriverAttachOrientInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachOrientInfo::tmlnDriverAttachOrientInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverAttachOrientInfo::~tmlnDriverAttachOrientInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverAttachOrientInfo::Clone()
{
	return new tmlnDriverAttachOrientInfo(*this);
}
