//****************************************************************************
//	captRenderProgressData.hpp
//
//	Render Progress Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#pragma once

#ifdef CAPT_RENDERPROGRESSDATA_HPP
#error captRenderPrefsData.hpp multiply included
#endif
#define CAPT_RENDERPROGRESSDATA_HPP

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
//============================================================================
//============================================================================

class captRenderProgressData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	captRenderProgressData();

	float m_FrameTotal;
	float m_FrameCompleted;
	float m_CamerasTotal;
	float m_CamerasCompleted;
	float m_LayersTotal;
	float m_LayersCompleted;
	float m_CurCamPercentage;
	nameString m_CurCamName;
	nameString m_CurLayerName;
	nameString m_CurSceneName;
};

