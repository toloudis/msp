/****************************************************************************\
**  giGIData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef GI_GIDATA_HPP
#error giGIData.hpp multiply included
#endif
#define GI_GIDATA_HPP

#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
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
#ifndef PRTY_NAME_HPP
#include "Core/prty/prtyName.hpp"
#endif


//============================================================================
//============================================================================
class giGIData
{
public:

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	giGIData();

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	giGIData(const giGIData& i_Data);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	~giGIData();

	//-------------------------------------------------
	bool operator == (const giGIData& i_Item);

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	giGIData& operator=(const giGIData& i_Data);

public:

	prtyFloat m_GIRadius;
	prtyFloat m_GIRadiusFar;
	prtyFloat m_AngleBias;
	prtyFloat m_Attenuation;
	prtyFloat m_Contrast;

	prtyFloat m_BlurWidth;
	prtyFloat m_BlurSharpness;

	prtyColor m_Color;
	prtyInt32 m_OverscanPixels;

	prtyName m_Name;

	// RSM GI parameters
	prtyFloat m_RSMGIScale;
	prtyFloat m_RSMGISampleRadius;
	prtyEnum  m_RSMGISampleNum;

	prtyFloat m_GILightScale;
	prtyInt32 m_RSMSize;

	prtyFloat m_LPVScale;
	prtyInt32 m_LPVIteration;
	prtyInt32 m_LPVVolumeSize;
	prtyFloat m_LPVGIFalloff;
	prtyInt32 m_LPVRSMSize;
};
