/*****************************************************************************
**  effRendermanOverrideData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effRendermanOverrideData.hpp"

#include "Graphics/eff/effRendermanOverrideDataParser.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"
#include "Graphics/mat/matTextureMgr.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRendermanOverrideData::effRendermanOverrideData()
:	m_ShaderLocation(fsLocator()),
	m_ParamList(""),
	m_AttributeList(""),
	m_bOverrideShader(false),
	m_bOverrideAttributes(false)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRendermanOverrideData::~effRendermanOverrideData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRendermanOverrideData::effRendermanOverrideData(const effRendermanOverrideData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effRendermanOverrideData& effRendermanOverrideData::operator = (const effRendermanOverrideData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_ShaderLocation = i_CopyFrom.m_ShaderLocation;
	m_ParamList = i_CopyFrom.m_ParamList;
	m_AttributeList = i_CopyFrom.m_AttributeList;
	m_bOverrideShader = i_CopyFrom.m_bOverrideShader;
	m_bOverrideAttributes = i_CopyFrom.m_bOverrideAttributes;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effRendermanOverrideData::operator == (const effRendermanOverrideData& i_EffRendermanOverrideData) const
{
	return (m_ShaderLocation == i_EffRendermanOverrideData.m_ShaderLocation) &&
		   (m_ParamList == i_EffRendermanOverrideData.m_ParamList) &&
		   (m_AttributeList == i_EffRendermanOverrideData.m_AttributeList) &&
		   (m_bOverrideShader == i_EffRendermanOverrideData.m_bOverrideShader) &&
		   (m_bOverrideAttributes == i_EffRendermanOverrideData.m_bOverrideAttributes);

}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effRendermanOverrideData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effRendermanOverrideData::GetTextureNames(std::vector<std::string>& o_Names) const
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effRendermanOverrideData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effRendermanOverrideData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effRendermanOverrideData::RemoveTextures()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effRendermanOverrideData::GetChunkName() const
{
	return effRendermanOverrideDataParser::GetChunkName();
}
