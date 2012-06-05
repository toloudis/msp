/*****************************************************************************
**  effLambertData.hpp
**
**      effLambertData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_LAMBERTDATA_HPP
#error effLambertData.hpp multiply included
#endif
#define EFF_LAMBERTDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effLambertData : public effShaderData
{
public:
	effLambertData();
	virtual ~effLambertData();
	effLambertData(const effLambertData& i_CopyFrom);
	effLambertData& operator = (const effLambertData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effLambertData(*this);}
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
	virtual maFloatRGBA GetSpecular() const {return maFloatRGBA(0,0,0,0);}
	virtual matTexture* GetTransparencyTexture(){ return m_TextureTransparencyMap; }
	virtual float GetTransparencyValue() const { return m_Transparency; }

	effUVTransform m_UV;
	maFloatRGBA m_ColorAmbient;
	maFloatRGBA m_ColorDiffuse;
	maFloatRGBA m_ColorEmissive;

	float m_BumpMapScale;
	float m_Transparency;

	std::string m_NameDiffuse;
	std::string m_NameNormalMap;
	std::string m_NameTransparencyMap;
	matTexture* m_TextureDiffuse;
	matTexture* m_TextureNormalMap;
	matTexture* m_TextureTransparencyMap;
};
