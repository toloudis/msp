/*****************************************************************************
**	effDisplacementData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effDisplacementData.hpp"
#include "Graphics/eff/effDisplacementDataParser.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceFinder.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDisplacementData::effDisplacementData()
:	m_Scale(0.0f),
	m_Bias(0.0f),
	m_Blur(0.0f),
	m_TessellationValue(0.0f),
	m_ObjUVScale(1.0f,1.0f),
	m_pDisplacementMap(NULL)
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDisplacementData::~effDisplacementData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDisplacementData::effDisplacementData(const effDisplacementData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effDisplacementData& effDisplacementData::operator = (const effDisplacementData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_Scale = i_CopyFrom.m_Scale;
	m_Bias = i_CopyFrom.m_Bias;
	m_Blur = i_CopyFrom.m_Blur;
	m_TessellationValue = i_CopyFrom.m_TessellationValue;
	m_NameDisplacementMap = i_CopyFrom.m_NameDisplacementMap;
	m_pDisplacementMap = i_CopyFrom.m_pDisplacementMap;
	m_ObjUVScale = i_CopyFrom.m_ObjUVScale;
	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effDisplacementData::operator == (const effDisplacementData& i_EffDisplacementData) const
{
	return( m_Scale == i_EffDisplacementData.m_Scale) &&
		  ( m_Bias == i_EffDisplacementData.m_Bias) &&
		  ( m_Blur == i_EffDisplacementData.m_Blur) &&
		  ( m_ObjUVScale == i_EffDisplacementData.m_ObjUVScale ) &&
		  ( m_TessellationValue == i_EffDisplacementData.m_TessellationValue ) &&
		  ( m_pDisplacementMap == i_EffDisplacementData.m_pDisplacementMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effDisplacementData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_pDisplacementMap)
		io_Textures.push_back(m_pDisplacementMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effDisplacementData::GetTextureNames(std::vector<std::string>& o_Names) const
{
	std::string sDisplacementMap;
	fsFileUtil::LocatorToANSIFilename(m_NameDisplacementMap, sDisplacementMap);
	if (sDisplacementMap != "")
		o_Names.push_back(sDisplacementMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effDisplacementData::ReloadTextures(const fsLocator& i_TextureDirectory)
{
	if (m_NameDisplacementMap.GetNumNames() != 0)
		m_pDisplacementMap = matTextureMgr::LoadTexture(m_NameDisplacementMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effDisplacementData::ReloadTextures(const fsResourceFinder& i_TextureFinder)
{
	if (m_NameDisplacementMap.GetNumNames() != 0)
		m_pDisplacementMap = matTextureMgr::LoadTexture(m_NameDisplacementMap);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effDisplacementData::RemoveTextures()
{
	m_pDisplacementMap = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effDisplacementData::GetChunkName() const 
{
	return effDisplacementDataParser::GetChunkName();
}
