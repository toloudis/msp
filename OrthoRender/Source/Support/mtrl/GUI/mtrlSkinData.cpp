/********************************************************************************************\
**  mtrlSkinData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlSkinData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSkinData::mtrlSkinData()
:	m_SpecColor("Specular Color", maFloatRGBA(0.45f, 0.65f, 1.0f, 1.0f)),
	m_SpecPower("Specular Power", 0.2f),
	m_SpecGloss("Specular Gloss", 15.0f),
	m_SpecFresnel("Specular Fresnel", 3.0f),
	m_FresnelPower("Frensel Power", 15.0f),
	m_FresnelGloss("Fresnel Gloss", 0.0f),
	m_TransColIn("Unscattered", maFloatRGBA(0.87f, 0.91f, 0.96f, 1.0f)),
	m_TransColOut("Melanin", maFloatRGBA(1.0f, 0.71f, 0.32f, 1.0f)),
	m_TransColBack("Hemoglobin", maFloatRGBA(0.58f, 0.2f, 0.24f, 1.0f)),
	m_TransMultiplier("Translucent Power", 1.0f),
	m_TransRampOff("Translucent Ramp Off", 1.5f),
	m_MicroScale("Micro Normal Scale", 50),
	m_DiffTex("Diffuse", itString("")),
	m_NormalTex("Normal Map", itString("")),
	m_MicroTex("Micro Normal Map", itString("")),
	m_SpecTex("Specular Color Map", itString("")),
	m_SpecPowerTex("Specular Power Map", itString("")),
	m_TransTex("Translucency/Depth", itString("")),
	m_CubeMapTex("Cubic Env Map", itString("")),
	m_ReflectFactorTex("Reflection mask", itString("")),
	m_TransparencyTex("Transparency Map", itString("")),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlSkinData::mtrlSkinData(const mtrlSkinData& i_Data)
:	m_SpecColor("Specular Color", maFloatRGBA(0.45f, 0.65f, 1.0f, 1.0f)),
	m_SpecPower("Specular Power", 0.2f),
	m_SpecGloss("Specular Gloss", 15.0f),
	m_SpecFresnel("Specular Fresnel", 3.0f),
	m_FresnelPower("Frensel Power", 15.0f),
	m_FresnelGloss("Fresnel Gloss", 0.0f),
	m_TransColIn("Unscattered", maFloatRGBA(0.87f, 0.91f, 0.96f, 1.0f)),
	m_TransColOut("Melanin", maFloatRGBA(1.0f, 0.71f, 0.32f, 1.0f)),
	m_TransColBack("Hemoglobin", maFloatRGBA(0.58f, 0.2f, 0.24f, 1.0f)),
	m_TransMultiplier("Translucent Power", 1.0f),
	m_TransRampOff("Translucent Ramp Off", 1.5f),
	m_MicroScale("Micro Normal Scale", 50),
	m_DiffTex("Diffuse", itString("")),
	m_NormalTex("Normal Map", itString("")),
	m_MicroTex("Micro Normal Map", itString("")),
	m_SpecTex("Specular Color Map", itString("")),
	m_SpecPowerTex("Specular Power Map", itString("")),
	m_TransTex("Translucency/Depth", itString("")),
	m_CubeMapTex("Cubic Env Map", itString("")),
	m_ReflectFactorTex("Reflection mask", itString("")),
	m_TransparencyTex("Transparency Map", itString("")),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_Transparency("Transparency", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSkinData::~mtrlSkinData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlSkinData& mtrlSkinData::operator=(const mtrlSkinData& i_Data)
{
	if (this == &i_Data) return *this;

	m_SpecColor = i_Data.m_SpecColor;
	m_SpecPower = i_Data.m_SpecPower;
	m_SpecGloss = i_Data.m_SpecGloss;
	m_SpecFresnel = i_Data.m_SpecFresnel;
	m_FresnelPower = i_Data.m_FresnelPower;
	m_FresnelGloss = i_Data.m_FresnelGloss;
	m_TransColIn = i_Data.m_TransColIn;
	m_TransColOut = i_Data.m_TransColOut;
	m_TransColBack = i_Data.m_TransColBack;
	m_TransMultiplier = i_Data.m_TransMultiplier;
	m_TransRampOff = i_Data.m_TransRampOff;
	m_MicroScale = i_Data.m_MicroScale;

	m_DiffTex = i_Data.m_DiffTex;
	m_NormalTex = i_Data.m_NormalTex;
	m_MicroTex = i_Data.m_MicroTex;
	m_SpecTex = i_Data.m_SpecTex;
	m_SpecPowerTex = i_Data.m_SpecPowerTex;
	m_TransTex = i_Data.m_TransTex;
	m_CubeMapTex = i_Data.m_CubeMapTex;
	m_ReflectFactorTex = i_Data.m_ReflectFactorTex;
	m_TransparencyTex = i_Data.m_TransparencyTex;

	m_BumpMapScale = i_Data.m_BumpMapScale;
	m_Transparency = i_Data.m_Transparency;
	m_UScale = i_Data.m_UScale;
	m_VScale = i_Data.m_VScale;
	m_UTrans = i_Data.m_UTrans;
	m_VTrans = i_Data.m_VTrans;
	m_UVAngle = i_Data.m_UVAngle;

	return *this;
}

