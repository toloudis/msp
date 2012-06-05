/*****************************************************************************
**  mtrlUVTransformData.hpp
**
**      mtrlUVTransformData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_TEXTUREFILTERDATA_HPP
#error mtrlTextureFilterData.hpp multiply included
#endif
#define MTRL_TEXTUREFILTERDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

//============================================================================
//============================================================================
class mtrlTextureFilterData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlTextureFilterData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlTextureFilterData(const mtrlTextureFilterData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlTextureFilterData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlTextureFilterData& operator=(const mtrlTextureFilterData& i_Data);

public:
	prtyBoolean m_bEnableMipmap;
};

