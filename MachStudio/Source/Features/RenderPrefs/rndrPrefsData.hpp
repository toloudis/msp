/********************************************************************************************\
**  rndrPrefsData.hpp
**
**
**  StudioGPU
**  Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef RNDRPREFSDATA_HPP
#error rndrPrefsData.hpp multiply included
#endif
#define RNDRPREFSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif

#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif

#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif

#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif

#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif

#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif 

//============================================================================
//============================================================================
class rndrPrefsData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	rndrPrefsData();

	//---------------------------------------------------------------------------
	//	Shadows data
	//---------------------------------------------------------------------------
	prtyBoolean m_bShadowsOn;

	//---------------------------------------------------------------------------
	//	Fur data
	//---------------------------------------------------------------------------
	//prtyBoolean	m_bEnableFur;
	//prtyFloat m_FurQuality;

	//---------------------------------------------------------------------------
	//	DOF data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableDOF;

	//---------------------------------------------------------------------------
	//	Glow data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableGlow;

	//---------------------------------------------------------------------------
	//	Matte data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bMatteMode;

	//---------------------------------------------------------------------------
	//	Headlight - directional light in direction of camera
	//---------------------------------------------------------------------------
	//prtyBoolean m_bHeadlightOn;

	//---------------------------------------------------------------------------
	//	Outline data
	//---------------------------------------------------------------------------
	//prtyBoolean	m_bEnableOutline;

	prtyEnum m_RendererType;

	prtyBoolean m_bToneMap;
//	prtyBoolean m_bBlueShift;
	prtyEnum m_HDRDebugMode;

	//prtyBoolean m_bEnableAmbientPass;
	prtyBoolean m_bEnableLitPass;
	prtyBoolean m_bEnableTransparent;

	prtyBoolean	m_bEnableReflection;
	prtyBoolean m_bEnableEnvironment;

	prtyBoolean m_bLowResolution;

	//prtyBoolean	m_bEnableAO;
	//prtyBoolean m_bRecalcAOPerFrame;
	
	prtyBoolean m_bProjLightFrustumCull;
	prtyBoolean m_bProjLightsOn;
	prtyBoolean m_bPtLightsOn;
	prtyBoolean m_bDoShadowMapGen;

	//prtyBoolean m_bEnableDeferredTransparency;
	prtyBoolean m_bHDRAA;

	prtyBoolean m_bEnableSSAO;
	enum eSSAOPreset
	{
		e_SSAOLow, e_SSAOMed, e_SSAOHigh, e_SSAOCustom
	};
	prtyEnum m_SSAOQuality;//preset steps and dirs.
	enum eSSAOPresetVP
	{
		e_SSAOLowVP, e_SSAOMedVP,e_SSAOCustomVP
	};
	prtyEnum m_SSAOQualityVP;//preset steps and dirs.
	prtyFloat m_SSAONumSteps;
	prtyInt8 m_SSAONumDirs;
	prtyBoolean m_SSAOEnableBlur;
};

