/*****************************************************************************
**  effReflData.hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effReflData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Core/it/itString.hpp"
#include "Graphics/eff/effReflDataParser.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/mat/matTexture.hpp"
#include "Graphics/mat/matTextureMgr.hpp"

#include <algorithm>
#include <functional>
#include <vector>


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflectionMap::effReflectionMap()
:	m_NearPlane(0.1f), m_bIsPlanar(false),
	m_bAutoGenEnvMap(false), m_ReflMapResolution(128),
	m_ReflectionMap(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflectionMap::effReflectionMap(const effReflectionMap& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflectionMap& effReflectionMap::operator = (const effReflectionMap& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	m_bAutoGenEnvMap = i_CopyFrom.m_bAutoGenEnvMap;
	m_ReflMapResolution = i_CopyFrom.m_ReflMapResolution;
	m_NearPlane = i_CopyFrom.m_NearPlane;
	m_bIsPlanar = i_CopyFrom.m_bIsPlanar;
	m_ReflectionMap = i_CopyFrom.m_ReflectionMap;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effReflectionMap::operator == (const effReflectionMap& i_EffReflData) const
{
	return (m_bAutoGenEnvMap == i_EffReflData.m_bAutoGenEnvMap) &&
		(m_ReflMapResolution == i_EffReflData.m_ReflMapResolution) &&
		(m_NearPlane == i_EffReflData.m_NearPlane) &&
		(m_bIsPlanar == i_EffReflData.m_bIsPlanar);
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflData::effReflData()
:	m_ColorAmbient(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorDiffuse(0.2f, 0.2f, 0.2f, 1.0f),
	m_ColorSpecular(0.2f, 0.2f, 0.2f, 1.0f),
	m_ColorEmissive(0.0f, 0.0f, 0.0f, 0.0f),
	m_SpecularPower(1),
	m_BumpMapScale(1),
	m_Reflectivity(1),
	m_Transparency(1),
	m_ReflectionMap(NULL),
	m_TextureNormalMap(NULL),
	m_TextureDiffuseMap(NULL),
	m_TextureSpecularMap(NULL),
	m_TextureReflectFactorMap(NULL),
	m_TextureTransparencyMap(NULL),
	m_bAutoGenEnvMap(false),
	m_ReflMapResolution(128),
	m_IOR(1),
	m_ReflLOD(0),
	m_RefrLOD(0),
	m_ReflFactor(1),
	m_RefrFactor(0),
	m_FresnelBias(0.2f),
	m_FresnelPower(4),
	m_bIsPlanar(false),
	m_NearPlane(0.1f)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflData::~effReflData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflData::effReflData(const effReflData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effReflData& effReflData::operator = (const effReflData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_UV = i_CopyFrom.m_UV;
	m_ColorAmbient = i_CopyFrom.m_ColorAmbient;
	m_ColorDiffuse = i_CopyFrom.m_ColorDiffuse;
	m_ColorSpecular = i_CopyFrom.m_ColorSpecular; 
	m_ColorEmissive = i_CopyFrom.m_ColorEmissive;
	m_SpecularPower = i_CopyFrom.m_SpecularPower;
	m_BumpMapScale = i_CopyFrom.m_BumpMapScale;
	m_Reflectivity = i_CopyFrom.m_Reflectivity;
	m_Transparency = i_CopyFrom.m_Transparency;
	m_TextureNormalMap = i_CopyFrom.m_TextureNormalMap;
	m_NameNormalMap = i_CopyFrom.m_NameNormalMap ;
	m_TextureDiffuseMap = i_CopyFrom.m_TextureDiffuseMap;
	m_NameDiffuseMap = i_CopyFrom.m_NameDiffuseMap ;
	m_TextureSpecularMap = i_CopyFrom.m_TextureSpecularMap;
	m_NameSpecularMap = i_CopyFrom.m_NameSpecularMap ;
	m_TextureReflectFactorMap = i_CopyFrom.m_TextureReflectFactorMap;
	m_NameReflectFactorMap = i_CopyFrom.m_NameReflectFactorMap ;
	m_ReflectionMap = i_CopyFrom.m_ReflectionMap;
	m_NameTransparencyMap = i_CopyFrom.m_NameTransparencyMap;
	m_TextureTransparencyMap = i_CopyFrom.m_TextureTransparencyMap;
	m_bAutoGenEnvMap = i_CopyFrom.m_bAutoGenEnvMap;
	m_ReflMapResolution = i_CopyFrom.m_ReflMapResolution;
	m_IOR = i_CopyFrom.m_IOR;
	m_ReflLOD = i_CopyFrom.m_ReflLOD;
	m_RefrLOD = i_CopyFrom.m_RefrLOD;
	m_ReflFactor = i_CopyFrom.m_ReflFactor;
	m_RefrFactor = i_CopyFrom.m_RefrFactor;
	m_FresnelBias = i_CopyFrom.m_FresnelBias;
	m_FresnelPower = i_CopyFrom.m_FresnelPower;
	m_NearPlane = i_CopyFrom.m_NearPlane;
	m_bIsPlanar = i_CopyFrom.m_bIsPlanar;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effReflData::HasTransparency()
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
void effReflData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
    if (m_TextureDiffuseMap) 
		io_Textures.push_back(m_TextureDiffuseMap);
    if (m_TextureNormalMap) 
		io_Textures.push_back(m_TextureNormalMap);
    if (m_TextureSpecularMap) 
		io_Textures.push_back(m_TextureSpecularMap);
    if (m_TextureReflectFactorMap) 
		io_Textures.push_back(m_TextureReflectFactorMap);
	if (m_TextureTransparencyMap) 
		io_Textures.push_back(m_TextureTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effReflData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffuseMap != "")
		o_Names.push_back(m_NameDiffuseMap);
	if (m_NameNormalMap != "")
		o_Names.push_back(m_NameNormalMap);
	if (m_NameSpecularMap != "")
		o_Names.push_back(m_NameSpecularMap);
	if (m_NameReflectFactorMap != "")
		o_Names.push_back(m_NameReflectFactorMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effReflData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffuseMap != "")
		m_TextureDiffuseMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffuseMap.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalMap.c_str()));
	if (m_NameSpecularMap != "")
		m_TextureSpecularMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecularMap.c_str()));
	if (m_NameReflectFactorMap != "")
		m_TextureReflectFactorMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameReflectFactorMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effReflData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffuseMap != "")
		m_TextureDiffuseMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffuseMap.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalMap.c_str()));
	if (m_NameSpecularMap != "")
		m_TextureSpecularMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecularMap.c_str()));
	if (m_NameReflectFactorMap != "")
		m_TextureReflectFactorMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameReflectFactorMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effReflData::RemoveTextures()
{
    m_TextureNormalMap = NULL;
    m_TextureDiffuseMap = NULL;
    m_TextureSpecularMap = NULL;
    m_TextureReflectFactorMap = NULL;
	m_TextureTransparencyMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effReflData::GetChunkName() const 
{
	return effReflDataParser::GetChunkName();
}
