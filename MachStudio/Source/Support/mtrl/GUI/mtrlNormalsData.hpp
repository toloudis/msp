/*****************************************************************************
**  mtrlNormalsData.hpp
**
**      mtrlNormalsData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_NORMALSDATA_HPP
#error mtrlNormalsData.hpp multiply included
#endif
#define MTRL_NORMALSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif


//============================================================================
//============================================================================
class mtrlNormalsData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlNormalsData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlNormalsData(const mtrlNormalsData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlNormalsData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlNormalsData& operator=(const mtrlNormalsData& i_Data);

public:
	prtyFloat m_BumpScale;
	prtyTextureFileName m_NormalMap;
};

