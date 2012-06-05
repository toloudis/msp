/*****************************************************************************
**  effDisplacementData.hpp
**
**      effDisplacementData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef EFF_DISPLACEMENTDATA_HPP
#error effDisplacementData.hpp multiply included
#endif
#define EFF_DISPLACEMENTDATA_HPP

#ifndef FS_LOCATOR_HPP
#include "Core/fs/fsLocator.hpp"
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
class effDisplacementData : public effShaderData
{
public:
	effDisplacementData();
	virtual ~effDisplacementData();
	effDisplacementData(const effDisplacementData& i_CopyFrom);
	effDisplacementData& operator = (const effDisplacementData& i_CopyFrom);
	bool operator == (const effDisplacementData& i_EffDisplacementData) const;

	virtual effShaderData* Clone() const {return new effDisplacementData(*this);}
	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	float m_Scale;
	float m_Bias;
	float m_Blur;
	float m_TessellationValue;
	maVector2d m_ObjUVScale;

	fsLocator m_NameDisplacementMap;
	matTexture* m_pDisplacementMap;
};
