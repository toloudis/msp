/*****************************************************************************
**  effSkinData.hpp
**
**      effSkinData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_SKINDATA_HPP
#error effSkinData.hpp multiply included
#endif
#define EFF_SKINDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effSkinData : public effShaderData
{
public:
	effSkinData();
	virtual ~effSkinData();
	effSkinData(const effSkinData& i_CopyFrom);
	effSkinData& operator = (const effSkinData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effSkinData(*this);}
	virtual bool HasTransparency();
	virtual bool DidTextureChange(const effShaderData* i_NewData) const;
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;
	virtual maFloatRGBA GetDiffuse() const {return m_TransColIn;}
	virtual maFloatRGBA GetAmbient() const {return m_TransColIn;}
//	virtual maFloatRGBA GetEmissive() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetSpecular() const {return m_SpecColor;}
	virtual matTexture* GetTransparencyTexture(){ return m_TransparencyTex; }
	virtual float GetTransparencyValue() const { return m_Transparency; }


	effUVTransform m_UV;
	maFloatRGBA m_SpecColor;
	float m_SpecPower;
	float m_SpecGloss;
	float m_SpecFresnel;
	float m_FresnelPower;
	float m_FresnelGloss;
	maFloatRGBA m_TransColIn;
	maFloatRGBA m_TransColOut;
	maFloatRGBA m_TransColBack;
	float m_TransMultiplier;
	float m_TransRampOff;
	float m_MicroScale;

	std::string m_NameDiffTex;
	std::string m_NameNormalTex;
	std::string m_NameMicroTex;
	std::string m_NameSpecTex;
	std::string m_NameSpecPowerTex;
	std::string m_NameTransTex;
	std::string m_NameCubeMapTex;
	std::string m_NameReflectFactorTex;
	std::string m_NameTransparencyTex;
	matTexture* m_DiffTex;
	matTexture* m_NormalTex;
	matTexture* m_MicroTex;
	matTexture* m_SpecTex;
	matTexture* m_SpecPowerTex;
	matTexture* m_TransTex;
	matTexture* m_CubeMapTex;
	matTexture* m_ReflectFactorTex;
	matTexture* m_TransparencyTex;

	float m_BumpMapScale;
	float m_Transparency;
};
