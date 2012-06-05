/*****************************************************************************
**  effBakeData.cpp
**
**	StudioGPU
**	Copyright(C) 20010 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBakeData.hpp"

#include "Graphics/eff/effBakeDataParser.hpp"
#include "Graphics/eff/effShaderParams.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <vector>
#include <algorithm>
#include <functional>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBakeData::effBakeData()
:	m_ColorEmissive(0.0f, 0.0f, 0.0f, 0.0f),
	m_TextureEmissive(NULL),
	m_TextureNormalMap(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBakeData::~effBakeData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBakeData::effBakeData(const effBakeData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBakeData& effBakeData::operator = (const effBakeData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_ColorEmissive = i_CopyFrom.m_ColorEmissive;
	m_BumpMapScale = i_CopyFrom.m_BumpMapScale;
	m_TextureEmissive = i_CopyFrom.m_TextureEmissive;
	m_TextureNormalMap = i_CopyFrom.m_TextureNormalMap;
	
	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureEmissive) 
		io_Textures.push_back(m_TextureEmissive);
    if (m_TextureNormalMap) 
		io_Textures.push_back(m_TextureNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameEmissive != "")
		o_Names.push_back(m_NameEmissive);
	if (m_NameNormalMap != "")
		o_Names.push_back(m_NameNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameEmissive != "")
		m_TextureEmissive = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameEmissive.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameEmissive != "")
		m_TextureEmissive = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameEmissive.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::RemoveTextures()
{
	m_TextureEmissive = NULL;
    m_TextureNormalMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effBakeData::GetChunkName() const 
{
	return effBakeDataParser::GetChunkName();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBakeData::AddToParams(effShaderParams& o_Params)
{
	o_Params.SetVersion(1);
	o_Params.AddParam(new effParamColor("g_emissive", "g_emissive", m_ColorEmissive));
	o_Params.AddParam(new effParamTexture("emissiveMap", "emissiveMap", itString(m_NameEmissive.c_str())));
	o_Params.AddParam(new effParamTexture("normalMap", "normalMap", itString(m_NameNormalMap.c_str())));
}
