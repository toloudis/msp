/********************************************************************************************\
**  mtrlBlinnData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlBlinnData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlBlinnData::mtrlBlinnData()
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_SpecularPower("Shininess", 1.0f),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Reflectivity("Reflectivity", 1.0f),
	m_IOR("IndexOfRefraction", 2.0f),
	m_DiffuseRoughness("Diffuse Roughness", 0.0f),
	m_Transparency("Transparency", 1.0f ),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureSpecular("Specular Map", itString("")),
	m_TextureGloss("Shininess Map", itString("")),
	m_TextureEnvironment("Cube Env Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureReflectFactorMap("Reflection Mask", itString("")),
	m_TextureIORMap("Index Of Refraction Map", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString(""))
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlBlinnData::mtrlBlinnData(const mtrlBlinnData& i_Data)
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorEmissive("Emissive", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_SpecularPower("Shininess", 1.0f),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Reflectivity("Reflectivity", 1.0f),
	m_IOR("IndexOfRefraction", 2.0f),
	m_DiffuseRoughness("Diffuse Roughness", 0.0f),
	m_Transparency("Transparency", 1.0f ),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureSpecular("Specular Map", itString("")),
	m_TextureGloss("Shininess Map", itString("")),
	m_TextureEnvironment("Cube Env Map", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_TextureReflectFactorMap("Reflection Mask", itString("")),
	m_TextureIORMap("Index Of Refraction Map", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString(""))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlBlinnData::~mtrlBlinnData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlBlinnData& mtrlBlinnData::operator=(const mtrlBlinnData& i_Data)
{
	if (this == &i_Data) return *this;

	m_ColorAmbient			= i_Data.m_ColorAmbient;
	m_ColorDiffuse			= i_Data.m_ColorDiffuse;
	m_ColorSpecular			= i_Data.m_ColorSpecular;
	m_ColorEmissive			= i_Data.m_ColorEmissive;
	m_SpecularPower			= i_Data.m_SpecularPower;
	m_BumpMapScale			= i_Data.m_BumpMapScale;
	m_Reflectivity			= i_Data.m_Reflectivity;
	m_IOR					= i_Data.m_IOR;
	m_DiffuseRoughness		= i_Data.m_DiffuseRoughness;
	m_Transparency			= i_Data.m_Transparency;
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
	m_TextureIORMap			= i_Data.m_TextureIORMap;
	m_TextureTransparencyMap = i_Data.m_TextureTransparencyMap;

	return *this;
}

