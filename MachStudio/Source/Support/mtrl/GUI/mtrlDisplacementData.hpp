/*****************************************************************************
**  mtrlDisplacementData.hpp
**
**      mtrlDisplacementData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_DISPLACEMENTDATA_HPP
#error mtrlDisplacementData.hpp multiply included
#endif
#define MTRL_DISPLACEMENTDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
//#ifndef PRTY_FILENAME_HPP
//#include "Core/prty/prtyFileName.hpp"
//#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif

//============================================================================
//============================================================================
class mtrlDisplacementData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlDisplacementData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlDisplacementData(const mtrlDisplacementData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlDisplacementData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlDisplacementData& operator=(const mtrlDisplacementData& i_Data);

	prtyTextureFileName m_DisplacementMap;
	prtyFloat m_DisplacementScale;
	prtyFloat m_DisplacementBias;
	prtyFloat m_DisplacementBlur;

//	prtyBoolean m_bEnableTessellation;
	prtyFloat m_TessellationValue;
	prtyFloat m_ObjTextureSize;
//	prtyFloat m_ObjTextureAspect;
};

