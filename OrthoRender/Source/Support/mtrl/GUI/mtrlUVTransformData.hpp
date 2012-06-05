/*****************************************************************************
**  mtrlUVTransformData.hpp
**
**      mtrlUVTransformData is the data for a shader effect.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_UVTRANSFORMDATA_HPP
#error mtrlUVTransformData.hpp multiply included
#endif
#define MTRL_UVTRANSFORMDATA_HPP

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

//============================================================================
//============================================================================
class mtrlUVTransformData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlUVTransformData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlUVTransformData(const mtrlUVTransformData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlUVTransformData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlUVTransformData& operator=(const mtrlUVTransformData& i_Data);

public:
	prtyFloat m_UScale, m_VScale;
	prtyFloat m_UTrans, m_VTrans;
	prtyFloat m_UVAngle;
};

