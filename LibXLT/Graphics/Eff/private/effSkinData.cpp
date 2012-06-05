/*****************************************************************************
**	effSkinData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effSkinData.hpp"

#include "Graphics/eff/effSkinDataParser.hpp"

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
effSkinData::effSkinData()
:	m_SpecColor(0.45f, 0.65f, 1.0f, 1.0f),
	m_SpecPower(0.2f),
	m_SpecGloss(15.0f),
	m_SpecFresnel(3.0f),
	m_FresnelPower(15.0f),
	m_FresnelGloss(0.0f),
	m_TransColIn(0.87f, 0.91f, 0.96f, 1.0f),
	m_TransColOut(1.0f, 0.71f, 0.32f, 1.0f),
	m_TransColBack(0.58f, 0.2f, 0.24f, 1.0f),
	m_TransMultiplier(1.0f),
	m_TransRampOff(1.5f),
	m_MicroScale(50),
	m_DiffTex(NULL),
	m_NormalTex(NULL),
	m_MicroTex(NULL),
	m_SpecTex(NULL),
	m_SpecPowerTex(NULL),
	m_TransTex(NULL),
	m_CubeMapTex(NULL),
	m_ReflectFactorTex(NULL),
	m_TransparencyTex(NULL),
	m_BumpMapScale(1),
	m_Transparency(1)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effSkinData::~effSkinData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effSkinData::effSkinData(const effSkinData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effSkinData& effSkinData::operator = (const effSkinData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator=(i_CopyFrom);
	m_UV = i_CopyFrom.m_UV;
	m_BumpMapScale = i_CopyFrom.m_BumpMapScale;
	m_Transparency = i_CopyFrom.m_Transparency;
	m_SpecColor = i_CopyFrom.m_SpecColor ;
	m_SpecPower = i_CopyFrom.m_SpecPower ;
	m_SpecGloss = i_CopyFrom.m_SpecGloss ;
	m_SpecFresnel = i_CopyFrom.m_SpecFresnel ;
	m_FresnelPower = i_CopyFrom.m_FresnelPower ;
	m_FresnelGloss = i_CopyFrom.m_FresnelGloss ;
	m_TransColIn = i_CopyFrom.m_TransColIn ;
	m_TransColOut = i_CopyFrom.m_TransColOut ;
	m_TransColBack = i_CopyFrom.m_TransColBack ;
	m_TransMultiplier = i_CopyFrom.m_TransMultiplier ;
	m_TransRampOff = i_CopyFrom.m_TransRampOff ;
	m_MicroScale = i_CopyFrom.m_MicroScale ;
	m_NameDiffTex = i_CopyFrom.m_NameDiffTex ;
	m_NameNormalTex = i_CopyFrom.m_NameNormalTex ;
	m_NameMicroTex = i_CopyFrom.m_NameMicroTex ;
	m_NameSpecTex = i_CopyFrom.m_NameSpecTex ;
	m_NameSpecPowerTex = i_CopyFrom.m_NameSpecPowerTex ;
	m_NameTransTex = i_CopyFrom.m_NameTransTex ;
	m_NameCubeMapTex = i_CopyFrom.m_NameCubeMapTex ;
	m_NameReflectFactorTex = i_CopyFrom.m_NameReflectFactorTex ;
	m_DiffTex = i_CopyFrom.m_DiffTex ;
	m_NormalTex = i_CopyFrom.m_NormalTex ;
	m_MicroTex = i_CopyFrom.m_MicroTex ;
	m_SpecTex = i_CopyFrom.m_SpecTex ;
	m_SpecPowerTex = i_CopyFrom.m_SpecPowerTex ;
	m_TransTex = i_CopyFrom.m_TransTex ;
	m_CubeMapTex = i_CopyFrom.m_CubeMapTex ;
	m_ReflectFactorTex = i_CopyFrom.m_ReflectFactorTex ;
	m_TransparencyTex = i_CopyFrom.m_TransparencyTex ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effSkinData::HasTransparency()
{
	if( m_Transparency != 1.0f )
	{
		return true;
	}

	if( m_TransparencyTex )		//ignore whether the texture has alpha, since only r channel is used
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
bool effSkinData::DidTextureChange(const effShaderData* i_NewData) const
{
	const effSkinData* pData = dynamic_cast<const effSkinData*>(i_NewData);
	if (pData == NULL)
		return true;

	if (m_NameDiffTex != pData->m_NameDiffTex)
		return true;
	if (m_NameNormalTex != pData->m_NameNormalTex)
		return true;
	if (m_NameMicroTex != pData->m_NameMicroTex)
		return true;
	if (m_NameSpecTex != pData->m_NameSpecTex)
		return true;
	if (m_NameSpecPowerTex != pData->m_NameSpecPowerTex)
		return true;
	if (m_NameTransTex != pData->m_NameTransTex)
		return true;
	if (m_NameCubeMapTex != pData->m_NameCubeMapTex)
		return true;
	if (m_NameReflectFactorTex != pData->m_NameReflectFactorTex)
		return true;

	return false;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effSkinData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_DiffTex) 
		io_Textures.push_back(m_DiffTex);
    if (m_NormalTex) 
		io_Textures.push_back(m_NormalTex);
    if (m_MicroTex) 
		io_Textures.push_back(m_MicroTex);
    if (m_SpecTex) 
		io_Textures.push_back(m_SpecTex);
    if (m_SpecPowerTex) 
		io_Textures.push_back(m_SpecPowerTex);
    if (m_TransTex) 
		io_Textures.push_back(m_TransTex);
    if (m_CubeMapTex) 
		io_Textures.push_back(m_CubeMapTex);
    if (m_ReflectFactorTex) 
		io_Textures.push_back(m_ReflectFactorTex);
	if (m_TransparencyTex) 
		io_Textures.push_back(m_TransparencyTex);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effSkinData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffTex != "")
		o_Names.push_back(m_NameDiffTex);
	if (m_NameNormalTex != "")
		o_Names.push_back(m_NameNormalTex);
	if (m_NameMicroTex != "")
		o_Names.push_back(m_NameMicroTex);
	if (m_NameSpecTex != "")
		o_Names.push_back(m_NameSpecTex);
	if (m_NameSpecPowerTex != "")
		o_Names.push_back(m_NameSpecPowerTex);
	if (m_NameTransTex != "")
		o_Names.push_back(m_NameTransTex);
	if (m_NameCubeMapTex != "")
		o_Names.push_back(m_NameCubeMapTex);
	if (m_NameReflectFactorTex != "")
		o_Names.push_back(m_NameReflectFactorTex);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effSkinData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffTex != "")
		m_DiffTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffTex.c_str()));
	if (m_NameNormalTex != "")
		m_NormalTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameNormalTex.c_str()));
	if (m_NameMicroTex != "")
		m_MicroTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameMicroTex.c_str()));
	if (m_NameSpecTex != "")
		m_SpecTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecTex.c_str()));
	if (m_NameSpecPowerTex != "")
		m_SpecPowerTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameSpecPowerTex.c_str()));
	if (m_NameTransTex != "")
		m_TransTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransTex.c_str()));
	if (m_NameCubeMapTex != "")
		m_CubeMapTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameCubeMapTex.c_str()));
	if (m_NameReflectFactorTex != "")
		m_ReflectFactorTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameReflectFactorTex.c_str()));
	if (m_NameTransparencyTex != "")
		m_TransparencyTex = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameTransparencyTex.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effSkinData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffTex != "")
		m_DiffTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffTex.c_str()));
	if (m_NameNormalTex != "")
		m_NormalTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameNormalTex.c_str()));
	if (m_NameMicroTex != "")
		m_MicroTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameMicroTex.c_str()));
	if (m_NameSpecTex != "")
		m_SpecTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecTex.c_str()));
	if (m_NameSpecPowerTex != "")
		m_SpecPowerTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameSpecPowerTex.c_str()));
	if (m_NameTransTex != "")
		m_TransTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameTransTex.c_str()));
	if (m_NameCubeMapTex != "")
		m_CubeMapTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameCubeMapTex.c_str()));
	if (m_NameReflectFactorTex != "")
		m_ReflectFactorTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameReflectFactorTex.c_str()));
	if (m_NameTransparencyTex != "")
		m_TransparencyTex = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameTransparencyTex.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effSkinData::RemoveTextures()
{
	m_DiffTex = NULL;
	m_NormalTex = NULL;
	m_MicroTex = NULL;
	m_SpecTex = NULL;
	m_SpecPowerTex = NULL;
	m_TransTex = NULL;
	m_CubeMapTex = NULL;
	m_ReflectFactorTex = NULL;
	m_TransparencyTex = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effSkinData::GetChunkName() const 
{
	return effSkinDataParser::GetChunkName();
}
