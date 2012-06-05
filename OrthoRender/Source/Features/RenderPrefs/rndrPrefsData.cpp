/****************************************************************************\
**  rndrPrefsData.cpp
**
**		see .hpp
**
**  Extra Large Technology
**  Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderPrefs/rndrPrefsData.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rndrPrefsData::rndrPrefsData()
:	m_bShadowsOn("Multipass Lighting",true),
	m_bEnableDOF("Render DOF",true),
	m_bEnableFur("Render Fur",true),
	m_bEnableGlow("Render Glow",true),
	m_bEnableOutline("Render Outline",true),
	m_bEnableAmbientPass("Render Ambient",true),
	m_bEnableLitPass("Render Lit",true),
	m_bEnableTransparent("Render Transparent",true),
	m_bMatteMode("Render Matte",false),
	//m_bHeadlightOn("Headlight On",false),
	m_RendererType("Renderer", 0),
	m_bBlueShift("Blue Shift", false),
	m_bToneMap("Tone Map", true),
	m_HDRDebugMode("Debug Mode", 0),
	m_FurQuality("Fur Quality", 1),
	m_bEnableReflection("Render Reflections", true),
	m_bEnableEnvironment("Render Environments", true),
	m_bLowResolution("Low Resolution", false),
	m_bEnableAO("Ambient Occlusion", false),
	m_bRecalcAOPerFrame("Recalc Per Frame", false),
	m_bProjLightFrustumCull("ProjLt Frustum Cull", true),
	m_bProjLightsOn("Proj Lights on", true),
	m_bPtLightsOn("Pt lights on", true),
	m_bDoShadowMapGen("Shadow Map Gen Pass", true),
	m_bEnableDeferredTransparency("Draw Particles Last", false),
	m_bHDRAA("HDR Antialiasing", false),
	m_bEnableSSAO("Enable SSAO", false),
	m_SSAOQuality("Sampling Preset", 0),  
	m_SSAONumSteps("Num Steps", 8),
	m_SSAONumDirs("Num Dirs", 16),
	m_SSAOEnableBlur("Enable Blur", true)
{
	m_HDRDebugMode.SetEnumTag(0, "Full Render");
	m_HDRDebugMode.SetEnumTag(1, "Clamped HDR Buffer");
	m_HDRDebugMode.SetEnumTag(2, "Scaled HDR Buffer");
	m_HDRDebugMode.SetEnumTag(3, "Pixel Luminances");
	m_HDRDebugMode.SetEnumTag(4, "DOF Blurriness");
	m_HDRDebugMode.SetEnumTag(5, "1st Luminance pass");
	m_HDRDebugMode.SetEnumTag(6, "Bright pass");
	m_HDRDebugMode.SetEnumTag(7, "Bloom source");
	m_HDRDebugMode.SetEnumTag(8, "Bloom");
	m_HDRDebugMode.SetEnumTag(9, "Star");

	m_RendererType.SetEnumTag(0, "Default HDR");
	m_RendererType.SetEnumTag(1, "AO Only");
	m_RendererType.SetEnumTag(2, "Depth Buffer");

	m_SSAOQuality.SetEnumTag(0, "Low");
	m_SSAOQuality.SetEnumTag(1, "Medium");
	m_SSAOQuality.SetEnumTag(2, "High");
}


