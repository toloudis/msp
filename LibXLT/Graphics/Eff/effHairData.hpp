/*****************************************************************************
**  effHairData.hpp
**
**      effHairData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_HAIRDATA_HPP
#error effHairData.hpp multiply included
#endif
#define EFF_HAIRDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effHairData : public effShaderData
{
public:
	effHairData();
	virtual ~effHairData();
	effHairData(const effHairData& i_CopyFrom);
	effHairData& operator = (const effHairData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effHairData(*this);}
	virtual bool HasTransparency();
	virtual bool DidTextureChange(const effShaderData* i_NewData) const;
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;
//	virtual maFloatRGBA GetDiffuse() const {return m_HairBaseColor;}
	virtual maFloatRGBA GetAmbient() const {return m_HairBaseColor;}
//	virtual maFloatRGBA GetEmissive() const {return m_ColorEmissive;}
//	virtual maFloatRGBA GetSpecular() const {return m_SpecularColor0;}

	effUVTransform m_UV;
	// hilight colors
	maFloatRGBA m_SpecularColor0;
	maFloatRGBA m_SpecularColor1;
	// 2 specular exponents (0..200)
	float m_SpecularExponent0, m_SpecularExponent1;
	// diffuse color
	maFloatRGBA m_HairBaseColor;
	// 2 different specular shift values (-1..1)
	float m_SpecularShift0,m_SpecularShift1;

	float m_BumpMapScale;
	float m_Reflectivity;

	std::string m_NameBase;
	std::string m_NameAlpha;
	std::string m_NameSpecularShift;
	std::string m_NameSpecularMask;
	std::string m_NameNormalMap;
	matTexture* m_TextureBase;
	matTexture* m_TextureAlpha;
	matTexture* m_TextureSpecularShift;
	matTexture* m_TextureSpecularMask;
	matTexture* m_TextureNormalMap;
};
