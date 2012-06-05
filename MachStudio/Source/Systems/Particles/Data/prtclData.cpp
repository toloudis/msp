/****************************************************************************\
**  prtclData.cpp
**
**		see .hpp
**
**  StudioGPU
**  Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Data/prtclData.hpp"

#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace
{
	float c_InitialStreakLength = 0.4f; 
}


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclData::prtclData()
:	m_bEditorVisible("VisibleInEditor", true),
	m_Name("Name"),
	m_Filename("Filename"),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Rate("Rate"),
	m_MaxParticles("MaxParticles"),
	m_LifetimeMin("LifetimeMin"),
	m_LifetimeMax("LifetimeMax"),
	m_ScaleStart("ScaleStart"),
	m_ScaleCoefficient("ScaleCoefficient"),
	m_ScaleMode("ScaleMode"),
	m_StartAngleMin("StartAngleMin"),
	m_StartAngleMax("StartAngleMax"),
	m_AngularVelocityMin("AngularVelocityMin"),
	m_AngularVelocityMax("AngularVelocityMax"),
	m_AngularAccelerationMin("AngularAccelerationMin"),
	m_AngularAccelerationMax("AngularAccelerationMax"),
	m_EmitterScale("EmitterScale"),
	m_ConeAngle("ConeAngle"),	// cone
	m_MinSpeed("MinSpeed"),
	m_MaxSpeed("MaxSpeed"),
	m_AccelerationX("AccelerationX"),
	m_AccelerationY("AccelerationY"),
	m_AccelerationZ("AccelerationZ"),
	m_MinEmitSpeed("MinEmitSpeed"),	// prtSpiralParticleGenerator
	m_MaxEmitSpeed("MaxEmitSpeed"),
	m_EmitDirectionX("EmitDirectionX"),
	m_EmitDirectionY("EmitDirectionY"),
	m_EmitDirectionZ("EmitDirectionZ"),
	m_MinRotStartAngle("MinRotStartAngle"),
	m_MaxRotStartAngle("MaxRotStartAngle"),
	m_MinRotAngularVel("MinRotAngularVel"),
	m_MaxRotAngularVel("MaxRotAngularVel"),
	m_PreSimTime("PreSimTime", 0.0f),
	m_RotRadius("RotRadius"),
	m_RotRadiusScaleRate("RotRadiusScaleRate"),
	m_TextureAlphaStart("AlphaStart"),
	m_TextureAlphaMiddle("AlphaMiddle"),
	m_TextureAlphaEnd("AlphaEnd"),
	m_TextureAlphaMiddlePercentStart("AlphaMiddlePercentStart"),
	m_TextureAlphaMiddlePercentEnd("AlphaMiddlePercentEnd"),
	m_TextureFilename("TextureFilename"),
	m_TextureRows("TextureRows",1),
	m_TextureCols("TextureCols",1),
	m_bTextureLooping("TextureLoops",true),
	m_bTextureReverse("TextureReverse", false),
	m_TextureRate("TextureRate", 24.0f),
	m_TextureUVAMode("TextureUVAMode"),
	m_bRenderStreaks("Render Streaks", false),
	m_StreakLength("Streak Length", c_InitialStreakLength),
	m_StreakTaper("Streak Taper", 1),
	m_StreakFade("Streak Fade", 1),
	m_bShowInCubeReflections("Reflected Cubic", false),
	m_bShowInPlanarReflections("Reflected Planar", true),
	m_bCastShadows("Casts Shadows", true),
	m_bUseDitheredShadows("Use Dithered Shadows", true),
	m_ShadowDitherBias("Dither Bias", 0.85f),
	m_bAdditive("Use Additive Blend", false)
{
	// let this order be the same as prtSpriteGroupParticleGenerator::UVAMode
	// so we can simply cast the enum value
	m_TextureUVAMode.SetEnumTag(0,"Lifetime");
	m_TextureUVAMode.SetEnumTag(1,"Random");
	m_TextureUVAMode.SetEnumTag(2,"TUV Framerate");
	// let this order be the same as prtSpriteGroupParticleGenerator::ScaleMode
	// so we can simply cast the enum value
	m_ScaleMode.SetEnumTag(0, "Linear");
	m_ScaleMode.SetEnumTag(1, "Exponential");
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclData::prtclData(const itString& i_Filename )
:	m_bEditorVisible("VisibleInEditor", true),
	m_Name("Name"),
	m_Filename("Filename"),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Rate("Rate"),
	m_MaxParticles("MaxParticles"),
	m_LifetimeMin("LifetimeMin"),
	m_LifetimeMax("LifetimeMax"),
	m_ScaleStart("ScaleStart"),
	m_ScaleCoefficient("ScaleCoefficient"),
	m_ScaleMode("ScaleMode"),
	m_StartAngleMin("StartAngleMin"),
	m_StartAngleMax("StartAngleMax"),
	m_AngularVelocityMin("AngularVelocityMin"),
	m_AngularVelocityMax("AngularVelocityMax"),
	m_AngularAccelerationMin("AngularAccelerationMin"),
	m_AngularAccelerationMax("AngularAccelerationMax"),
	m_EmitterScale("EmitterScale"),
	m_ConeAngle("ConeAngle"),	// cone
	m_MinSpeed("MinSpeed"),
	m_MaxSpeed("MaxSpeed"),
	m_AccelerationX("AccelerationX"),
	m_AccelerationY("AccelerationY"),
	m_AccelerationZ("AccelerationZ"),
	m_MinEmitSpeed("MinEmitSpeed"),	// prtSpiralParticleGenerator
	m_MaxEmitSpeed("MaxEmitSpeed"),
	m_EmitDirectionX("EmitDirectionX"),
	m_EmitDirectionY("EmitDirectionY"),
	m_EmitDirectionZ("EmitDirectionZ"),
	m_MinRotStartAngle("MinRotStartAngle"),
	m_MaxRotStartAngle("MaxRotStartAngle"),
	m_MinRotAngularVel("MinRotAngularVel"),
	m_MaxRotAngularVel("MaxRotAngularVel"),
	m_PreSimTime("PreSimTime", 0.0f),
	m_RotRadius("RotRadius"),
	m_RotRadiusScaleRate("RotRadiusScaleRate"),
	m_TextureAlphaStart("AlphaStart"),
	m_TextureAlphaMiddle("AlphaMiddle"),
	m_TextureAlphaEnd("AlphaEnd"),
	m_TextureAlphaMiddlePercentStart("AlphaMiddlePercentStart"),
	m_TextureAlphaMiddlePercentEnd("AlphaMiddlePercentEnd"),
	m_TextureFilename("TextureFilename"),
	m_TextureRows("TextureRows",1),
	m_TextureCols("TextureCols",1),
	m_bTextureLooping("TextureLoops",true),
	m_bTextureReverse("TextureReverse", false),
	m_TextureRate("TextureRate", 24.0f),
	m_TextureUVAMode("TextureUVAMode"),
	m_bRenderStreaks("Render Streaks", false),
	m_StreakLength("Streak Length", c_InitialStreakLength),
	m_StreakTaper("Streak Taper", 1),
	m_StreakFade("Streak Fade", 1),
	m_bShowInCubeReflections("Reflected Cubic", false),
	m_bShowInPlanarReflections("Reflected Planar", true),
	m_bCastShadows("Casts Shadows", true),
	m_bUseDitheredShadows("Use Dithered Shadows", true),
	m_ShadowDitherBias("Dither Bias", 0.85f),
	m_bAdditive("Use Additive Blend", false)
{
	// let this order be the same as prtSpriteGroupParticleGenerator::UVAMode
	// so we can simply cast the enum value
	m_TextureUVAMode.SetEnumTag(0,"Lifetime");
	m_TextureUVAMode.SetEnumTag(1,"Random");
	m_TextureUVAMode.SetEnumTag(2,"TUV Framerate");
	// let this order be the same as prtSpriteGroupParticleGenerator::ScaleMode
	// so we can simply cast the enum value
	m_ScaleMode.SetEnumTag(0, "Linear");
	m_ScaleMode.SetEnumTag(1, "Exponential");

	m_Filename = i_Filename;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclData::prtclData(	const itString& i_Filename,
						const maPoint3d& i_Position,
						const maRotation& i_Orientation )
:	m_bEditorVisible("VisibleInEditor", true),
	m_Name("Name"),
	m_Filename("Filename"),
	m_Position("Position"),
	m_Orientation("Orientation"),
	m_Rate("Rate"),
	m_MaxParticles("MaxParticles"),
	m_LifetimeMin("LifetimeMin"),
	m_LifetimeMax("LifetimeMax"),
	m_ScaleStart("ScaleStart"),
	m_ScaleCoefficient("ScaleCoefficient"),
	m_ScaleMode("ScaleMode"),
	m_StartAngleMin("StartAngleMin"),
	m_StartAngleMax("StartAngleMax"),
	m_AngularVelocityMin("AngularVelocityMin"),
	m_AngularVelocityMax("AngularVelocityMax"),
	m_AngularAccelerationMin("AngularAccelerationMin"),
	m_AngularAccelerationMax("AngularAccelerationMax"),
	m_EmitterScale("EmitterScale"),
	m_ConeAngle("ConeAngle"),	// cone
	m_MinSpeed("MinSpeed"),
	m_MaxSpeed("MaxSpeed"),
	m_AccelerationX("AccelerationX"),
	m_AccelerationY("AccelerationY"),
	m_AccelerationZ("AccelerationZ"),
	m_MinEmitSpeed("MinEmitSpeed"),	// prtSpiralParticleGenerator
	m_MaxEmitSpeed("MaxEmitSpeed"),
	m_EmitDirectionX("EmitDirectionX"),
	m_EmitDirectionY("EmitDirectionY"),
	m_EmitDirectionZ("EmitDirectionZ"),
	m_MinRotStartAngle("MinRotStartAngle"),
	m_MaxRotStartAngle("MaxRotStartAngle"),
	m_MinRotAngularVel("MinRotAngularVel"),
	m_MaxRotAngularVel("MaxRotAngularVel"),
	m_PreSimTime("PreSimTime", 0.0f),
	m_RotRadius("RotRadius"),
	m_RotRadiusScaleRate("RotRadiusScaleRate"),
	m_TextureAlphaStart("AlphaStart"),
	m_TextureAlphaMiddle("AlphaMiddle"),
	m_TextureAlphaEnd("AlphaEnd"),
	m_TextureAlphaMiddlePercentStart("AlphaMiddlePercentStart"),
	m_TextureAlphaMiddlePercentEnd("AlphaMiddlePercentEnd"),
	m_TextureFilename("TextureFilename"),
	m_TextureRows("TextureRows",1),
	m_TextureCols("TextureCols",1),
	m_bTextureLooping("TextureLoops",true),
	m_bTextureReverse("TextureReverse", false),
	m_TextureRate("TextureRate", 24.0f),
	m_TextureUVAMode("TextureUVAMode"),
	m_bRenderStreaks("Render Streaks", false),
	m_StreakLength("Streak Length", c_InitialStreakLength),
	m_StreakTaper("Streak Taper", 1),
	m_StreakFade("Streak Fade", 1),
	m_bShowInCubeReflections("Reflected Cubic", false),
	m_bShowInPlanarReflections("Reflected Planar", true),
	m_bCastShadows("Casts Shadows", true),
	m_bUseDitheredShadows("Use Dithered Shadows", true),
	m_ShadowDitherBias("Dither Bias", 0.85f),
	m_bAdditive("Use Additive Blend", false)
{
	// let this order be the same as prtSpriteGroupParticleGenerator::UVAMode
	// so we can simply cast the enum value
	m_TextureUVAMode.SetEnumTag(0,"Lifetime");
	m_TextureUVAMode.SetEnumTag(1,"Random");
	m_TextureUVAMode.SetEnumTag(2,"TUV Framerate");
	// let this order be the same as prtSpriteGroupParticleGenerator::ScaleMode
	// so we can simply cast the enum value
	m_ScaleMode.SetEnumTag(0, "Linear");
	m_ScaleMode.SetEnumTag(1, "Exponential");

	m_Filename		= i_Filename;
	m_Position		= i_Position;
	m_Orientation	= i_Orientation;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool prtclData::operator == (const prtclData& i_Item)
{
	return ((this->m_Name) == i_Item.m_Name);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
prtclData& prtclData::operator=(const prtclData& i_Data)
{
	if (this == &i_Data) return *this;

	this->m_bEditorVisible		= i_Data.m_bEditorVisible;
	this->m_Name				= i_Data.m_Name;
	this->m_Filename			= i_Data.m_Filename;
	this->m_Orientation			= i_Data.m_Orientation;
	this->m_Position			= i_Data.m_Position;
	this->m_Rate				= i_Data.m_Rate;
	this->m_MaxParticles		= i_Data.m_MaxParticles;
	this->m_LifetimeMin			= i_Data.m_LifetimeMin;
	this->m_LifetimeMax			= i_Data.m_LifetimeMax;
	this->m_ScaleStart			= i_Data.m_ScaleStart;
	this->m_ScaleCoefficient	= i_Data.m_ScaleCoefficient;
	this->m_ScaleMode			= i_Data.m_ScaleMode;
	this->m_StartAngleMin		= i_Data.m_StartAngleMin;
	this->m_StartAngleMax		= i_Data.m_StartAngleMax;
	this->m_AngularVelocityMin	= i_Data.m_AngularVelocityMin;
	this->m_AngularVelocityMax	= i_Data.m_AngularVelocityMax;
	this->m_AngularAccelerationMin	= i_Data.m_AngularAccelerationMin;
	this->m_AngularAccelerationMax	= i_Data.m_AngularAccelerationMax;
	this->m_EmitterScale		= i_Data.m_EmitterScale;
	this->m_ConeAngle			= i_Data.m_ConeAngle;	// cone
	this->m_MinSpeed			= i_Data.m_MinSpeed;
	this->m_MaxSpeed			= i_Data.m_MaxSpeed;
	this->m_AccelerationX		= i_Data.m_AccelerationX;
	this->m_AccelerationY		= i_Data.m_AccelerationY;
	this->m_AccelerationZ		= i_Data.m_AccelerationZ;
	this->m_MinEmitSpeed		= i_Data.m_MinEmitSpeed;	// prtSpiralParticleGenerator
	this->m_MaxEmitSpeed		= i_Data.m_MaxEmitSpeed;
	this->m_EmitDirectionX		= i_Data.m_EmitDirectionX;
	this->m_EmitDirectionY		= i_Data.m_EmitDirectionY;
	this->m_EmitDirectionZ		= i_Data.m_EmitDirectionZ;
	this->m_MinRotStartAngle	= i_Data.m_MinRotStartAngle;
	this->m_MaxRotStartAngle	= i_Data.m_MaxRotStartAngle;
	this->m_MinRotAngularVel	= i_Data.m_MinRotAngularVel;
	this->m_MaxRotAngularVel	= i_Data.m_MaxRotAngularVel;
	this->m_PreSimTime			= i_Data.m_PreSimTime;
	this->m_RotRadius			= i_Data.m_RotRadius;
	this->m_RotRadiusScaleRate	= i_Data.m_RotRadiusScaleRate;
	this->m_TextureAlphaStart	= i_Data.m_TextureAlphaStart;
	this->m_TextureAlphaMiddle	= i_Data.m_TextureAlphaMiddle;
	this->m_TextureAlphaEnd		= i_Data.m_TextureAlphaEnd;
	this->m_TextureAlphaMiddlePercentStart	= i_Data.m_TextureAlphaMiddlePercentStart;
	this->m_TextureAlphaMiddlePercentEnd	= i_Data.m_TextureAlphaMiddlePercentEnd;
	this->m_TextureFilename		= i_Data.m_TextureFilename;
	this->m_TextureRows			= i_Data.m_TextureRows;
	this->m_TextureCols			= i_Data.m_TextureCols;
	this->m_bTextureLooping		= i_Data.m_bTextureLooping;
	this->m_bTextureReverse		= i_Data.m_bTextureReverse;
	this->m_TextureRate			= i_Data.m_TextureRate;
	this->m_TextureUVAMode		= i_Data.m_TextureUVAMode;
	this->m_bRenderStreaks		= i_Data.m_bRenderStreaks;
	this->m_StreakLength		= i_Data.m_StreakLength;
	this->m_StreakTaper			= i_Data.m_StreakTaper;
	this->m_StreakFade			= i_Data.m_StreakFade;
	this->m_bShowInCubeReflections		= i_Data.m_bShowInCubeReflections;
	this->m_bShowInPlanarReflections	= i_Data.m_bShowInPlanarReflections;
	this->m_bCastShadows		= i_Data.m_bCastShadows;
	this->m_bUseDitheredShadows	= i_Data.m_bUseDitheredShadows;
	this->m_ShadowDitherBias	= i_Data.m_ShadowDitherBias;
	this->m_bAdditive			= i_Data.m_bAdditive;

	return *this;
}
