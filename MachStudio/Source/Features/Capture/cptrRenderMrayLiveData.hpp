//****************************************************************************
//	cptrRenderMrayLiveData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#pragma once

#ifdef CPTR_RENDERMRAYLIVEDATA_HPP
#error cptrRenderMrayLiveData.hpp multiply included
#endif
#define CPTR_RENDERMRAYLIVEDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_TRIGGER_HPP
#include "Core/prty/prtyTrigger.hpp"
#endif

#include <map>

//============================================================================
//============================================================================
class cptrRenderMrayLiveData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	cptrRenderMrayLiveData();

	prtyInt32	m_MrayVerbosity;
	prtyInt32	m_MrayNumReflBounces;
	prtyInt32	m_MrayNumRefrBounces;
	prtyInt32	m_MrayMaxTraceDepth;
	prtyBoolean	m_bMrayAO;
	prtyInt32	m_MrayAOSamples;
	prtyBoolean	m_bMrayFinalGather;
	prtyBoolean	m_bMrayFGBlur;
	prtyInt32	m_MrayFGNDiffuse;
	prtyInt32	m_MrayFGNRefl;
	prtyInt32	m_MrayFGNRefr;
	prtyInt32	m_MrayFGNRays;
	prtyColor	m_MrayFGColor;
	prtyEnum	m_MrayOutputFormat;
	prtyInt32	m_MrayNumThreads;
	prtyInt32	m_MrayMemoryLimit;
	prtyBoolean	m_bMrayEnableReflections;
	prtyBoolean	m_bMrayEnableShadows;
	prtyEnum	m_MrayShadowType;
	prtyBoolean	m_bMrayRewriteAssets;
	prtyEnum	m_MrayVerbosityLevel;
	prtyBoolean	m_bMrayFGMapEnable;
	prtyEnum	m_MrayFGMapRebuild;
	prtyFilePath m_MrayFGMapPath;
	prtyInt32	m_MrayReflSamples;
	prtyBoolean	m_bMrayOverrideMSPSampling;
	prtyInt32	m_MrayMinCaptureSamples;
	prtyInt32	m_MrayMaxCaptureSamples;
	prtyFloat	m_MRayAAContrast;
	prtyBoolean	m_bMrayIgnoreBadTex;
	prtyBoolean	m_bMrayDisplayPreview;
	prtyBoolean m_bMRayTonemapEnable;
	prtyBoolean m_bEnableIBL;
	prtyFloat	m_IBLQuality;
	prtyEnum	m_IBLMapRes;
	prtyFloat	m_IBLScale;
	prtyInt32	m_IBLSampleNum;
	prtyBoolean	m_bMrayProgressive;
	prtyInt32	m_MrayProgSubsamplingSize;
	prtyEnum	m_MrayProgSubsamplingMode;
	prtyEnum	m_MrayProgSubsamplingPattern;
	prtyInt32	m_MrayProgMinSamples;
	prtyInt32	m_MrayProgMaxSamples;
	prtyInt32	m_MrayProgMaxTime;
	prtyFloat	m_MrayProgErrorThreshold;
	prtyEnum	m_MrayRenderPass;
	prtyTrigger m_bStartStop;
	prtyFloat	m_MrayPixelFilterSize;
	prtyEnum	m_MrayPixelFilterType;

	prtyTrigger m_CompositeAO;
	prtyTrigger m_CompositeFG;
	prtyTrigger m_CompositeRefl;
	prtyTrigger m_CompositeBeauty;
};

