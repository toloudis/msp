/*****************************************************************************
**  mtrlReflData.hpp
**
**      mtrlReflData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_REFLDATA_HPP
#error mtrlReflData.hpp multiply included
#endif
#define MTRL_REFLDATA_HPP

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
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif


//============================================================================
//============================================================================
class mtrlReflData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlReflData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlReflData(const mtrlReflData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlReflData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlReflData& operator=(const mtrlReflData& i_Data);

	prtyBoolean m_bAutoGenEnvMap;
	prtyInt32 m_ReflMapResolution;
	prtyFloat m_NearPlane;
	prtyBoolean m_bIsPlanar;
};

