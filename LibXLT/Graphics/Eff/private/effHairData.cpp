/*****************************************************************************
**	effHairData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effHairData.hpp"

#include "Graphics/eff/effHairDataParser.hpp"

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
effHairData::effHairData()
:	m_HairBaseColor(1.0f, 1.0f, 1.0f, 1.0f),
	m_BumpMapScale(1),
	m_Reflectivity(1),
	m_TextureBase(NULL),
	m_TextureAlpha(NULL),
	m_TextureSpecularShift(NULL),
	m_TextureSpecularMask(NULL),
	m_TextureNormalMap(NULL),
	m_SpecularColor0(1,0,0,1),
	m_SpecularColor1(0,1,0,1),
	m_SpecularExponent0(200),
	m_SpecularExponent1(200),
	m_SpecularShift0(-.15f),
	m_SpecularShift1(.15f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effHairData::~effHairData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effHairData::effHairData(const effHairData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effHairData& effHairData::operator = (const effHairData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_UV = i_CopyFrom.m_UV;
	m_SpecularColor0 = i_CopyFrom.m_SpecularColor0;
	m_SpecularColor1 = i_CopyFrom.m_SpecularColor1;
	m_SpecularExponent0 = i_CopyFrom.m_SpecularExponent0;
	m_SpecularExponent1 = i_CopyFrom.m_SpecularExponent1;
	m_HairBaseColor = i_CopyFrom.m_HairBaseColor;
	m_SpecularShift0 = i_CopyFrom.m_SpecularShift0;
	m_SpecularShift1 = i_CopyFrom.m_SpecularShift1;
	m_BumpMapScale = i_CopyFrom.m_BumpMapScale;
	m_Reflectivity = i_CopyFrom.m_Reflectivity;
	m_TextureBase = i_CopyFrom.m_TextureBase ;
	m_TextureAlpha = i_CopyFrom.m_TextureAlpha; 
	m_TextureSpecularShift = i_CopyFrom.m_TextureSpecularShift ;
	m_TextureSpecularMask = i_CopyFrom.m_TextureSpecularMask ;
	m_TextureNormalMap = i_CopyFrom.m_TextureNormalMap ;
	m_NameBase = i_CopyFrom.m_NameBase;
	m_NameAlpha = i_CopyFrom.m_NameAlpha;
	m_NameSpecularShift = i_CopyFrom.m_NameSpecularShift;
	m_NameSpecularMask = i_CopyFrom.m_NameSpecularMask;
	m_NameNormalMap = i_CopyFrom.m_NameNormalMap;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effHairData::HasTransparency()
{
	if (m_TextureAlpha != NULL)
		return true;
	if( m_HairBaseColor.GetAlpha() != 1.0f )
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
		if (textures[i]->HasTransparency())
			num_trans++;
	}
	return (num_trans == textures.size());
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effHairData::DidTextureChange(const effShaderData* i_NewData) const
{
	const effHairData* pData = dynamic_cast<const effHairData*>(i_NewData);
	if (pData == NULL)
		return true;

	if (m_NameBase != pData->m_NameBase)
		return true;
	if (m_NameAlpha != pData->m_NameAlpha)
		return true;
	if (m_NameSpecularShift != pData->m_NameSpecularShift)
		return true;
	if (m_NameSpecularMask != pData->m_NameSpecularMask)
		return true;
	if (m_NameNormalMap != pData->m_NameNormalMap)
		return true;

	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effHairData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureBase) 
		io_Textures.push_back(m_TextureBase);
    if (m_TextureAlpha) 
		io_Textures.push_back(m_TextureAlpha);
    if (m_TextureSpecularShift) 
		io_Textures.push_back(m_TextureSpecularShift);
    if (m_TextureSpecularMask) 
		io_Textures.push_back(m_TextureSpecularMask);
    if (m_TextureNormalMap) 
		io_Textures.push_back(m_TextureNormalMap);
	// glow texture ?? 
	// owned by effGlowData
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effHairData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameBase != "")
		o_Names.push_back(m_NameBase);
	if (m_NameAlpha != "")
		o_Names.push_back(m_NameAlpha);
	if (m_NameSpecularShift != "")
		o_Names.push_back(m_NameSpecularShift);
	if (m_NameSpecularMask != "")
		o_Names.push_back(m_NameSpecularMask);
	if (m_NameNormalMap != "")
		o_Names.push_back(m_NameNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effHairData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameBase != "")
		m_TextureBase = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameBase.c_str()));
	if (m_NameAlpha != "")
		m_TextureAlpha = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameAlpha.c_str()));
	if (m_NameSpecularShift != "")
		m_TextureSpecularShift = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecularShift.c_str()));
	if (m_NameSpecularMask != "")
		m_TextureSpecularMask = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecularMask.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effHairData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameBase != "")
		m_TextureBase = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameBase.c_str()));
	if (m_NameAlpha != "")
		m_TextureAlpha = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameAlpha.c_str()));
	if (m_NameSpecularShift != "")
		m_TextureSpecularShift = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecularShift.c_str()));
	if (m_NameSpecularMask != "")
		m_TextureSpecularMask = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecularMask.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effHairData::RemoveTextures()
{
	m_TextureBase = NULL;
	m_TextureAlpha = NULL;
	m_TextureSpecularShift = NULL;
	m_TextureSpecularMask = NULL;
	m_TextureNormalMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effHairData::GetChunkName() const 
{
	return effHairDataParser::GetChunkName();
}
