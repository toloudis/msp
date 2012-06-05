/*****************************************************************************
**  effTextureFilterData.hpp
**
**      effTextureFilterData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_TEXTUREFILTERDATA_HPP
#error effTextureFilterData.hpp multiply included
#endif
#define EFF_TEXTUREFILTERDATA_HPP

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
class effTextureFilterData : public effShaderData
{
public:
	effTextureFilterData();
	virtual ~effTextureFilterData();
	effTextureFilterData(const effTextureFilterData& i_CopyFrom);
	effTextureFilterData& operator = (const effTextureFilterData& i_CopyFrom);
	bool operator == (const effTextureFilterData& i_EffTextureFilterData) const;

	virtual effShaderData* Clone() const {return new effTextureFilterData(*this);}

	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	bool m_bEnableMipmap;
};
