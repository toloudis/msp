/********************************************************************************************\
**  mtrlLambertData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlLambertData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlLambertData::mtrlLambertData()
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString(""))
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlLambertData::mtrlLambertData(const mtrlLambertData& i_Data)
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureTransparencyMap("Transprency Map", itString(""))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlLambertData::~mtrlLambertData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlLambertData& mtrlLambertData::operator=(const mtrlLambertData& i_Data)
{
	if (this == &i_Data) return *this;

	m_ColorAmbient			= i_Data.m_ColorAmbient;
	m_ColorDiffuse			= i_Data.m_ColorDiffuse;
	m_ColorEmissive			= i_Data.m_ColorEmissive;
	m_BumpMapScale			= i_Data.m_BumpMapScale;
	m_Transparency			= i_Data.m_Transparency;
	m_UScale				= i_Data.m_UScale;
	m_VScale				= i_Data.m_VScale;
	m_UTrans				= i_Data.m_UTrans;
	m_VTrans				= i_Data.m_VTrans;
	m_UVAngle				= i_Data.m_UVAngle;
	m_TextureDiffuse		= i_Data.m_TextureDiffuse;
	m_TextureNormalMap		= i_Data.m_TextureNormalMap;
	m_TextureTransparencyMap = i_Data.m_TextureTransparencyMap;

	return *this;
}

