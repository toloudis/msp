//****************************************************************************
//	cptrRenderRmanLiveData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#pragma once

#ifdef CPTR_RENDERRMANLIVEDATA_HPP
#error cptrRenderRmanLiveData.hpp multiply included
#endif
#define CPTR_RENDERRMANLIVEDATA_HPP

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
class cptrRenderRmanLiveData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	cptrRenderRmanLiveData();

	//---------------------------------------------------------------------------
	//	RenderMan data
	//---------------------------------------------------------------------------
	prtyFloat	m_RmanShadingRate;
	prtyInt32	m_nRmanAArate;
	prtyInt32	m_RmanAOsamples;
	prtyBoolean m_bRmanCacheTextures;
	prtyBoolean m_bRmanDisableWarnings;
	prtyBoolean m_bRmanAOEnable;
	prtyFloat	m_RmanAOMaxVariation;
	prtyBoolean m_bRmanReflEnable;	
	prtyEnum	m_RmanReflType;
	prtyBoolean m_bRmanShadowEnable;
	prtyEnum	m_RmanShadowType;
	prtyBoolean m_bRmanGIEnable;
	prtyInt32	m_RmanGIsamples;
	prtyFloat	m_RmanGIMaxVariation;
	prtyEnum	m_RmanFilterType;
	prtyFloat	m_RmanFilterWidth;
	prtyInt32	m_RmanNumCores;
	prtyInt32	m_RmanTexMemory;
	prtyEnum	m_RmanBucketOrder;
	prtyInt32	m_RmanBucketSize;
	prtyInt32	m_RmanRayDepth;
	prtyBoolean m_bRmanTonemapEnable;

	prtyEnum	m_RmanRenderPass;
	prtyTrigger m_bStartStop;
};

