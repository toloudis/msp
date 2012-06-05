/*****************************************************************************
**  effBakeData.hpp
**
**      effBakeData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_BAKEDATA_HPP
#error effBakeData.hpp multiply included
#endif
#define EFF_BAKEDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/Fs/fsLocator.hpp"
#endif 
#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif
#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#include <string>


//============================================================================
//============================================================================
class effBakeData : public effShaderData
{
public:
	effBakeData();
	virtual ~effBakeData();
	effBakeData(const effBakeData& i_CopyFrom);
	effBakeData& operator = (const effBakeData& i_CopyFrom);

	virtual effShaderData* Clone() const {return new effBakeData(*this);}
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;
	virtual maFloatRGBA GetEmissive() const {return m_ColorEmissive;}
	virtual maFloatRGBA GetDiffuse() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetAmbient() const {return maFloatRGBA(0,0,0,0);}
	virtual maFloatRGBA GetSpecular() const {return maFloatRGBA(0,0,0,0);}

	maFloatRGBA m_ColorEmissive;

	float m_BumpMapScale;

	// These std::string fields are kept around for legacy code,
	// exporters using effPhongData should use the fsLocator variation
	// of the Diffuse map when using full paths.
	fsLocator	m_FullpathDiffuse;

	std::string m_NameEmissive;
	std::string m_NameNormalMap;
	
	matTexture* m_TextureEmissive;
	matTexture* m_TextureNormalMap;

	void AddToParams(effShaderParams& o_Params);
};
