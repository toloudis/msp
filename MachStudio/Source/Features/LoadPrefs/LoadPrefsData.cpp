/****************************************************************************\
**  LoadPrefsData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004-7 - All Rights Reserved
\****************************************************************************/
#include "Features/LoadPrefs/LoadPrefsData.hpp"

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
LoadPrefsData::LoadPrefsData()
:	m_bNeverLoadTextures("Skip All Textures", false),
	m_bAllowMissingTextures("Allow Missing Textures", false),
	m_TextureReduce("Reduce Texture Size", 0),
	m_bNoDepthMaps("No Depth Maps", false),
	m_DepthMapReduce("Reduce Depth Map Size", 0),
	m_MaxSubdivLevel("Max Subdiv Level", 2),
	m_bOptimizeMeshes("Optimize Meshes", true),
	m_bComputeBasisVectors("Compute Basis Vectors", true),
	m_bGeometryInVideoMemory("Geometry in Video Memory", true),
	m_bSkipHighRes("Skip Loading High-Res", false),
	m_bSkipLowRes("Skip Loading Low-Res", false),
	m_bAutoGenLowRes("Auto Generate Low-Res", true),
	m_bNeverLoadAnimation("Never Load Animation", false),
	m_bDelayLoadingAnimation("Delay Loading Animation", false),
	m_bNeverLoadSounds("Never Load Sounds", false),
	m_bAlwaysLoadAOTextures("Always Load AO Textures", false),
	m_bAutoDXTCompress("Auto Compress Textures", false)
{
}


