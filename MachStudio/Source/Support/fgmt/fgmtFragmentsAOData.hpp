/*****************************************************************************
**  fgmtFragmentsAOData.hpp
**
**      The fgmtFragmentsAOData represents ambient occlusion settings for 
**	a fgmtScriptObject (typically a character, set, or prop).
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_FRAGMENTSAODATA_HPP
#error fgmtFragmentsAOData.hpp multiply included
#endif
#define FGMT_FRAGMENTSAODATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif

class fgmtFragmentsAOData
{
	public:
		
		fgmtFragmentsAOData()
			:	m_bInherit("Inherit Parent", true),
				m_bIsOccluder("Is Occluder", false),
				m_bReceivesOcclusion("Receive Occlusion", true),
				m_bSelfOccludeOnly("Self Occlude Only", false),
				m_nSamples("Samples", 200),
				m_SamplingResolution("SampleRes", 512),
				m_DepthBias("SampleBias", 0.001f),
				m_TextureResolution("TextureRes", 512),
				m_BlendFactor("Blend Factor", 1.0f),
				m_bTexturesInSceneFolder("Textures Use Scene Folder", true),
				m_bStatic("Static", true),
				m_DistanceCutoff("Distance Cutoff", 1000.0f),
				m_bUseBakedTexture("Use Baked Texture", false)
		{
		}

		// ambient occlusion settings
		prtyBoolean m_bInherit;
		prtyBoolean m_bIsOccluder;
		prtyBoolean m_bReceivesOcclusion;
		prtyBoolean m_bSelfOccludeOnly;
		prtyInt32 m_nSamples;
		prtyInt32 m_SamplingResolution;
		prtyFloat m_DepthBias;
		prtyInt32 m_TextureResolution;
		prtyFloat m_BlendFactor;
		prtyBoolean m_bTexturesInSceneFolder;
		prtyBoolean m_bStatic;
		prtyFloat m_DistanceCutoff;

		prtyBoolean m_bUseBakedTexture;
};

