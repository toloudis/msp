/*****************************************************************************
**	effFurData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007-9 - All Rights Reserved
\****************************************************************************/
#include "Graphics/eff/effFurData.hpp"

#include "Graphics/eff/effFurDataParser.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effFurData::effFurData()
:	m_NumShells(32),
	m_LengthScale(1),
	m_SpreadScale(maVector3d(2,2,8)),
	m_ShellFader(1),
	m_bShowFins(true),
	m_FinFader(1),
	m_bColorSourcing(true),
	m_bFurThinning(false),
	m_bAnisotropic(false),
	m_CurrentLengthPercentage(0),
	m_FurColorTexture(NULL),
	m_FurAnisoOpacityTexture(NULL),
	m_FurOffsetThreshTexture(NULL),
	m_FurAnisoStrandTexture(NULL),
	m_FurDensityTexture(NULL),
	m_FurAnisoOpacityTexture_fin(NULL),
	m_FurOffsetThreshTexture_fin(NULL),
	m_TextureFolder("\\projects\\MachStudio\\Shaders\\Special\\Fur")
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effFurData::~effFurData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effFurData::effFurData(const effFurData& i_CopyFrom)
{
	(*this) = i_CopyFrom;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
effFurData& effFurData::operator = (const effFurData& i_CopyFrom)
{
	if (&i_CopyFrom == this)
		return *this;

	effShaderData::operator =(i_CopyFrom);
	m_TextureFolder = i_CopyFrom.m_TextureFolder;
	m_NumShells = i_CopyFrom.m_NumShells;
	m_LengthScale = i_CopyFrom.m_LengthScale;
	m_SpreadScale = i_CopyFrom.m_SpreadScale;
	m_ShellFader = i_CopyFrom.m_ShellFader;
	m_bShowFins = i_CopyFrom.m_bShowFins;
	m_FinFader = i_CopyFrom.m_FinFader;
	m_bColorSourcing = i_CopyFrom.m_bColorSourcing;
	m_bFurThinning = i_CopyFrom.m_bFurThinning;
	m_bAnisotropic = i_CopyFrom.m_bAnisotropic;
	m_CurrentLengthPercentage = i_CopyFrom.m_CurrentLengthPercentage;
	m_FurColorTexture = i_CopyFrom.m_FurColorTexture;
	m_FurAnisoOpacityTexture = i_CopyFrom.m_FurAnisoOpacityTexture;
	m_FurOffsetThreshTexture = i_CopyFrom.m_FurOffsetThreshTexture;
	m_FurAnisoStrandTexture = i_CopyFrom.m_FurAnisoStrandTexture;
	m_FurDensityTexture = i_CopyFrom.m_FurDensityTexture;
	m_FurAnisoOpacityTexture_fin = i_CopyFrom.m_FurAnisoOpacityTexture_fin;
	m_FurOffsetThreshTexture_fin = i_CopyFrom.m_FurOffsetThreshTexture_fin;

	return *this;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool effFurData::operator == (const effFurData& i_EffFurData) const
{
	return (m_TextureFolder == i_EffFurData.m_TextureFolder) &&
		(m_NumShells == i_EffFurData.m_NumShells) &&
        (m_LengthScale == i_EffFurData.m_LengthScale) &&
		(m_SpreadScale == i_EffFurData.m_SpreadScale) &&
		(m_ShellFader == i_EffFurData.m_ShellFader) &&
		(m_bShowFins == i_EffFurData.m_bShowFins) &&
		(m_FinFader == i_EffFurData.m_FinFader) &&
		(m_bColorSourcing == i_EffFurData.m_bColorSourcing) &&
		(m_bFurThinning == i_EffFurData.m_bFurThinning) &&
		(m_bAnisotropic == i_EffFurData.m_bAnisotropic) &&
		(m_CurrentLengthPercentage == i_EffFurData.m_CurrentLengthPercentage) &&
		(m_FurColorTexture == i_EffFurData.m_FurColorTexture) &&
		(m_FurAnisoOpacityTexture == i_EffFurData.m_FurAnisoOpacityTexture) &&
		(m_FurOffsetThreshTexture == i_EffFurData.m_FurOffsetThreshTexture) &&
		(m_FurAnisoStrandTexture == i_EffFurData.m_FurAnisoStrandTexture) &&
		(m_FurDensityTexture == i_EffFurData.m_FurDensityTexture) &&
		(m_FurAnisoOpacityTexture_fin == i_EffFurData.m_FurAnisoOpacityTexture_fin) &&
		(m_FurOffsetThreshTexture_fin == i_EffFurData.m_FurOffsetThreshTexture_fin);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void effFurData::GetTextures(std::vector<matTexture*>& io_Textures) const
{
	if (m_FurColorTexture)
		io_Textures.push_back(m_FurColorTexture);
	if (m_FurAnisoOpacityTexture)
		io_Textures.push_back(m_FurAnisoOpacityTexture);
	if (m_FurOffsetThreshTexture)
		io_Textures.push_back(m_FurOffsetThreshTexture);
	if (m_FurAnisoStrandTexture)
		io_Textures.push_back(m_FurAnisoStrandTexture);
	if (m_FurDensityTexture)
		io_Textures.push_back(m_FurDensityTexture);
	if (m_FurAnisoOpacityTexture_fin)
		io_Textures.push_back(m_FurAnisoOpacityTexture_fin);
	if (m_FurOffsetThreshTexture_fin)
		io_Textures.push_back(m_FurOffsetThreshTexture_fin);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
chDefs::Name effFurData::GetChunkName() const
{
	return effFurDataParser::GetChunkName();
}
