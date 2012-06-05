/*****************************************************************************
**  mtrlOutlineData.hpp
**
**      mtrlOutlineData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_OUTLINEDATA_HPP
#error mtrlOutlineData.hpp multiply included
#endif
#define MTRL_OUTLINEDATA_HPP

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
class mtrlOutlineData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlOutlineData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlOutlineData(const mtrlOutlineData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlOutlineData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlOutlineData& operator=(const mtrlOutlineData& i_Data);

public:
	prtyFloat m_OutlineDepthScale;
	prtyFloat m_OutlineThreshold;
	prtyFloat m_OutlineThickness;
	prtyColor m_OutlineColor;
	prtyBoolean m_bUseDepths;
	prtyBoolean m_bUseNormals;
};

