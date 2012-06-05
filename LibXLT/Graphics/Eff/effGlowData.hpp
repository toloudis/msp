/*****************************************************************************
**  effGlowData.hpp
**
**      effGlowData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_GLOWDATA_HPP
#error effGlowData.hpp multiply included
#endif
#define EFF_GLOWDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif


//============================================================================
//============================================================================
class effGlowData : public effShaderData
{
public:
	effGlowData();
	virtual ~effGlowData();
	effGlowData(const effGlowData& i_CopyFrom);
	effGlowData& operator = (const effGlowData& i_CopyFrom);
	bool operator == (const effGlowData& i_EffGlowData) const;

	virtual effShaderData* Clone() const {return new effGlowData(*this);}

	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	fsLocator m_NameGlowMask;
	matTexture* m_pGlowMask;

	matTexture* m_pTexture;
	float m_GlowAmount;
	maVector4d m_GlowScale;
	float m_GlowSize;
	bool m_bConstantGlow;
	maVector4d m_SrcSizeInfo;
};
