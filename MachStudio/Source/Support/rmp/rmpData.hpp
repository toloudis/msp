/********************************************************************************************\
**  rmpData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef RMP_DATA_HPP
#error rmpData.hpp multiply included
#endif
#define RMP_DATA_HPP

#ifndef PRTY_PROPERTYCALLBACK_HPP
#include "Core/prty/prtyPropertycallback.hpp"
#endif

#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif

#ifndef PRTY_GRADIENT_HPP
#include "Core/prty/prtyGradient.hpp"
#endif

#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif


//============================================================================
//============================================================================
class rmpData
{
public:
	enum RampTexureQuality
	{
		RT_LOW, RT_MED, RT_HIGH, RT_VERY_HIGH, RT_VERY_VERY_HIGH
	};

	enum RampTexureInterpolation
	{
		RT_LINEAR, RT_NONE, RT_SMOOTH, RT_BUMP, RT_SPIKE
	};
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	rmpData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void RegisterCallback(shared_ptr<prtyPropertyCallback> i_pCallback);

	//---------------------------------------------------------------------------
	//	ramp property
	//---------------------------------------------------------------------------	
	prtyEnum		m_Shape;
	prtyEnum		m_Interpolation;
	prtyInt32		m_TexSize;
	prtyGradient	m_Gradient;
	prtyFloat		m_UWave;
	prtyFloat		m_UWaveFreq;
	prtyFloat		m_VWave;
	prtyFloat		m_VWaveFreq;
	prtyFloat		m_Noise;
	prtyFloat		m_NoiseFreq;
	prtyTrigger		m_Copy;
	prtyTrigger		m_Paste;

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	const rmpData& operator = (const rmpData& i_CopyFrom);
};

