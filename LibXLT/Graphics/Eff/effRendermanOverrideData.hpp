/*****************************************************************************
**  effRendermanOverrideData.hpp
**
**      effRendermanOverrideData is the data for a shader effect.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef EFF_RENDERMANOVERRIDEDATA_HPP
#error effRendermanOverrideData.hpp multiply included
#endif
#define EFF_RENDERMANOVERRIDEDATA_HPP

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
class effRendermanOverrideData : public effShaderData
{
public:
	effRendermanOverrideData();
	virtual ~effRendermanOverrideData();
	effRendermanOverrideData(const effRendermanOverrideData& i_CopyFrom);
	effRendermanOverrideData& operator = (const effRendermanOverrideData& i_CopyFrom);
	bool operator == (const effRendermanOverrideData& i_EffRendermanOverrideData) const;

	virtual effShaderData* Clone() const {return new effRendermanOverrideData(*this);}

	virtual void GetTextures(std::vector<matTexture*>& io_Textures) const;
	virtual void GetTextureNames(std::vector<std::string>& o_Names) const;
	virtual void ReloadTextures(const fsLocator& i_TextureDirectory);
	virtual void ReloadTextures(const fsResourceFinder& i_TextureFinder);
	virtual void RemoveTextures();
	virtual chDefs::Name GetChunkName() const;

	fsLocator m_ShaderLocation;
	std::string m_ParamList;
	std::string m_AttributeList;
	bool m_bOverrideShader;
	bool m_bOverrideAttributes;
};
