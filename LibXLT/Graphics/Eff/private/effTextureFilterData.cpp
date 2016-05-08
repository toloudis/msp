/*****************************************************************************
**  effRendermanOverrideData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effTextureFilterData.hpp"

//#include "Graphics/eff/effTextureFilterDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTextureFilterData::effTextureFilterData()
:	m_bEnableMipmap(true)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTextureFilterData::~effTextureFilterData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTextureFilterData::effTextureFilterData(const effTextureFilterData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effTextureFilterData& effTextureFilterData::operator = (const effTextureFilterData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_bEnableMipmap = i_CopyFrom.m_bEnableMipmap;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effTextureFilterData::operator == (const effTextureFilterData& i_EffTextureFileData) const
{
	return (m_bEnableMipmap == i_EffTextureFileData.m_bEnableMipmap);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effTextureFilterData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effTextureFilterData::GetTextureNames(std::vector<std::string>& o_Names) const
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effTextureFilterData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effTextureFilterData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effTextureFilterData::RemoveTextures()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effTextureFilterData::GetChunkName() const
{
	//return effTextureFilterDataParser::GetChunkName();
	return chDefs::Name('\0');
}
