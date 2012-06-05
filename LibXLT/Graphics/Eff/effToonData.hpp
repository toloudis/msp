/*****************************************************************************
**  effToonData.hpp
**
**      effToonData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_TOONDATA_HPP
#error effToonData.hpp multiply included
#endif
#define EFF_TOONDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effToonData : public effShaderData
{
public:
	effToonData();
	virtual ~effToonData();
	effToonData(const effToonData& i_CopyFrom);
	effToonData& operator = (const effToonData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effToonData(*this);}
	virtual bool HasTransparency();
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;
	virtual maFloatRGBA GetDiffuse() const {return m_ColorDiffuse;}
	virtual maFloatRGBA GetAmbient() const {return m_ColorAmbient;}
	virtual maFloatRGBA GetSpecular() const {return m_ColorSpecular;}
	virtual matTexture* GetTransparencyTexture(){ return m_TextureTransparencyMap; }
	virtual float GetTransparencyValue() const { return m_Transparency; }

	effUVTransform m_UV;
	maFloatRGBA m_ColorAmbient;
	maFloatRGBA m_ColorMidtone;
	maFloatRGBA m_ColorDiffuse;
	maFloatRGBA m_ColorSpecular;

	float m_Transparency;

	bool m_SpecularEnable;
	float m_Smoothness;

	float m_Transition1;
	float m_Transition2;
	float m_Transition3;

	std::string m_NameDiffuse;
	std::string m_NameGradientMap;
	std::string m_NameTransparencyMap;
	matTexture* m_TextureDiffuse;
	matTexture* m_TextureGradientMap;
	matTexture* m_TextureTransparencyMap;
};
