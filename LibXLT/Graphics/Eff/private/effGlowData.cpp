/*****************************************************************************
**	effGlowData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effGlowData.hpp"

#include "Graphics/eff/effGlowDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effGlowData::effGlowData()
:	m_pTexture(NULL),
	m_pGlowMask(NULL),
	m_GlowAmount(0),
	m_GlowScale(0,0,0,0),
	m_GlowSize(0),
	m_bConstantGlow(false),
	m_SrcSizeInfo(0,0,0,0)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effGlowData::~effGlowData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effGlowData::effGlowData(const effGlowData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effGlowData& effGlowData::operator = (const effGlowData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_NameGlowMask = i_CopyFrom.m_NameGlowMask ;
	m_pGlowMask = i_CopyFrom.m_pGlowMask ;
	m_pTexture = i_CopyFrom.m_pTexture ;
	m_GlowAmount = i_CopyFrom.m_GlowAmount ;
	m_GlowScale = i_CopyFrom.m_GlowScale ;
	m_GlowSize = i_CopyFrom.m_GlowSize ;
	m_bConstantGlow = i_CopyFrom.m_bConstantGlow ;
	m_SrcSizeInfo = i_CopyFrom.m_SrcSizeInfo ;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effGlowData::operator == (const effGlowData& i_EffGlowData) const
{
	return (m_pTexture == i_EffGlowData.m_pTexture) &&
		(m_NameGlowMask == i_EffGlowData.m_NameGlowMask) &&
		(m_pGlowMask == i_EffGlowData.m_pGlowMask) &&
		(m_GlowAmount == i_EffGlowData.m_GlowAmount) &&
		(m_GlowScale == i_EffGlowData.m_GlowScale) &&
		(m_GlowSize == i_EffGlowData.m_GlowSize) &&
		(m_bConstantGlow == i_EffGlowData.m_bConstantGlow) &&
		(m_SrcSizeInfo == i_EffGlowData.m_SrcSizeInfo);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effGlowData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_pGlowMask)
		io_Textures.push_back(m_pGlowMask);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effGlowData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	std::string sGlowMask;
	fsFileUtil::LocatorToANSIFilename(m_NameGlowMask, sGlowMask);
	if (sGlowMask != "")
		o_Names.push_back(sGlowMask);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effGlowData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameGlowMask.GetNumNames() != 0)
		m_pGlowMask = matTextureMgr::LoadTexture(m_NameGlowMask);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effGlowData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameGlowMask.GetNumNames() != 0)
		m_pGlowMask = matTextureMgr::LoadTexture(m_NameGlowMask);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effGlowData::RemoveTextures()
{
	m_pTexture = NULL;
	m_pGlowMask = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effGlowData::GetChunkName() const
{
	return effGlowDataParser::GetChunkName();
}
