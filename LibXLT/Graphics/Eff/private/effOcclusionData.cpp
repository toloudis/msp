/*****************************************************************************
**  effOcclusionData.hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effOcclusionData.hpp"

//#include "effOcclusionDataParser.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <vector>
#include <algorithm>
#include <functional>


//============================================================================
//============================================================================
namespace
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g2dPFD GetAOPFD()
	{
		// can change this to 32 bit float. as it is, this is 8 bit grey rgba.
//		g2dPFD pfd (g2dPFD::e_Color, 32);
//		pfd.Set(0,8, 8,8, 16,8, 24,8, 32);
	//	g2dPFD pfd (g2dPFD::e_Float32, 32);
	//	g2dPFD pfd (g2dPFD::e_Float16, 64);

		// need a HDR texture format (that won't saturate at 1) with alpha. 
		// also the shader stores weight factors in B.


//		g2dPFD pfd (g2dPFD::e_RGBA16f, 64);

		g2dPFD pfd (g2dPFD::e_Color, 32);
		pfd.Set(0,8, 8,8, 16,8, 24,8, 32);

		return pfd;
	}
};


//----------------------------------------------------------------------------
// allow AO textures to be loaded even if the matTextureMgr doesn't want to.
//----------------------------------------------------------------------------
bool effOcclusionData::s_bSkipTextureOverride = false;


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOcclusionData::effOcclusionData()
:	m_bAOInvalid(false),
	m_bStatic(true),
	m_bInheritParent(true),
	m_bSelfOcclude(false),
	m_bSiblingOcclude(false),
	m_BlendFactor(1.0f),
	m_Resolution(512),
	m_nSamples(200),
	m_SamplingResolution(512),
	m_DepthBias(0.001f),
	m_DistanceCutoff(1000.0f),
	m_TextureDiffuse(NULL),
	m_TextureName("")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOcclusionData::~effOcclusionData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOcclusionData::effOcclusionData(const effOcclusionData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effOcclusionData& effOcclusionData::operator = (const effOcclusionData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_bAOInvalid = i_CopyFrom.m_bAOInvalid;
	m_bStatic = i_CopyFrom.m_bStatic;
	m_bInheritParent = i_CopyFrom.m_bInheritParent;
	m_bSelfOcclude = i_CopyFrom.m_bSelfOcclude;
	m_bSiblingOcclude = i_CopyFrom.m_bSiblingOcclude;
	m_BlendFactor = i_CopyFrom.m_BlendFactor;
	m_Resolution = i_CopyFrom.m_Resolution;
	m_nSamples = i_CopyFrom.m_nSamples;
	m_SamplingResolution = i_CopyFrom.m_SamplingResolution;
	m_DepthBias = i_CopyFrom.m_DepthBias;
	m_DistanceCutoff = i_CopyFrom.m_DistanceCutoff;
	m_TextureName = i_CopyFrom.m_TextureName;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effOcclusionData::HasTransparency()
{
	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOcclusionData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureDiffuse)
		io_Textures.push_back(m_TextureDiffuse);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOcclusionData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_TextureName != "")
		o_Names.push_back(m_TextureName);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOcclusionData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	g2dPFD pfd = GetAOPFD();

	// first time here, if there is a filename, then load from disk.
	// hereafter, just create an empty render target.
	if (m_TextureName != "")
	{
		bool skip = matTextureMgr::IsSkipAllTextures();
		if (s_bSkipTextureOverride && skip)
		{
			matTextureMgr::SetSkipAllTextures(false);
		}
		matTexture* loadedTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_TextureName.c_str()));
		if (s_bSkipTextureOverride && skip)
		{
			matTextureMgr::SetSkipAllTextures(true);
		}
	
//		m_TextureDiffuse = matTextureMgr::CreateRenderTargetTexture(loadedTex, true);
		// release now because we are not going to load this texture again this session.
//		matTextureMgr::ReleaseTexture(loadedTex);

		m_TextureDiffuse = loadedTex;
	}
	else
	{
		m_TextureDiffuse = 
			matTextureMgr::CreateRenderTargetTexture(m_Resolution,m_Resolution,false,&pfd,true);
		matTextureMgr::FillTexture(m_TextureDiffuse, maFloatRGBA(1,1,1,1));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOcclusionData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	g2dPFD pfd = GetAOPFD();

	if (m_TextureName != "")
	{
		bool skip = matTextureMgr::IsSkipAllTextures();
		if (s_bSkipTextureOverride && skip)
		{
			matTextureMgr::SetSkipAllTextures(false);
		}
		matTexture* loadedTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_TextureName.c_str()));
		if (s_bSkipTextureOverride && skip)
		{
			matTextureMgr::SetSkipAllTextures(true);
		}
	
		m_TextureDiffuse = loadedTex;
	}
	else
	{
		m_TextureDiffuse = 
			matTextureMgr::CreateRenderTargetTexture(m_Resolution,m_Resolution,false,&pfd,true);
		matTextureMgr::FillTexture(m_TextureDiffuse, maFloatRGBA(1,1,1,1));
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effOcclusionData::RemoveTextures()
{
	m_TextureDiffuse = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//chDefs::Name effOcclusionData::GetChunkName() const 
//{
//	return effOcclusionDataParser::GetChunkName();
//}

//----------------------------------------------------------------------------
// allow AO textures to be loaded even if the matTextureMgr doesn't want to.
//----------------------------------------------------------------------------
void effOcclusionData::SetSkipTextureOverride(bool i_bSkipTextureOverride)
{
	s_bSkipTextureOverride = i_bSkipTextureOverride;
}
