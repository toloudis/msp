/*****************************************************************************
**  effOcclusionData.hpp
**
**      effOcclusionData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_OCCLUSIONDATA_HPP
#error effOcclusionData.hpp multiply included
#endif
#define EFF_OCCLUSIONDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class matTexture;


//============================================================================
//============================================================================
class effOcclusionData : public effShaderData
{
public:
	effOcclusionData();
	virtual ~effOcclusionData();
	effOcclusionData(const effOcclusionData& i_CopyFrom);
	effOcclusionData& operator = (const effOcclusionData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effOcclusionData(*this);}
	virtual bool HasTransparency();
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
//	virtual chDefs::Name GetChunkName() const;

	// allow AO textures to be loaded even if the matTextureMgr doesn't want to.
	static void SetSkipTextureOverride(bool i_bSkipTextureOverride);
	static bool s_bSkipTextureOverride;

	// needs recalc
	bool m_bAOInvalid;
	// only recalc once
	bool m_bStatic;

	bool m_bInheritParent;

	// only "this" fragment.
	bool m_bSelfOcclude;
	// only "sibling" fragments.
	bool m_bSiblingOcclude;

	float m_BlendFactor;

	// texture size
	int m_Resolution;

	// depth map AO 
	int m_nSamples;
	int m_SamplingResolution;
	float m_DepthBias;
	float m_DistanceCutoff;

	std::string m_TextureName;
	matTexture* m_TextureDiffuse;
};
