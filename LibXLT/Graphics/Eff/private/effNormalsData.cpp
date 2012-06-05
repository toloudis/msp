/*****************************************************************************
**  effNormalsData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effNormalsData.hpp"

#include "Graphics/eff/effNormalsDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effNormalsData::effNormalsData()
:	m_pNormalMap(NULL),
	m_BumpScale(0)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effNormalsData::~effNormalsData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effNormalsData::effNormalsData(const effNormalsData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effNormalsData& effNormalsData::operator = (const effNormalsData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_NameNormalMap = i_CopyFrom.m_NameNormalMap ;
	m_pNormalMap = i_CopyFrom.m_pNormalMap ;
	m_BumpScale = i_CopyFrom.m_BumpScale ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effNormalsData::operator == (const effNormalsData& i_EffNormalsData) const
{
	return (m_NameNormalMap == i_EffNormalsData.m_NameNormalMap) &&
		(m_pNormalMap == i_EffNormalsData.m_pNormalMap) &&
		(m_BumpScale == i_EffNormalsData.m_BumpScale);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effNormalsData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_pNormalMap)
		io_Textures.push_back(m_pNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effNormalsData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	std::string sNormalsMask;
	fsFileUtil::LocatorToANSIFilename(m_NameNormalMap, sNormalsMask);
	if (sNormalsMask != "")
		o_Names.push_back(sNormalsMask);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effNormalsData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameNormalMap.GetNumNames() != 0)
		m_pNormalMap = matTextureMgr::LoadTexture(m_NameNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effNormalsData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameNormalMap.GetNumNames() != 0)
		m_pNormalMap = matTextureMgr::LoadTexture(m_NameNormalMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effNormalsData::RemoveTextures()
{
	m_pNormalMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effNormalsData::GetChunkName() const
{
	return effNormalsDataParser::GetChunkName();
}
