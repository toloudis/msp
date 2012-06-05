/*****************************************************************************
**  effReflData.hpp
**
**      effReflData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_REFLDATA_HPP
#error effReflData.hpp multiply included
#endif
#define EFF_REFLDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effReflectionMap
{
public:
	// override near plane for cube maps
	float m_NearPlane;
	bool m_bIsPlanar;
	bool m_bAutoGenEnvMap;
	int m_ReflMapResolution;
	matTexture* m_ReflectionMap;

	effReflectionMap();
	effReflectionMap(const effReflectionMap& i_CopyFrom);
	effReflectionMap& operator = (const effReflectionMap& i_CopyFrom);
	bool operator == (const effReflectionMap& i_EffReflData) const;
};

//============================================================================
//============================================================================
class effReflData : public effShaderData
{
public:
	effReflData();
	virtual ~effReflData();
	effReflData(const effReflData& i_CopyFrom);
	effReflData& operator = (const effReflData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effReflData(*this);}
	virtual bool HasTransparency();
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;
	virtual maFloatRGBA GetDiffuse() const {return m_ColorDiffuse;}
	virtual maFloatRGBA GetAmbient() const {return m_ColorAmbient;}
	virtual maFloatRGBA GetEmissive() const {return m_ColorEmissive;}
	virtual maFloatRGBA GetSpecular() const {return m_ColorSpecular;}
	virtual matTexture* GetTransparencyTexture(){ return m_TextureTransparencyMap; }
	virtual float GetTransparencyValue() const { return m_Transparency; }

	effUVTransform m_UV;
	maFloatRGBA m_ColorAmbient;
	maFloatRGBA m_ColorDiffuse;
	maFloatRGBA m_ColorSpecular;
	maFloatRGBA m_ColorEmissive;

	float m_SpecularPower;
	float m_BumpMapScale;
	float m_Reflectivity;
	float m_Transparency;

	std::string m_NameNormalMap;
	matTexture* m_TextureNormalMap;
	std::string m_NameDiffuseMap;
	matTexture* m_TextureDiffuseMap;
	std::string m_NameSpecularMap;
	matTexture* m_TextureSpecularMap;
	std::string m_NameReflectFactorMap;
	matTexture* m_TextureReflectFactorMap;
	std::string m_NameTransparencyMap;
	matTexture* m_TextureTransparencyMap;

	bool m_bAutoGenEnvMap;
	matTexture* m_ReflectionMap;
	int m_ReflMapResolution;

	// IOR ratio (n1)/(n2) for refraction
	float m_IOR;
	// mipmap LOD value for cubemap (refl and refr separate?)
	float m_ReflLOD;
	float m_RefrLOD;
	float m_ReflFactor;
	float m_RefrFactor;
	float m_FresnelBias;
	float m_FresnelPower;

	// override near plane for cube maps
	float m_NearPlane;

	bool m_bIsPlanar;
};
