/*****************************************************************************
**	effToonData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effToonData.hpp"

#include "Graphics/eff/effToonDataParser.hpp"

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
effToonData::effToonData()
:	m_ColorAmbient(0.0f, 0.0f, 0.0f, 1.0f),
	m_ColorMidtone(0.0f, 0.0f, 0.0f, 1.0f),
	m_ColorDiffuse(0.7f, 0.0f, 0.0f, 1.0f),
	m_ColorSpecular(1.0f, 1.0f, 1.0f, 1.0f),
	m_Transparency(1),
	m_Smoothness(500),
	m_TextureDiffuse(NULL),
	m_TextureGradientMap(NULL),
	m_TextureTransparencyMap(NULL),
	m_SpecularEnable(true),
	m_Transition1(0.0f),
	m_Transition2(0.0f),
	m_Transition3(0.98f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effToonData::~effToonData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effToonData::effToonData(const effToonData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effToonData& effToonData::operator = (const effToonData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_UV = i_CopyFrom.m_UV;
	m_ColorAmbient = i_CopyFrom.m_ColorAmbient;
	m_ColorMidtone = i_CopyFrom.m_ColorMidtone;
	m_ColorDiffuse = i_CopyFrom.m_ColorDiffuse;
	m_ColorSpecular = i_CopyFrom.m_ColorSpecular;
	m_Transparency = i_CopyFrom.m_Transparency;
	m_Smoothness = i_CopyFrom.m_Smoothness;
	m_Transition1 = i_CopyFrom.m_Transition1;
	m_Transition2 = i_CopyFrom.m_Transition2;
	m_Transition3 = i_CopyFrom.m_Transition3;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;
	m_TextureGradientMap = i_CopyFrom.m_TextureGradientMap;
	m_TextureTransparencyMap = i_CopyFrom.m_TextureTransparencyMap;
	m_NameDiffuse = i_CopyFrom.m_NameDiffuse ;
	m_NameGradientMap = i_CopyFrom.m_NameGradientMap ;
	m_NameTransparencyMap = i_CopyFrom.m_NameTransparencyMap ;
	m_SpecularEnable = i_CopyFrom.m_SpecularEnable;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effToonData::HasTransparency()
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
void effToonData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureDiffuse) 
		io_Textures.push_back(m_TextureDiffuse);
    if (m_TextureGradientMap) 
		io_Textures.push_back(m_TextureGradientMap);
	if (m_TextureTransparencyMap) 
		io_Textures.push_back(m_TextureTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effToonData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffuse != "")
		o_Names.push_back(m_NameDiffuse);
	if (m_NameGradientMap != "")
		o_Names.push_back(m_NameGradientMap);
	if (m_NameTransparencyMap != "")
		o_Names.push_back(m_NameTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effToonData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffuse.c_str()));
	if (m_NameGradientMap != "")
		m_TextureGradientMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameGradientMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effToonData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffuse.c_str()));
	if (m_NameGradientMap != "")
		m_TextureGradientMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameGradientMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effToonData::RemoveTextures()
{
	m_TextureDiffuse = NULL;
    m_TextureGradientMap = NULL;
	m_TextureTransparencyMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effToonData::GetChunkName() const 
{
	return effToonDataParser::GetChunkName();
}
