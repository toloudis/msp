/*****************************************************************************
**  mtrlGlowData.hpp
**
**      mtrlGlowData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_GLOWDATA_HPP
#error mtrlGlowData.hpp multiply included
#endif
#define MTRL_GLOWDATA_HPP

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
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif
#ifndef PRTY_TEXTUREFILENAME_HPP
#include "Core/prty/prtyTextureFileName.hpp"
#endif


//============================================================================
//============================================================================
class mtrlGlowData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlGlowData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlGlowData(const mtrlGlowData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlGlowData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlGlowData& operator=(const mtrlGlowData& i_Data);

public:
	prtyFloat m_GlowAmount;
	prtyPoint3d m_GlowScale;
	prtyFloat m_GlowSize;
	prtyBoolean m_bConstantGlow;
	prtyTextureFileName m_GlowMask;
};

