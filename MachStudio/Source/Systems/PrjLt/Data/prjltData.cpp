/********************************************************************************************\
**  prjltData.cpp
**
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\********************************************************************************************/
#include "Systems/PrjLt/Data/prjltData.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/G3d/g3dProjectedLight.hpp"

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prjltData::prjltData()
:	m_LightType(e_ProjectedLight),
	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", true),
	m_Target("Target Point", maPoint3d(0, 0, -20)),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 500),
	m_Intensity("Intensity", 1.0f),
	m_Angle("Angle", 45.0f),
	m_Scale("Scale", 10),
	m_Aspect("Aspect", 1.0f),
	m_Tilt("Tilt", 0.0f),
	m_bDirectional("Directional", false),
	m_bConeLighting("Cone Lighting", false),
	m_Penumbra("Penumbra", 1.0f),
	m_TextureFilename("Texture Filename", fsLocator()), //bga - removing "Projection.dds"
	m_DepthMapSize("Shadow Map Res.", 1024),
	m_ShadowQuality("Shadow Sampling", 1),
	m_LightSize("Shadow Softness", 0.04f),
	m_PCSSAdjust("Distance Softness Scale", 0.0f),
	m_SceneScale("Scene Scale", 1.0f),
	m_ShadowIntensity("Shadow Intensity", 1.0f),
	m_DepthBias("Depth Bias", 0.04f),
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
	m_ShaftTextureFilename("Shaft Texture",  fsLocator()),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true),
	m_ShadowColor("Shadow Color", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_bEnabledRamp("Enable Ramp Texture", false),
	m_RampTrigger("Ramp Edit"),
	/*m_RampGradient("Ramp", maGradient()),
	m_RampShape("Ramp Shape", 0),
	m_RampInterpolation("Ramp Interpolation", 0),
	m_RampTexSize("Ramp Texture Size", 512),
	m_RampUWave("U Wave", 0.0f),
	m_RampVWave("V Wave", 0.0f),
	m_RampNoise("Noise", 0.0f),
	m_RampNoiseFreq("Noise Frequency", 0.5f)*/
	m_HairShadowEnable("Hair Enable Shadow", false ),
	m_HairShadowSize("Hair Shadow Res", 64 ),
	m_HairShadowBias("Depth Bias", 0.0f ),
	m_HairShadowDensity("Density", 1.0f ),
	m_HairMinBound("Min Bounds", 0.0f ),
	m_HairMaxBound("Max Bounds", 1000000.0f ),
	m_HairShadowType("Hair Shadow Type"),
	m_GISource("GI Source", false),
	m_ReflectiveMapSize("GI Map Size", 256),
	m_bMRayAreaLight("Enable Area Light", false),
	m_MRayAreaLightType("Area Light Type", 0),
	m_MRayAreaLightSampling("Area Light Sampling", 8),
	m_bMRayAreaLightVisible("Visible", false)
{
	m_ShadowQuality.SetEnumTag(0, "Low");
	m_ShadowQuality.SetEnumTag(1, "Medium");
	m_ShadowQuality.SetEnumTag(2, "High");
	m_ShadowQuality.SetEnumTag(3, "Very High");

	m_MRayAreaLightType.SetEnumTag(0, "Rectangle");
	m_MRayAreaLightType.SetEnumTag(1, "Disc");

	/*m_RampShape.SetEnumTag(0, "Linear X");
	m_RampShape.SetEnumTag(1, "Linear Y");
	m_RampShape.SetEnumTag(2, "Circular");
	m_RampShape.SetEnumTag(3, "Square");

	m_RampInterpolation.SetEnumTag(0, "Linear");*/

	m_HairShadowType.SetEnumTag( HAIR_SHADOW_OSM4, "Opacity Shadow Map (4 Layers)");
	m_HairShadowType.SetEnumTag( HAIR_SHADOW_OSM16, "Opacity Shadow Map (16 Layers)");
	m_HairShadowType.SetEnumTag( HAIR_SHADOW_OSM32, "Opacity Shadow Map (32 Layers)");
	m_HairShadowType.SetEnumTag( HAIR_SHADOW_DOSM4, "Deep Shadow Map (4 Layers)");
	m_HairShadowType.SetEnumTag( HAIR_SHADOW_DOSM16, "Deep Shadow Map (16 Layers)");
	m_HairShadowType.SetEnumTag( HAIR_SHADOW_DOSM32, "Deep Shadow Map (32 Layers)");
//	m_HairShadowType.SetEnumTag( HAIR_SHADOW_VOLUME4, "Volume Opacity Map (4 Layers)");
}

//-------------------------------------------------
//-------------------------------------------------
prjltData::prjltData(const prjltData& i_Data)
:	m_LightType(e_ProjectedLight),
	m_Color("Color", maFloatRGBA(0.5f, 0.5f, 0.5f, 1.0f)),
	m_Enabled("Enabled", true),
	m_ShadowSource("Shadow Source", true),
	m_Target("Target Point", maPoint3d(0, 0, -1)),
	m_Falloff("Falloff", maPoint3d(1.0f, 0.0f, 0.0f)),
	m_Range("Range", 500),
	m_Intensity("Intensity", 1.0f),
	m_Angle("Angle", 45.0f),
	m_Scale("Scale", 10),
	m_Aspect("Aspect", 1.0f),
	m_Tilt("Tilt", 0.0f),
	m_bDirectional("Directional", false),
	m_bConeLighting("Cone Lighting", false),
	m_Penumbra("Penumbra", 1.0f),
	m_TextureFilename("Texture Filename", fsLocator()), //bga - removing "Projection.dds"
	m_DepthMapSize("Depth Map Size", 1024),
	m_ShadowQuality("Shadow Sampling", 1),
	m_LightSize("Shadow Softness", 0.04f),
	m_PCSSAdjust("Distance Softness Scale", 0.0f),
	m_SceneScale("Scene Scale", 1.0f),
	m_ShadowIntensity("Shadow Intensity", 1.0f),
	m_DepthBias("Depth Bias", 0.04f),
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
	m_ShaftTextureFilename("Shaft Texture", fsLocator()),
	m_bAffectsFur("Affects Fur", true),
	m_bAffectsGlow("Affects Glow", true),
	m_ShadowColor("Shadow Color", maFloatRGBA(0.0f, 0.0f, 0.0f, 0.0f)),
	m_bEnabledRamp("Enable Ramp Texture", false),
	m_RampTrigger("Ramp Edit"),
	m_GISource("GI Source", false),
	m_ReflectiveMapSize("GI Map Size", 256),
	m_bMRayAreaLight("Enable Area Light", false),
	m_MRayAreaLightSampling("Area Light Sampling", 8),
	m_MRayAreaLightType("Area Light Type", 0),
	m_bMRayAreaLightVisible("Visible", false)
	/*m_RampGradient("Ramp", maGradient()),
	m_RampShape("Ramp Shape", 0),
	m_RampInterpolation("Ramp Interpolation", 0),
	m_RampTexSize("Ramp Texture Size", 512),
	m_RampUWave("U Wave", 0.0f),
	m_RampVWave("V Wave", 0.0f),
	m_RampNoise("Noise", 0.0f),
	m_RampNoiseFreq("Noise Frequency", 0.5f)*/
{
	m_ShadowQuality.SetEnumTag(0, "Low");
	m_ShadowQuality.SetEnumTag(1, "Medium");
	m_ShadowQuality.SetEnumTag(2, "High");
	m_ShadowQuality.SetEnumTag(3, "Very High");

	/*m_RampShape.SetEnumTag(0, "Linear X");
	m_RampShape.SetEnumTag(1, "Linear Y");
	m_RampShape.SetEnumTag(2, "Circular");
	m_RampShape.SetEnumTag(3, "Square");

	m_RampInterpolation.SetEnumTag(0, "Linear");*/

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

	this->m_LightType		= i_Data.m_LightType;
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
	this->m_bDirectional	= i_Data.m_bDirectional;
	this->m_bConeLighting	= i_Data.m_bConeLighting;
	this->m_Penumbra		= i_Data.m_Penumbra;

	this->m_TextureFilename = i_Data.m_TextureFilename;
	this->m_DepthMapSize	= i_Data.m_DepthMapSize;

	this->m_LightSize		= i_Data.m_LightSize;
	this->m_PCSSAdjust		= i_Data.m_PCSSAdjust;
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
	this->m_bEnabledRamp	= i_Data.m_bEnabledRamp;
	this->m_bEnabledShaftRamp	= i_Data.m_bEnabledShaftRamp;
	this->m_RampTrigger		= i_Data.m_RampTrigger;
	this->m_RampData		= i_Data.m_RampData;
	this->m_ShaftRampData		= i_Data.m_ShaftRampData;

	/*this->m_RampGradient	= i_Data.m_RampGradient;
	this->m_RampShape		= i_Data.m_RampShape;
	this->m_RampInterpolation = i_Data.m_RampInterpolation;
	this->m_RampTexSize		= i_Data.m_RampTexSize;
	this->m_RampUWave		= i_Data.m_RampUWave;
	this->m_RampVWave		= i_Data.m_RampVWave;
	this->m_RampNoise		= i_Data.m_RampNoise;
	this->m_RampNoiseFreq	= i_Data.m_RampNoiseFreq;*/

	this->m_HairShadowEnable = i_Data.m_HairShadowEnable;
	this->m_HairShadowSize	= i_Data.m_HairShadowSize;
	this->m_HairShadowType	= i_Data.m_HairShadowType;
	this->m_HairShadowBias	= i_Data.m_HairShadowBias;
	this->m_HairShadowDensity = i_Data.m_HairShadowDensity;
	this->m_HairMinBound	= i_Data.m_HairMinBound;
	this->m_HairMaxBound	= i_Data.m_HairMaxBound;

	this->m_GISource			= i_Data.m_GISource;

	this->m_bMRayAreaLight			= i_Data.m_bMRayAreaLight;
	this->m_MRayAreaLightSampling	= i_Data.m_MRayAreaLightSampling;
	this->m_MRayAreaLightType		= i_Data.m_MRayAreaLightType;
	this->m_bMRayAreaLightVisible	= i_Data.m_bMRayAreaLightVisible;

	return *this;
}
