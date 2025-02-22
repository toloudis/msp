/*****************************************************************************
**	effBlinnData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effBlinnData.hpp"

#include "Graphics/eff/effBlinnDataParser.hpp"
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
effBlinnData::effBlinnData()
:	m_ColorAmbient(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorDiffuse(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorSpecular(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorEmissive(0.0f, 0.0f, 0.0f, 0.0f),
	m_SpecularPower(1),
	m_BumpMapScale(1),
	m_Reflectivity(1),
	m_Transparency(1),
	m_IOR(2),
	m_DiffuseRoughness(0),
	m_TextureDiffuse(NULL),
	m_TextureSpecular(NULL),
	m_TextureGloss(NULL),
	m_TextureEnvironment(NULL),
	m_TextureNormalMap(NULL),
	m_TextureReflectFactorMap(NULL),
	m_TextureIORMap(NULL),
	m_TextureTransparencyMap(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlinnData::~effBlinnData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlinnData::effBlinnData(const effBlinnData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effBlinnData& effBlinnData::operator = (const effBlinnData& i_CopyFrom)
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
	m_IOR = i_CopyFrom.m_IOR;
	m_Transparency = i_CopyFrom.m_Transparency;
	m_DiffuseRoughness = i_CopyFrom.m_DiffuseRoughness;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;
	m_TextureSpecular = i_CopyFrom.m_TextureSpecular; 
	m_TextureGloss = i_CopyFrom.m_TextureGloss;
	m_TextureEnvironment = i_CopyFrom.m_TextureEnvironment;
	m_TextureNormalMap = i_CopyFrom.m_TextureNormalMap;
	m_TextureReflectFactorMap = i_CopyFrom.m_TextureReflectFactorMap;
	m_TextureIORMap = i_CopyFrom.m_TextureIORMap;
	m_TextureTransparencyMap = i_CopyFrom.m_TextureTransparencyMap;
	m_NameDiffuse = i_CopyFrom.m_NameDiffuse ;
	m_NameSpecular = i_CopyFrom.m_NameSpecular ;
	m_NameGloss = i_CopyFrom.m_NameGloss ;
	m_NameEnvironment = i_CopyFrom.m_NameEnvironment ;
	m_NameNormalMap = i_CopyFrom.m_NameNormalMap ;
	m_NameReflectFactorMap = i_CopyFrom.m_NameReflectFactorMap ;
	m_NameIORMap = i_CopyFrom.m_NameIORMap ;
	m_NameTransparencyMap = i_CopyFrom.m_NameTransparencyMap ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effBlinnData::HasTransparency()
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
void effBlinnData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureDiffuse) 
		io_Textures.push_back(m_TextureDiffuse);
    if (m_TextureSpecular) 
		io_Textures.push_back(m_TextureSpecular);
    if (m_TextureGloss) 
		io_Textures.push_back(m_TextureGloss);
    if (m_TextureEnvironment) 
		io_Textures.push_back(m_TextureEnvironment);
    if (m_TextureNormalMap) 
		io_Textures.push_back(m_TextureNormalMap);
    if (m_TextureReflectFactorMap) 
		io_Textures.push_back(m_TextureReflectFactorMap);
    if (m_TextureIORMap) 
		io_Textures.push_back(m_TextureIORMap);
	if (m_TextureTransparencyMap) 
		io_Textures.push_back(m_TextureTransparencyMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBlinnData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffuse != "")
		o_Names.push_back(m_NameDiffuse);
	if (m_NameSpecular != "")
		o_Names.push_back(m_NameSpecular);
	if (m_NameGloss != "")
		o_Names.push_back(m_NameGloss);
	if (m_NameEnvironment != "")
		o_Names.push_back(m_NameEnvironment);
	if (m_NameNormalMap != "")
		o_Names.push_back(m_NameNormalMap);
	if (m_NameReflectFactorMap != "")
		o_Names.push_back(m_NameReflectFactorMap);
	if (m_NameIORMap != "")
		o_Names.push_back(m_NameIORMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBlinnData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffuse.c_str()));
	if (m_NameSpecular != "")
		m_TextureSpecular = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecular.c_str()));
	if (m_NameGloss != "")
		m_TextureGloss = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameGloss.c_str()));
	if (m_NameEnvironment != "")
		m_TextureEnvironment = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameEnvironment.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalMap.c_str()));
	if (m_NameReflectFactorMap != "")
		m_TextureReflectFactorMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameReflectFactorMap.c_str()));
	if (m_NameIORMap != "")
		m_TextureIORMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameIORMap.c_str()));
	if (m_NameTransparencyMap != "")
		m_TextureTransparencyMap = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransparencyMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBlinnData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffuse.c_str()));
	if (m_NameSpecular != "")
		m_TextureSpecular = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecular.c_str()));
	if (m_NameGloss != "")
		m_TextureGloss = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameGloss.c_str()));
	if (m_NameEnvironment != "")
		m_TextureEnvironment = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameEnvironment.c_str()));
	if (m_NameNormalMap != "")
		m_TextureNormalMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalMap.c_str()));
	if (m_NameReflectFactorMap != "")
		m_TextureReflectFactorMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameReflectFactorMap.c_str()));
	if (m_NameIORMap != "")
		m_TextureIORMap = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameIORMap.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBlinnData::RemoveTextures()
{
	m_TextureDiffuse = NULL;
    m_TextureSpecular = NULL;
    m_TextureGloss = NULL;
    m_TextureEnvironment = NULL;
    m_TextureNormalMap = NULL;
    m_TextureReflectFactorMap = NULL;
    m_TextureIORMap = NULL;
	m_TextureTransparencyMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effBlinnData::GetChunkName() const 
{
	return effBlinnDataParser::GetChunkName();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effBlinnData::AddToParams(effShaderParams& o_Params)
{
	o_Params.SetVersion(1);
	o_Params.AddParam(new effParamFloat("g_shininess", "g_shininess", m_SpecularPower));
	o_Params.AddParam(new effParamFloat("g_bumpMapScale", "g_bumpMapScale", m_BumpMapScale));
	o_Params.AddParam(new effParamFloat("g_reflectivity", "g_reflectivity", m_Reflectivity));
	float alpha = (m_Transparency < 1) ? m_Transparency : m_ColorDiffuse.GetAlpha();
	o_Params.AddParam(new effParamFloat("g_transparency", "g_transparency", alpha));
	o_Params.AddParam(new effParamColor("g_emissive", "g_emissive", m_ColorEmissive));
	o_Params.AddParam(new effParamColor("g_ambient", "g_ambient", m_ColorAmbient));
	o_Params.AddParam(new effParamColor("g_diffuse", "g_diffuse", m_ColorDiffuse));
	o_Params.AddParam(new effParamColor("g_specular", "g_specular", m_ColorSpecular));

	o_Params.AddParam(new effParamTexture("diffuseMap", "diffuseMap", itString(m_NameDiffuse.c_str())));
	o_Params.AddParam(new effParamTexture("normalMap", "normalMap", itString(m_NameNormalMap.c_str())));
	o_Params.AddParam(new effParamTexture("cubeMap", "cubeMap", itString(m_NameEnvironment.c_str())));
	o_Params.AddParam(new effParamTexture("specularMap", "specularMap", itString(m_NameSpecular.c_str())));
	o_Params.AddParam(new effParamTexture("glossMap", "glossMap", itString(m_NameGloss.c_str())));
	o_Params.AddParam(new effParamTexture("reflectFactorMap", "reflectFactorMap", itString(m_NameReflectFactorMap.c_str())));
	o_Params.AddParam(new effParamTexture("transparencyMap", "transparencyMap", itString(m_NameTransparencyMap.c_str())));
}
