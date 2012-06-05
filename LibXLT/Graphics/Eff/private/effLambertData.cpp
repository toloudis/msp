/*****************************************************************************
**  effLambertData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effLambertData.hpp"

#include "Graphics/eff/effLambertDataParser.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <vector>
#include <algorithm>
#include <functional>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLambertData::effLambertData()
:	m_ColorAmbient(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorDiffuse(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorEmissive(0.0f, 0.0f, 0.0f, 0.0f),
	m_BumpMapScale(1),
	m_Transparency(1),
	m_TextureDiffuse(NULL),
	m_TextureNormalMap(NULL),
	m_TextureTransparencyMap(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLambertData::~effLambertData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLambertData::effLambertData(const effLambertData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effLambertData& effLambertData::operator = (const effLambertData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_UV = i_CopyFrom.m_UV;
	m_ColorAmbient = i_CopyFrom.m_ColorAmbient;
	m_ColorDiffuse = i_CopyFrom.m_ColorDiffuse;
	m_ColorEmissive = i_CopyFrom.m_ColorEmissive;
	m_BumpMapScale = i_CopyFrom.m_BumpMapScale;
	m_Transparency = i_CopyFrom.m_Transparency;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;
	m_TextureNormalMap = i_CopyFrom.m_TextureNormalMap;
	m_TextureTransparencyMap = i_CopyFrom.m_TextureTransparencyMap;
	m_NameDiffuse = i_CopyFrom.m_NameDiffuse ;
	m_NameNormalMap = i_CopyFrom.m_NameNormalMap ;
	m_NameTransparencyMap = i_CopyFrom.m_NameTransparencyMap ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effLambertData::HasTransparency()
{
	if( m_Transparency != 1.0f )
	{
		return true;
	}

	if( m_TextureTransparencyMap )		//ignore whether the texture has alpha, since only r channel is used
	{
		return true;
	}

	if( m_ColorDiffuse.GetAlpha() != 1.0f )
	{
		return true;
	}

	std::vector<matTexture*> textures;
	GetTextures(textures);
	//	It is also probably transparent if all of the textures have alpha channels
	if( textures.size() == 0 )
	{
		return false;
	}

	int num_trans = 0;
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i]->HasTransparency())	num_trans++;
	}
	return (num_trans >= textures.size());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effLambertData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureDiffuse) 
		io_Textures.push_back(m_TextureDiffuse);
    if (m_TextureNormalMap) 
		io_Textures.push_back(m_TextureNormalMap);
	if (m_TextureTransparencyMap) 
		io_Textures.push_back(m_TextureTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effLambertData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffuse != "")
		o_Names.push_back(m_NameDiffuse);
	if (m_NameNormalMap != "")
		o_Names.push_back(m_NameNormalMap);
	if (m_NameTransparencyMap != "")
		o_Names.push_back(m_NameTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effLambertData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffuse.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effLambertData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffuse.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effLambertData::RemoveTextures()
{
	m_TextureDiffuse = NULL;
    m_TextureNormalMap = NULL;
	m_TextureTransparencyMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effLambertData::GetChunkName() const 
{
	return effLambertDataParser::GetChunkName();
}
