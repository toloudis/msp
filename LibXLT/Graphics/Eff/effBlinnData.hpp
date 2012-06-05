/*****************************************************************************
**  effBlinnData.hpp
**
**      effBlinnData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_BLINNDATA_HPP
#error effBlinnData.hpp multiply included
#endif
#define EFF_BLINNDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effBlinnData : public effShaderData
{
public:
	effBlinnData();
	virtual ~effBlinnData();
	effBlinnData(const effBlinnData& i_CopyFrom);
	effBlinnData& operator = (const effBlinnData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effBlinnData(*this);}
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
	float m_IOR;
	float m_DiffuseRoughness;
	float m_Transparency;

	std::string m_NameDiffuse;
	std::string m_NameSpecular;
	std::string m_NameGloss;
	std::string m_NameEnvironment;
	std::string m_NameNormalMap;
	std::string m_NameReflectFactorMap;
	std::string m_NameIORMap;
	std::string m_NameTransparencyMap;
	matTexture* m_TextureDiffuse;
	matTexture* m_TextureSpecular;
	matTexture* m_TextureGloss;
	matTexture* m_TextureEnvironment;
	matTexture* m_TextureNormalMap;
	matTexture* m_TextureReflectFactorMap;
	matTexture* m_TextureIORMap;
	matTexture* m_TextureTransparencyMap;
};
