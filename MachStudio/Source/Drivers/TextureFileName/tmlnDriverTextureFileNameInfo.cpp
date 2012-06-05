/*****************************************************************************
**	tmlnDriverTextureFileNameInfo.cpp
**
**	Data structure for parsing texture location drivers
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Drivers/TextureFileName/tmlnDriverTextureFileNameInfo.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverTextureFileNameInfo::tmlnDriverTextureFileNameInfo(chDefs::Name i_ChunkName)
: tmlnDriverInfo(i_ChunkName), m_Value()
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tmlnDriverTextureFileNameInfo::~tmlnDriverTextureFileNameInfo()
{

}

//--------------------------------------------------------------------
// Return pointer to new equivalent driver info structure
//--------------------------------------------------------------------
tmlnDriverInfo* tmlnDriverTextureFileNameInfo::Clone()
{
	return new tmlnDriverTextureFileNameInfo(*this);
}
