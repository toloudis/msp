/********************************************************************************************\
**  mtrlToonData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2007 - All Rights Reserved
\********************************************************************************************/
#include "Support/mtrl/GUI/mtrlToonData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlToonData::mtrlToonData()
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorMidtone("Midtone", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_Transparency("Transparency", 1.0f),
	m_Smoothness("Smoothness", 500.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_Transition1("Ambient to Midtone", 0.0f ),
	m_Transition2("Midtone to Diffuse", 0.0f ),
	m_Transition3("Diffuse to Specular", 0.0f ),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureGradientMap("Gradient Map", itString("")),
	m_TextureTransparencyMap("Transparency Map", itString("")),
	m_SpecularEnable("Specular Enable", true)
{
}

//-------------------------------------------------
//-------------------------------------------------
mtrlToonData::mtrlToonData(const mtrlToonData& i_Data)
:	m_ColorAmbient("Ambient", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorMidtone("Midtone", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_ColorDiffuse("Diffuse", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_ColorSpecular("Specular", maFloatRGBA(1.0f, 1.0f, 1.0f, 1.0f)),
	m_Transparency("Transparency", 1.0f),
	m_Smoothness("Smoothness", 500.0f),
	m_UScale("U Scale", 1.0f),
	m_VScale("V Scale", 1.0f),
	m_UTrans("U Translate", 0.0f),
	m_VTrans("V Translate", 0.0f),
	m_UVAngle("UV Rotate", 0.0f),
	m_Transition1("Ambient to Midtone", 0.0f ),
	m_Transition2("Midtone to Diffuse", 0.0f ),
	m_Transition3("Diffuse to Specular", 0.0f ),
	m_TextureDiffuse("Diffuse Map", itString("")),
	m_TextureGradientMap("Gradient Map", itString("")),
	m_TextureTransparencyMap("Transprency Map", itString("")),
	m_SpecularEnable("Specular Enable", true )
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlToonData::~mtrlToonData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
mtrlToonData& mtrlToonData::operator=(const mtrlToonData& i_Data)
{
	if (this == &i_Data) return *this;

	m_ColorAmbient			= i_Data.m_ColorAmbient;
	m_ColorMidtone			= i_Data.m_ColorMidtone;
	m_ColorDiffuse			= i_Data.m_ColorDiffuse;
	m_ColorSpecular			= i_Data.m_ColorSpecular;
	m_Transparency			= i_Data.m_Transparency;
	m_Smoothness			= i_Data.m_Smoothness;
	m_UScale				= i_Data.m_UScale;
	m_VScale				= i_Data.m_VScale;
	m_UTrans				= i_Data.m_UTrans;
	m_VTrans				= i_Data.m_VTrans;
	m_UVAngle				= i_Data.m_UVAngle;
	m_Transition1			= i_Data.m_Transition1;
	m_Transition2			= i_Data.m_Transition2;
	m_Transition3			= i_Data.m_Transition3;
	m_TextureDiffuse		= i_Data.m_TextureDiffuse;
	m_TextureGradientMap	= i_Data.m_TextureGradientMap;
	m_TextureTransparencyMap = i_Data.m_TextureTransparencyMap;
	m_SpecularEnable		= i_Data.m_SpecularEnable;

	return *this;
}

