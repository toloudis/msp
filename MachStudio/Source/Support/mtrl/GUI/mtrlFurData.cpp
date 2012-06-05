/********************************************************************************************\
**  mtrlFurData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlFurData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlFurData::mtrlFurData()
:	m_TextureFolder("Texture Folder", itString("C:\\projects\\MachStudio\\Shaders\\Special\\Fur\\")),
	m_NumShells("Num Shells", 32),
	m_LengthScale("Length Scale", 1),
	m_SpreadScale("Spread Scale", maVector3d(1,1,1)),
	m_ShellFader("Shell Fade", 1),
	m_bShowFins("Show Fins", false),
	m_FinFader("Fin Fade", 1),
	m_bColorSourcing("Color Sourcing", false),
	m_bFurThinning("Fur Thinning", false),
	m_bAnisotropic("Anisotropic Light", false)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlFurData::mtrlFurData(const mtrlFurData& i_Data)
:	m_TextureFolder("Texture Folder", itString("C:\\projects\\MachStudio\\Shaders\\Special\\Fur\\")),
	m_NumShells("Num Shells", 32),
	m_LengthScale("Length Scale", 1),
	m_SpreadScale("Spread Scale", maVector3d(1,1,1)),
	m_ShellFader("Shell Fade", 1),
	m_bShowFins("Show Fins", false),
	m_FinFader("Fin Fade", 1),
	m_bColorSourcing("Color Sourcing", false),
	m_bFurThinning("Fur Thinning", false),
	m_bAnisotropic("Anisotropic Light", false)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlFurData::~mtrlFurData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlFurData& mtrlFurData::operator=(const mtrlFurData& i_Data)
{
	if (this == &i_Data) return *this;

	m_TextureFolder = i_Data.m_TextureFolder;
	m_NumShells = i_Data.m_NumShells;
	m_LengthScale = i_Data.m_LengthScale;
	m_SpreadScale = i_Data.m_SpreadScale;
	m_ShellFader = i_Data.m_ShellFader;
	m_bShowFins = i_Data.m_bShowFins;
	m_FinFader = i_Data.m_FinFader;
	m_bColorSourcing = i_Data.m_bColorSourcing;
	m_bFurThinning = i_Data.m_bFurThinning;
	m_bAnisotropic = i_Data.m_bAnisotropic;

	return *this;
}

