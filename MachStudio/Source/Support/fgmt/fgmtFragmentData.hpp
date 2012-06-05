/*****************************************************************************
**  fgmtFragmentData.h
**
**      The fgmtFragmentData represents all intensive visual properties of a
**	surface.  It includes information about the ambient, diffuse, and
**	specular colors, and the texture maps applied to the surface.
**
**	StudioGPU
**	Copyright(C) 2002 - All Rights Reserved
\****************************************************************************/

#ifdef FGMT_FRAGMENTDATA_HPP
#error fgmtFragmentData.hpp multiply included
#endif
#define FGMT_FRAGMENTDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_TEXT_HPP
#include "Core/prty/prtyText.hpp"
#endif

#include <string>


class fgmtFragmentData
{
	public:
		
		fgmtFragmentData()
			:	m_bCastsShadow("Casts Shadow", false),
				m_bReceivesShadow("Receives Shadow", false),
				m_bShadowHull("Shadow Hull", false),
				m_bDoubleSided("Double Sided", false),
				m_bUseBakedTexture("Use Baked Texture", false),

				m_bAOInherit("Inherit Parent", true),
				m_bIsOccluder("Is Occluder", false),
				m_bReceivesOcclusion("Receive Occlusion", true),
				m_bReceivesGI("Receive GI", true),
				m_bSiblingOccludeOnly("Sibling Occlude Only", false),
				m_bSelfOccludeOnly("Self Occlude Only", false),
				m_bStaticAO("Static", true),
				m_nSamples("Samples", 200),
				m_SamplingResolution("SampleRes", 512),
				m_DepthBias("SampleBias", 0.001f),
				m_AOTextureResolution("TextureRes", 512),

				m_AOBlendFactor("Blend Factor", 1.0f),
				m_AOUserTextureName("User Texture"),
				m_AODistanceCutoff("Distance Cutoff", 1000.0f)
		{
		}

		//----------------------------------------------------------------------------
		//	Fragment Name
		//----------------------------------------------------------------------------
		inline void SetFragmentName(const std::string& i_Name);
		inline const std::string& GetFragmentName() const;

		// casts shadows
		prtyBoolean m_bCastsShadow;
		// receives light from shadow sources
		prtyBoolean m_bReceivesShadow;
		// fragment is invisible, but casts shadow
		prtyBoolean m_bShadowHull;
		// render both sides of triangles
		prtyBoolean m_bDoubleSided;

		// Ambient Occlusion options:
		prtyBoolean m_bAOInherit;
		prtyBoolean m_bIsOccluder;
		prtyBoolean m_bReceivesOcclusion;
		prtyBoolean m_bReceivesGI;
		prtyBoolean m_bSiblingOccludeOnly;
		prtyBoolean m_bSelfOccludeOnly;
		prtyBoolean m_bStaticAO;
		prtyInt32 m_nSamples;
		prtyInt32 m_SamplingResolution;
		prtyFloat m_DepthBias;
		prtyInt32 m_AOTextureResolution;
		prtyFloat m_AOBlendFactor;
		prtyFileName m_AOUserTextureName;
		prtyFloat m_AODistanceCutoff;

		// baked texture data, if there's one
		prtyBoolean		m_bUseBakedTexture;
		prtyBoolean		m_bHasBeenBaked;
		prtyFilePath	m_BakedTextureLocation;		// Dir that contains bake texture dir
		prtyText		m_BakedTextureFormat;
private:
		std::string m_FragmentName;
};

//====================================================================
//	Fragment Name
//====================================================================
inline void fgmtFragmentData::SetFragmentName(const std::string& i_Name)
{
	m_FragmentName = i_Name;
}
inline const std::string& fgmtFragmentData::GetFragmentName() const
{
	return m_FragmentName;
}

