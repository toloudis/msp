/*****************************************************************************
**  effParticleData.hpp
**
**      effParticleData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_PARTICLEDATA_HPP
#error effParticleData.hpp multiply included
#endif
#define EFF_PARTICLEDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effParticleData : public effShaderData
{
public:
	effParticleData();
	virtual ~effParticleData();
	effParticleData(const effParticleData& i_CopyFrom);
	effParticleData& operator = (const effParticleData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effParticleData(*this);}
	virtual bool HasTransparency();
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual maFloatRGBA GetDiffuse() const {return m_ColorDiffuse;}
	virtual maFloatRGBA GetAmbient() const {return m_ColorAmbient;}
	virtual maFloatRGBA GetEmissive() const {return m_ColorEmissive;}
	virtual maFloatRGBA GetSpecular() const {return m_ColorSpecular;}

	maFloatRGBA m_ColorAmbient;
	maFloatRGBA m_ColorDiffuse;
	maFloatRGBA m_ColorSpecular;
	maFloatRGBA m_ColorEmissive;

	float m_SpecularPower;

	std::string m_NameDiffuse;
	matTexture* m_TextureDiffuse;
};
