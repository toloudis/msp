/*****************************************************************************
**  effParticleData.cpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effParticleData.hpp"

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
effParticleData::effParticleData()
:	m_ColorAmbient(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorDiffuse(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorSpecular(1.0f, 1.0f, 1.0f, 1.0f),
	m_ColorEmissive(0.0f, 0.0f, 0.0f, 0.0f),
	m_SpecularPower(1),
	m_TextureDiffuse(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effParticleData::~effParticleData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effParticleData::effParticleData(const effParticleData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effParticleData& effParticleData::operator = (const effParticleData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_ColorAmbient = i_CopyFrom.m_ColorAmbient;
	m_ColorDiffuse = i_CopyFrom.m_ColorDiffuse;
	m_ColorSpecular = i_CopyFrom.m_ColorSpecular; 
	m_ColorEmissive = i_CopyFrom.m_ColorEmissive;
	m_SpecularPower = i_CopyFrom.m_SpecularPower;
	m_TextureDiffuse = i_CopyFrom.m_TextureDiffuse;
	m_NameDiffuse = i_CopyFrom.m_NameDiffuse ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effParticleData::HasTransparency()
{
	return true;
/*
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
		if (textures[i]->HasTransparency())
			num_trans++;
	}
	return (num_trans == textures.size());
*/
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effParticleData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_TextureDiffuse) 
		io_Textures.push_back(m_TextureDiffuse);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effParticleData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	if (m_NameDiffuse != "")
		o_Names.push_back(m_NameDiffuse);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effParticleData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureDirectory, itString(m_NameDiffuse.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effParticleData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDiffuse != "")
		m_TextureDiffuse = matTextureMgr::LoadTexture(i_TextureFinder, itString(m_NameDiffuse.c_str()));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effParticleData::RemoveTextures()
{
	m_TextureDiffuse = NULL;
}

