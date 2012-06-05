/********************************************************************************************\
**  LoadPrefsData.hpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef LOADPREFSDATA_HPP
#error LoadPrefsData.hpp multiply included
#endif
#define LOADPREFSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif

//============================================================================
//============================================================================
class LoadPrefsData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	LoadPrefsData();

	//---------------------------------------------------------------------------
	//	Textures
	//---------------------------------------------------------------------------
	prtyBoolean	m_bNeverLoadTextures;
	prtyBoolean	m_bAllowMissingTextures;
	prtyBoolean m_bAlwaysLoadAOTextures;
	prtyInt8	m_TextureReduce;	

	//---------------------------------------------------------------------------
	//	DepthMaps
	//---------------------------------------------------------------------------
	prtyBoolean	m_bNoDepthMaps;
	prtyInt8	m_DepthMapReduce;	

	//---------------------------------------------------------------------------
	//	Surfaces
	//---------------------------------------------------------------------------
	prtyInt8	m_MaxSubdivLevel;	
	prtyBoolean	m_bOptimizeMeshes;	
	prtyBoolean	m_bComputeBasisVectors;
	prtyBoolean	m_bGeometryInVideoMemory;	

	//---------------------------------------------------------------------------
	//	Resolutions
	//---------------------------------------------------------------------------
	prtyBoolean	m_bSkipHighRes;
	prtyBoolean	m_bSkipLowRes;
	prtyBoolean	m_bAutoGenLowRes;

	//---------------------------------------------------------------------------
	//	Animation
	//---------------------------------------------------------------------------
	prtyBoolean	m_bNeverLoadAnimation;
	prtyBoolean	m_bDelayLoadingAnimation;

	//---------------------------------------------------------------------------
	//	Sounds
	//---------------------------------------------------------------------------
	prtyBoolean	m_bNeverLoadSounds;

};

