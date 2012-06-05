/*****************************************************************************
**  g3dPrefs.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dPrefs.hpp"

//============================================================================
//============================================================================
namespace g3dPrefs
{
	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	g3dRenderPrefs::g3dRenderPrefs()
	:	m_RendererType(g3dSceneRendererTypes::e_Default),
	m_RendererEngine(g3dSceneRenderEngineCreate::e_Default),
		m_bEnableAmbientPass(true),
		m_bEnableLitPass(true),
		m_bEnableTransparent(true),
		m_bEnableDOF(true),
		m_bEnableGlow(true),
		m_bEnableReflection(true),
		m_bEnableAO(true),
		m_bEnableEnvironment(true),
		m_bRenderMatte(false),
		m_bMultipassOn(true),
		m_bHeadlightOn(false),
		m_bEnableOutline(true),

		m_bEnableSpecularLighting(true),
		m_bEnableDiffuseLighting(true),

		m_HDRToneMap(true),
		m_HDRBlueShift(false),
		m_HDRDebugMode(0),

		m_bLowResolution(false),
		m_DrawStyle(g3dSceneNode::e_Solid),

		//m_RecalcAORequested(false),
		//m_bRecalcAOPerFrame(false),
		m_bAOInvalid(false),
		m_nAOLights(16),
		m_AODepthBias(0.01f),
		m_AOResolutionReduce(0),

		m_bGIInvalid(false),
		m_nGILights(16),
		m_GIDepthBias(0.01f),
		m_GIResolutionReduce(0),

		m_bProjLightFrustumCull(true),
		m_bProjLightsOn(true),
		m_bPtLightsOn(true),
		m_bDoShadowMapGen(true),
		m_bEnableShadows(true),
		m_bEnableInvisibleCastShadows(true),
		m_bEnableInvisibleMaskBlack(true),
		m_bEnableInvisibleInReflections(true),

		m_bEnableDeferredTransparency(false),
		m_bHDRAA(true),
		m_bEnableSSAO(false),
		m_bEnableSSAOBlur(false),
		m_SSAONumSteps(8),
		m_SSAONumDirs(16),
		m_bEnableSSAODepthPeeling(false),
		m_SSAONumLayers(1),

		m_bEnableSSGI(false),
		m_bEnableSSGIBlur(false),
		m_SSGINumSteps(8),
		m_SSGINumDirs(16),
		m_bEnableSSGIDepthPeeling(false),
		m_SSGINumLayers(1),

		m_bUseHardwareTessellation(true),
		m_PixelSubdivLimit(4.0f),
		m_bRenderWireframe(false),

		m_bMotionBlurEnable(false),
		m_MotionBlurSamples(20),
		m_MotionBlurPercent(100.0f),

		m_TransparencyMode(0),
		m_bDebugDepthPeel(false),
		m_bDebugSinglePeel(false),
		m_nDebugDepthPeelLayers(0),

		m_nDoRenderCaptureIteration(-1),
		m_bLoadingScene(false),

		// illumination renderer accounts for normals
		m_bIlluminationUsesNormals(true),

		m_bRTOnlySecondary(false),
		m_RTReflFactor(1),
		m_RTRefrFactor(1),
		m_RTNumReflBounces(0),
		m_RTNumRefrBounces(0),
		m_bRTReflection(false),
		m_bRTRefraction(false),

		m_bEnableHair(false),
		m_bHairLines(false),
		m_HairShadowType(0),
		m_HairShadowRes(0),
		m_HairTessellation(1.0f),
		m_HairVertexLimit(50),
		m_HairStrandSkip(0),
		m_HairTransparencyMode(0),
		m_HairSubPixelPower(1.0f),
		m_bHairDepthPeel(false),
		m_nHairDepthPeelLayers(0),
		m_HairInterpolationCount(0),
		m_HairClumpRadius(0.0f)
	{
	}

	// State
	g3dRenderPrefs g_DefaultPrefs;

	// init to defaults
	g3dRenderPrefs* g_RenderPrefs = &g_DefaultPrefs;

	//--------------------------------------------------------------------
	// Return the current set of renderprefs.
	//--------------------------------------------------------------------
	g3dRenderPrefs& CurrentPrefs()
	{
		return *g_RenderPrefs;
	}

	//--------------------------------------------------------------------
	// Install a set of renderprefs.
	// Prefs are not owned here.  Caller owns the pointer. Make sure that 
	// the pointer is not deleted without setting up new prefs here first!
	//--------------------------------------------------------------------
	void SetPrefs(g3dRenderPrefs* i_Prefs)
	{
		if (i_Prefs != NULL)
			g_RenderPrefs = i_Prefs;
		else
			SetDefaultPrefs();
	}
	//--------------------------------------------------------------------
	// Install the default set of renderprefs.
	//--------------------------------------------------------------------
	void SetDefaultPrefs()
	{
		g_RenderPrefs = &g_DefaultPrefs;
	}
}