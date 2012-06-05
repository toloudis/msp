/********************************************************************************************\
**  mtrlPhongData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlPhongData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPhongData::mtrlPhongData()
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_SpecularPower("Shininess", 1.0f),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Reflectivity("Reflectivity", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_DisplacementScale("Displacement Scale", 0.0f),
	m_DisplacementBias("Displacement Bias", 0.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureSpecular("Specular Map", itString("")),
	m_TextureGloss("Gloss Map", itString("")),
	m_TextureEnvironment("Cube Env Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureReflectFactorMap("Reflection Mask", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString("")),
	m_TextureDisplacementMap("Displacement Map", itString(""))
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlPhongData::mtrlPhongData(const mtrlPhongData& i_Data)
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_SpecularPower("Shininess", 1.0f),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Reflectivity("Reflectivity", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_DisplacementScale("Displacement Scale", 0.0f),
	m_DisplacementBias("Displacement Bias", 0.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureSpecular("Specular Map", itString("")),
	m_TextureGloss("Gloss Map", itString("")),
	m_TextureEnvironment("Cube Env Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureReflectFactorMap("Reflection Mask", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString("")),
	m_TextureDisplacementMap("Displacement Map", itString(""))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPhongData::~mtrlPhongData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlPhongData& mtrlPhongData::operator=(const mtrlPhongData& i_Data)
{
	if (this == &i_Data) return *this;

	m_ColorAmbient			= i_Data.m_ColorAmbient;
	m_ColorDiffuse			= i_Data.m_ColorDiffuse;
	m_ColorSpecular			= i_Data.m_ColorSpecular;
	m_ColorEmissive			= i_Data.m_ColorEmissive;
	m_SpecularPower			= i_Data.m_SpecularPower;
	m_BumpMapScale			= i_Data.m_BumpMapScale;
	m_Reflectivity			= i_Data.m_Reflectivity;
	m_Transparency			= i_Data.m_Transparency;
	m_DisplacementScale		= i_Data.m_DisplacementScale;
	m_DisplacementBias		= i_Data.m_DisplacementBias;
	m_UScale				= i_Data.m_UScale;
	m_VScale				= i_Data.m_VScale;
	m_UTrans				= i_Data.m_UTrans;
	m_VTrans				= i_Data.m_VTrans;
	m_UVAngle				= i_Data.m_UVAngle;
	m_TextureDiffuse		= i_Data.m_TextureDiffuse;
	m_TextureSpecular		= i_Data.m_TextureSpecular;
	m_TextureGloss			= i_Data.m_TextureGloss;
	m_TextureEnvironment	= i_Data.m_TextureEnvironment;
	m_TextureNormalMap		= i_Data.m_TextureNormalMap;
	m_TextureReflectFactorMap = i_Data.m_TextureReflectFactorMap;
	m_TextureTransparencyMap = i_Data.m_TextureTransparencyMap;
	m_TextureDisplacementMap = i_Data.m_TextureDisplacementMap;

	return *this;
}

