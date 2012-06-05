/*****************************************************************************
**  mtrlGlowData.hpp
**
**      mtrlGlowData is the data for a shader effect.
**
**	Extra Large Technology
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
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_VECTOR3D_HPP
#include "Core/prty/prtyVector3d.hpp"
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
	prtyVector3d m_GlowScale;
	prtyFloat m_GlowSize;
	prtyBoolean m_bConstantGlow;
	prtyFileName m_GlowMask;
};

