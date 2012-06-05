/*****************************************************************************
**  effNormalsData.hpp
**
**      effNormalsData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_NORMALSDATA_HPP
#error effNormalsData.hpp multiply included
#endif
#define EFF_NORMALSDATA_HPP

#ifndef MA_FLOATRGBA_HPP
#include "Core/ma/maFloatRGBA.hpp"
#endif

#ifndef MAT_SHADEREFFECT_HPP
#include "Graphics/mat/matShaderEffect.hpp"
#endif

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
#endif


//============================================================================
//============================================================================
class effNormalsData : public effShaderData
{
public:
	effNormalsData();
	virtual ~effNormalsData();
	effNormalsData(const effNormalsData& i_CopyFrom);
	effNormalsData& operator = (const effNormalsData& i_CopyFrom);
	bool operator == (const effNormalsData& i_EffNormalsData) const;

	virtual effShaderData* Clone() const {return new effNormalsData(*this);}

	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	fsLocator m_NameNormalMap;
	matTexture* m_pNormalMap;
	float m_BumpScale;
};
