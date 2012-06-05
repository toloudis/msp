/********************************************************************************************\
**  prjltData.cpp
**
**
**  Extra Large Technology
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PrjLt/Data/prjltData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltData::prjltData()
:	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", true),
	m_Target("Target Point", maPoint3d(0, 0, -1)),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 100.0f),
	m_Intensity("Intensity", 1.0f),
	m_Angle("Angle", 90.0f),
	m_Scale("Scale", 1.0f),
	m_Aspect("Aspect", 1.0f),
	m_Tilt("Tilt", 0.0f),
	m_TextureFilename("Texture Filename", itString("Projection.dds")),
	m_DepthMapSize("Shadow Map Res.", 512),
	m_ShadowQuality("Shadow Quality", false),
	m_LightSize("Shadow Softness", 0.001f),
	m_SceneScale("Scene Scale", 100.0f),
	m_ShadowIntensity("Shadow Intensity", 1.0f),
	m_DepthBias("Depth Bias", 0.002f),
	m_bEditorVisible("Visible in Editor", true),
	m_Orientation("Orientation",  maRotation( 0.0f, 0.0f, 0.0f ) ),
	m_Position("Position",  maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	m_Name("Name"),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_bShaftVisible("Shaft Visible", false),
	m_ShaftAlpha("Shaft Alpha", 0.35f),
	m_ShaftDensity("Edge Softness", 0.3f),
	m_ShaftDistFalloffStart("Shaft Falloff Start", 20.0f),
	m_ShaftDistFalloffEnd("Shaft Falloff End", 50.0f),
	m_ShaftTextureFilename("Shaft Texture", itString("")),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true),
	m_ShadowColor("Shadow Color", maFloatRGBA(0,0,0,0))
{
}

//-------------------------------------------------
//-------------------------------------------------
prjltData::prjltData(const prjltData& i_Data)
:	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", true),
	m_Target("Target Point", maPoint3d(0, 0, -1)),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 100.0f),
	m_Intensity("Intensity", 1.0f),
	m_Angle("Angle", 90.0f),
	m_Scale("Scale", 1.0f),
	m_Aspect("Aspect", 1.0f),
	m_Tilt("Tilt", 0.0f),
	m_TextureFilename("Texture Filename", itString("Projection.dds")),
	m_DepthMapSize("Depth Map Size", 512),
	m_ShadowQuality("Shadow Quality", false),
	m_LightSize("Light Size", 0.001f),
	m_SceneScale("Scene Scale", 100.0f),
	m_ShadowIntensity("Shadow Intensity", 1.0f),
	m_DepthBias("Depth Bias", 0.002f),
	m_bEditorVisible("Visible in Editor", true),
	m_Orientation("Orientation",  maRotation( 0.0f, 0.0f, 0.0f ) ),
	m_Position("Position",  maPoint3d( 0.0f, 0.0f, 0.0f ) ),
	m_Name("Name"),
	m_bDiffuseEnabled("Enable Diffuse", true),
	m_bSpecularEnabled("Enable Specular", true),
	m_bShaftVisible("Shaft Visible", false),
	m_ShaftAlpha("Shaft Alpha", 0.35f),
	m_ShaftDensity("Shaft Density", 0.3f),
	m_ShaftDistFalloffStart("Shaft Falloff Start", 20.0f),
	m_ShaftDistFalloffEnd("Shaft Falloff End", 50.0f),
	m_ShaftTextureFilename("Shaft Texture", itString("")),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true),
	m_ShadowColor("Shadow Color", maFloatRGBA(0,0,0,0))
{
	(*this) = i_Data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltData::~prjltData()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool prjltData::operator == (const prjltData& i_Item)
{
	return (this->m_Name == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltData& prjltData::operator=(const prjltData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_bEditorVisible	= i_Data.m_bEditorVisible;
	this->m_Name			= i_Data.m_Name;
	this->m_Orientation		= i_Data.m_Orientation;
	this->m_Position		= i_Data.m_Position;

	this->m_Color			= i_Data.m_Color;
	this->m_Enabled			= i_Data.m_Enabled;
	this->m_ShadowSource	= i_Data.m_ShadowSource;
	this->m_Falloff			= i_Data.m_Falloff;
	this->m_Range			= i_Data.m_Range;
	this->m_Intensity		= i_Data.m_Intensity;

	this->m_Target			= i_Data.m_Target;
	this->m_Angle			= i_Data.m_Angle;
	this->m_Scale			= i_Data.m_Scale;
	this->m_Aspect			= i_Data.m_Aspect;
	this->m_Tilt			= i_Data.m_Tilt;

	this->m_TextureFilename = i_Data.m_TextureFilename;
	this->m_DepthMapSize	= i_Data.m_DepthMapSize;

	this->m_LightSize		= i_Data.m_LightSize;
	this->m_SceneScale		= i_Data.m_SceneScale;
	this->m_ShadowQuality	= i_Data.m_ShadowQuality;
	this->m_ShadowIntensity	= i_Data.m_ShadowIntensity;
	this->m_DepthBias		= i_Data.m_DepthBias;

	this->m_bDiffuseEnabled	= i_Data.m_bDiffuseEnabled;
	this->m_bSpecularEnabled= i_Data.m_bSpecularEnabled;

	this->m_bShaftVisible	= i_Data.m_bShaftVisible;
	this->m_ShaftAlpha		= i_Data.m_ShaftAlpha;
	this->m_ShaftDensity	= i_Data.m_ShaftDensity;
	this->m_ShaftDistFalloffStart = i_Data.m_ShaftDistFalloffStart;
	this->m_ShaftDistFalloffEnd = i_Data.m_ShaftDistFalloffEnd;
	this->m_ShaftTextureFilename = i_Data.m_ShaftTextureFilename;

	this->m_bAffectsFur		= i_Data.m_bAffectsFur;
	this->m_bAffectsGlow	= i_Data.m_bAffectsGlow;

	this->m_ShadowColor		= i_Data.m_ShadowColor;

	return *this;
}
