/********************************************************************************************\
**  mtrlTextureFilterData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlTextureFilterData.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlTextureFilterData::mtrlTextureFilterData()
:	m_bEnableMipmap("Enable Texture Filtering", true)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlTextureFilterData::mtrlTextureFilterData(const mtrlTextureFilterData& i_Data)
:	m_bEnableMipmap("Enable Texture Filtering", true)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlTextureFilterData::~mtrlTextureFilterData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlTextureFilterData& mtrlTextureFilterData::operator=(const mtrlTextureFilterData& i_Data)
{
	if (this == &i_Data) return *this;

	m_bEnableMipmap = i_Data.m_bEnableMipmap;

	return *this;
}

