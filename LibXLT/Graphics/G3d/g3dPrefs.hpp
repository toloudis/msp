/****************************************************************************\
**	g3dPrefs.hpp
**
**		Renderer preferences - global settings
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef G3D_PREFS_HPP
#error g3dPrefs.hpp already included
#endif
#define G3D_PREFS_HPP

#ifndef G3D_SCENERENDERERTYPES_HPP
#include "Graphics/g3d/g3dSceneRendererTypes.hpp"
#endif
#ifndef G3D_SCENERENDERENGINECREATE_HPP
#include "Graphics/g3d/g3dSceneRenderEngineCreate.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif


//============================================================================
//============================================================================
namespace g3dPrefs
{
	//------------------------------------------------------------------------
	// all primitive types/enums, so default copy constructor is fine.
	//------------------------------------------------------------------------
	struct g3dRenderPrefs
	{
		g3dSceneRendererTypes::RendererType m_RendererType;
		g3dSceneRenderEngineCreate::RenderEngine m_RendererEngine;
		bool m_bEnableAmbientPass;
		bool m_bEnableLitPass;
		bool m_bEnableTransparent;
		bool m_bEnableDOF;
		bool m_bEnableGlow;
		bool m_bEnableReflection;
		bool m_bEnableAO;
		bool m_bEnableEnvironment;
		bool m_bRenderMatte;
		bool m_bMultipassOn;
		bool m_bHeadlightOn;			// directional light in direction of camera
		bool m_bEnableOutline;

		bool m_bEnableSpecularLighting;
		bool m_bEnableDiffuseLighting;

		bool m_HDRToneMap;
		bool m_HDRBlueShift;
		int m_HDRDebugMode;

		bool m_bLowResolution;
		g3dSceneNode::DrawStyle m_DrawStyle;

		//bool m_RecalcAORequested;
		//bool m_bRecalcAOPerFrame;
		bool m_bAOInvalid;
		int m_nAOLights;			
		float m_AODepthBias;
		int m_AOResolutionReduce;

		bool m_bGIInvalid;			//variable for GI Volume and LPVGI
		int m_nGILights;			//variable for GI Volume
		float m_GIDepthBias;		//variable for GI Volume
		int m_GIResolutionReduce;	//variable for GI Volume

		bool m_bProjLightFrustumCull;
		bool m_bProjLightsOn;
		bool m_bPtLightsOn;
		bool m_bDoShadowMapGen;
		bool m_bEnableShadows;
		bool m_bEnableInvisibleCastShadows;
		bool m_bEnableInvisibleMaskBlack;
		bool m_bEnableInvisibleInReflections;

		bool m_bEnableDeferredTransparency;

		bool m_bHDRAA;

		bool m_bEnableSSAO;
		bool m_bEnableSSAOBlur;
		float m_SSAONumSteps;
		int m_SSAONumDirs;
		bool m_bEnableSSAODepthPeeling;
		int m_SSAONumLayers;
		bool m_bUseHardwareTessellation;
		float m_PixelSubdivLimit;
		bool m_bRenderWireframe;
		// gi vars
		bool m_bEnableSSGI;
		bool m_bEnableSSGIBlur;
		float m_SSGINumSteps;
		int m_SSGINumDirs;
		bool m_bEnableSSGIDepthPeeling;
		int m_SSGINumLayers;

		//motion blur vars
		bool m_bMotionBlurEnable;
		int m_MotionBlurSamples;
		float m_MotionBlurPercent;

		//transparency vars
		int m_TransparencyMode;
		bool m_bDebugDepthPeel;
		bool m_bDebugSinglePeel;
		int m_nDebugDepthPeelLayers;

		// keep track of do_render_capture() iteration
		int m_nDoRenderCaptureIteration;

		// keep track of whether we're loading a scene
		bool m_bLoadingScene;

		// illumination renderer accounts for normals
		bool m_bIlluminationUsesNormals;

		// ray tracing support - not functional yet!!
		bool m_bRTReflection;
		int m_RTNumReflBounces;
		bool m_bRTRefraction;
		int m_RTNumRefrBounces;
		bool m_bRTOnlySecondary;
		float m_RTReflFactor;
		float m_RTRefrFactor;

		bool m_bEnableHair;
		bool m_bHairLines;
		int m_HairShadowType;
		int m_HairShadowRes;
		float m_HairTessellation;
		unsigned int m_HairVertexLimit;
		unsigned int m_HairStrandSkip;
		int m_HairTransparencyMode;
		float m_HairSubPixelPower;
		bool m_bHairDepthPeel;
		int	m_nHairDepthPeelLayers;
		unsigned int m_HairInterpolationCount;
		float m_HairClumpRadius;

		g3dRenderPrefs();
	};
	
	//--------------------------------------------------------------------
	// Return the current set of renderprefs.
	//--------------------------------------------------------------------
	g3dRenderPrefs& CurrentPrefs();

	//--------------------------------------------------------------------
	// Install a set of renderprefs.
	// Prefs are not owned here.  Caller owns the pointer. Make sure that 
	// the pointer is not deleted without setting up new prefs here first!
	//--------------------------------------------------------------------
	void SetPrefs(g3dRenderPrefs* i_Prefs);


	//--------------------------------------------------------------------
	// Install the default set of renderprefs.
	//--------------------------------------------------------------------
	void SetDefaultPrefs();
}