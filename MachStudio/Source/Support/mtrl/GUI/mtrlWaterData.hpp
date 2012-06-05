/*****************************************************************************
**  mtrlWaterData.hpp
**
**      mtrlWaterData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#ifdef MTRL_WATERDATA_HPP
#error mtrlWaterData.hpp multiply included
#endif
#define MTRL_WATERDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_POINT3D_HPP
#include "Core/prty/prtyPoint3d.hpp"
#endif

class mtrlWaterData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlWaterData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlWaterData(const mtrlWaterData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~mtrlWaterData();

	//-------------------------------------------------
	bool operator == (const mtrlWaterData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	mtrlWaterData& operator=(const mtrlWaterData& i_Data);

	prtyFloat   m_FadeBias;
	prtyFloat   m_FadeExp;
	prtyFloat   m_NoiseBumpFactor;
	prtyFloat   m_NoiseSpeed;
	prtyFloat   m_RingBumpFactor;
	prtyFloat   m_RingFreq;
	prtyFloat   m_RingSpeed;
	prtyFloat   m_TimeOffset;
	prtyFloat   m_WaveSpeed;
	prtyPoint3d m_RingCenter;
	prtyColor   m_WaterColor;
};

