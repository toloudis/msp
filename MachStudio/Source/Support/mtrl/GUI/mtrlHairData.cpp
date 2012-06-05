/********************************************************************************************\
**  mtrlHairData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlHairData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlHairData::mtrlHairData()
:	m_HairBaseColor("Color", maFloatRGBA(0.58f, 0.284f, 0.0f, 1.0f)),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_Reflectivity("Reflectivity", 1),
	m_TextureBase("Hair Map", itString("")),
	m_TextureAlpha("Alpha Map", itString("")),
	m_TextureSpecularShift("Specular Shift", itString("")),
	m_TextureSpecularMask("Specular Noise Mask", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_SpecularColor0("Spec Hlt Col 1", maFloatRGBA(0.9f, 0.9f, 0.5f, 1.0f)),
	m_SpecularColor1("Spec Hlt Col 2", maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f)),
	m_SpecularExponent0("Spec Exp 1", 200),
	m_SpecularExponent1("Spec Exp 2", 100),
	m_SpecularShift0("Spec Shift 1", 0.0f),
	m_SpecularShift1("Spec Shift 2", 0.2f)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlHairData::mtrlHairData(const mtrlHairData& i_Data)
:	m_HairBaseColor("Color", maFloatRGBA(0.58f, 0.284f, 0.0f, 1.0f)),
	m_BumpMapScale("BumpMapScale", 1.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_Reflectivity("Reflectivity", 1),
	m_TextureBase("Hair Map", itString("")),
	m_TextureAlpha("Alpha Map", itString("")),
	m_TextureSpecularShift("Specular Shift", itString("")),
	m_TextureSpecularMask("Specular Noise Mask", itString("")),
	m_TextureNormalMap("Normal Map", itString("")),
	m_SpecularColor0("Spec Hlt Col 1", maFloatRGBA(0.9f, 0.9f, 0.5f, 1.0f)),
	m_SpecularColor1("Spec Hlt Col 2", maFloatRGBA(1.0f, 0.0f, 0.0f, 1.0f)),
	m_SpecularExponent0("Spec Exp 1", 200),
	m_SpecularExponent1("Spec Exp 2", 100),
	m_SpecularShift0("Spec Shift 1", 0.0f),
	m_SpecularShift1("Spec Shift 2", 0.2f)
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlHairData::~mtrlHairData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlHairData& mtrlHairData::operator=(const mtrlHairData& i_Data)
{
	if (this == &i_Data) return *this;

	m_HairBaseColor			= i_Data.m_HairBaseColor;
	m_BumpMapScale			= i_Data.m_BumpMapScale;
	m_UScale				= i_Data.m_UScale;
	m_VScale				= i_Data.m_VScale;
	m_UTrans				= i_Data.m_UTrans;
	m_VTrans				= i_Data.m_VTrans;
	m_UVAngle				= i_Data.m_UVAngle;
	m_Reflectivity			= i_Data.m_Reflectivity;
	m_TextureBase			= i_Data.m_TextureBase;
	m_TextureAlpha			= i_Data.m_TextureAlpha;
	m_TextureSpecularShift	= i_Data.m_TextureSpecularShift;
	m_TextureSpecularMask	= i_Data.m_TextureSpecularMask;
	m_TextureNormalMap		= i_Data.m_TextureNormalMap;
	m_SpecularColor0		= i_Data.m_SpecularColor0;
	m_SpecularColor1		= i_Data.m_SpecularColor1;
	m_SpecularExponent0		= i_Data.m_SpecularExponent0;
	m_SpecularExponent1		= i_Data.m_SpecularExponent1;
	m_SpecularShift0		= i_Data.m_SpecularShift0;
	m_SpecularShift1		= i_Data.m_SpecularShift1;

	return *this;
}

