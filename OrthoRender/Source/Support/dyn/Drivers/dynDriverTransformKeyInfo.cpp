/*****************************************************************************
**	dynDriverTransformKeyInfo.cpp
**
**	Data structure for parsing sound drivers
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/


#include "Support/dyn/Drivers/dynDriverTransformKeyInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynDriverTransformKeyInfo::dynDriverTransformKeyInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Translation(0,0,0)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
dynDriverTransformKeyInfo::~dynDriverTransformKeyInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* dynDriverTransformKeyInfo::Clone()
{
	return new dynDriverTransformKeyInfo(*this);
}
