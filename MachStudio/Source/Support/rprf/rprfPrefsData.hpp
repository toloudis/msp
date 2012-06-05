/********************************************************************************************\
**	rprfPrefsData.hpp
**
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\********************************************************************************************/
#pragma once

#ifdef RPRFPREFSDATA_HPP
#error rprfPrefsData.hpp multiply included
#endif
#define RPRFPREFSDATA_HPP

#ifndef PRTY_BOOLEAN_HPP
#include "Core/prty/prtyBoolean.hpp"
#endif
#ifndef PRTY_COLOR_HPP
#include "Core/prty/prtyColor.hpp"
#endif
#ifndef PRTY_FLOAT_HPP
#include "Core/prty/prtyFloat.hpp"
#endif
#ifndef PRTY_ENUM_HPP
#include "Core/prty/prtyEnum.hpp"
#endif
#ifndef PRTY_FILEPATH_HPP
#include "Core/prty/prtyFilePath.hpp"
#endif
#ifndef PRTY_FILENAME_HPP
#include "Core/prty/prtyFileName.hpp"
#endif
//#ifndef PRTY_OBJECT_HPP
//#include "Core/prty/prtyObject.hpp"
//#endif
#ifndef PRTY_INT8_HPP
#include "Core/prty/prtyInt8.hpp"
#endif 
#ifndef PRTY_INT32_HPP
#include "Core/prty/prtyInt32.hpp"
#endif 


//============================================================================
//============================================================================
class rprfPrefsData
{
public:
	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	rprfPrefsData();

	//---------------------------------------------------------------------------
	//	Shadows data
	//---------------------------------------------------------------------------
	prtyBoolean m_bMultipassOn;

	//---------------------------------------------------------------------------
	//	DOF data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableDOF;

	//---------------------------------------------------------------------------
	//	Glow data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableGlow;

	//---------------------------------------------------------------------------
	//	Matte data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bMatteMode;

	//---------------------------------------------------------------------------
	//	Headlight - directional light in direction of camera
	//---------------------------------------------------------------------------
	//prtyBoolean m_bHeadlightOn;

	//---------------------------------------------------------------------------
	//	Outline data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableOutline;

	prtyEnum m_RendererEngine;
	prtyEnum m_RendererType;
	prtyEnum m_RendererTypeVP;

	// which renderpass to show (viewport)
	// these are combo box UI indices!!
	enum eRenderPassChoice
	{
		e_PassBeauty,
		e_PassDiffuse,
		e_PassDiffEnv,
		e_PassDiffLit,
		e_PassSpecular,
		e_PassSpecEnv,
		e_PassSpecLit,
		e_PassEmissive,
		e_PassAO,
		e_PassGI,
		e_PassDepth,
		e_PassShadowMask,
		e_PassIllumination,
		e_PassNormals,
		e_PassDirtyMatte,
		e_PassWireframe,
		e_PassMaterials,
		e_PassReflections,
		e_PassBloom,
		e_PassStar,
		e_PassCameraDOF,
		e_PassPreview,
		e_PassGlow
	};
	prtyEnum m_RenderPassVP;

	prtyBoolean m_bToneMap;
//	prtyBoolean m_bBlueShift;
	prtyEnum m_HDRDebugMode;

	//prtyBoolean m_bEnableAmbientPass;
	prtyBoolean m_bEnableLitPass;
	prtyBoolean m_bEnableTransparent;

	prtyBoolean m_bEnableDiffuseLighting;
	prtyBoolean m_bEnableSpecularLighting;

	prtyBoolean	m_bEnableReflection;
	prtyBoolean m_bEnableEnvironment;

	prtyBoolean m_bLowResolution;
	prtyBoolean m_bCaptureToneMapped;
	//prtyBoolean	m_bEnableAO;
	//prtyBoolean m_bRecalcAOPerFrame;
	
	prtyBoolean m_bProjLightFrustumCull;
	prtyBoolean m_bProjLightsOn;
	prtyBoolean m_bPtLightsOn;
	prtyBoolean m_bDoShadowMapGen;
	prtyBoolean m_bEnableShadows;
	prtyBoolean m_bEnableInvisibleCastShadows;
	prtyBoolean m_bEnableInvisibleMaskBlack;
	prtyBoolean m_bEnableInvisibleInReflections;

	//prtyBoolean m_bEnableDeferredTransparency;
	prtyBoolean m_bHDRAA;

	prtyBoolean m_bEnableBeautySSAO;
	prtyBoolean m_bEnableBeautySSGI;

	//	Screen Space Ambient Occlusion
	prtyBoolean m_bEnableSSAO;
	enum eSSAOPreset
	{
		e_SSAOLow, e_SSAOMed, e_SSAOHigh, e_SSAOCustom
	};
	prtyEnum m_SSAOQuality;//preset steps and dirs.
	enum eSSAOPresetVP
	{
		e_SSAOLowVP, e_SSAOMedVP, e_SSAOHighVP, e_SSAOCustomVP
	};
	prtyEnum m_SSAOQualityVP;//preset steps and dirs.
	prtyFloat m_SSAONumSteps;
	prtyInt8 m_SSAONumDirs;
	prtyBoolean m_SSAOEnableBlur;
	prtyBoolean m_bEnableSSAODepthPeeling;
	prtyInt8 m_SSAONumLayers;

	prtyBoolean m_bUseAOVolumes;

	prtyBoolean m_bEnableSSGI;
	enum eSSGIPreset
	{
		e_SSGILow, e_SSGIMed, e_SSGIHigh, e_SSGICustom
	};
	prtyEnum m_SSGIQuality;//preset steps and dirs.
	enum eSSGIPresetVP
	{
		e_SSGILowVP, e_SSGIMedVP, e_SSGIHighVP,e_SSGICustomVP
	};
	prtyEnum m_SSGIQualityVP;//preset steps and dirs.
	prtyFloat m_SSGINumSteps;
	prtyInt8 m_SSGINumDirs;
	prtyBoolean m_SSGIEnableBlur;
	prtyBoolean m_bEnableSSGIDepthPeeling;
	prtyInt8 m_SSGINumLayers;

	prtyBoolean m_bUseLPVGI;
	

	//Hardware Tessellation
	prtyBoolean		m_bUseHardwareTessellation;
	prtyFloat		m_PixelSubdivLimit;

	//Render Wireframe
	prtyBoolean m_bRenderWireframe;

	//---------------------------------------------------------------------------
	//	Motion Blur data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bMotionBlurEnable;
	prtyInt8	m_MotionBlurSamples;
	prtyFloat	m_MotionBlurPercent;

	// Transparency data
	prtyEnum m_TransparencyMode;

	prtyBoolean m_bDebugDepthPeel;
	prtyBoolean m_bDebugSinglePeel;
	prtyInt8	m_nDebugDepthPeelLayers;

	// for illumination renderer
	prtyBoolean m_bIlluminationUsesNormals;

	prtyBoolean m_bLoadingScene;

	//---------------------------------------------------------------------------
	//	Hair data
	//---------------------------------------------------------------------------
	prtyBoolean	m_bEnableHair;
	prtyBoolean	m_bHairLines;
	prtyEnum	m_HairShadowType;
	prtyEnum	m_HairShadowRes;
	prtyFloat	m_HairTessellation;
	prtyFloat	m_HairVertexLimit;
	prtyFloat	m_HairStrandSkip;
	prtyEnum	m_HairTransparencyMode;
	prtyFloat   m_HairSubPixelPower;
	prtyBoolean m_bHairDepthPeel;
	prtyInt8	m_nHairDepthPeelLayers;
	prtyFloat	m_HairInterpolationCount;
	prtyFloat	m_HairClumpRadius;

	//---------------------------------------------------------------------------
	//	Render pass from file data
	//---------------------------------------------------------------------------
	prtyBoolean m_bRPF_AO;
	prtyBoolean m_bRPF_GI;
	prtyBoolean m_bRPF_Refl;
	prtyBoolean m_bRPF_ShadowMask;
	prtyBoolean m_bRPF_Beauty;

	//---------------------------------------------------------------------------
	//	RenderMan data
	//---------------------------------------------------------------------------
	prtyBoolean m_bRmanRenderRIB;
	prtyBoolean m_bRmanGenShadowMaps;
	prtyBoolean m_bRmanGenReflectionMaps;
	prtyFloat	m_RmanShadingRate;
	prtyInt32	m_nRmanAArate;
	prtyInt32	m_RmanAOsamples;	
	prtyEnum	m_RmanOutType;
	prtyEnum	m_RmanBatchContent;
	prtyBoolean m_bRmanTextureBatch;
	prtyBoolean m_bRmanCacheTextures;
	prtyBoolean m_bRmanDisableWarnings;
	prtyBoolean m_bRmanAOEnable;
	prtyFloat	m_RmanAOMaxDist;
	prtyFloat	m_RmanAOMaxVariation;
	prtyFloat	m_RmanAOConeAngle;
	prtyBoolean m_bRmanReflEnable;	
	prtyEnum	m_RmanReflType;
	prtyBoolean m_bRmanShadowEnable;
	prtyEnum	m_RmanShadowType;
	prtyInt32	m_RmanShadowMinSamples;
	prtyInt32	m_RmanShadowSamples;
	prtyFloat	m_RmanShadowBias;
	prtyFloat	m_RmanShadowSoftness;
	prtyBoolean m_bRmanGIEnable;
	prtyInt32	m_RmanGIsamples;
	prtyFloat	m_RmanGIMaxDist;
	prtyFloat	m_RmanGIMaxVariation;
	prtyFloat	m_RmanGIConeAngle;
	prtyEnum	m_RmanFilterType;
	prtyFloat	m_RmanFilterWidth;
	prtyInt32	m_RmanNumCores;
	prtyInt32	m_RmanTexMemory;
	prtyEnum	m_RmanBucketOrder;
	prtyInt32	m_RmanBucketSize;
	prtyInt32	m_RmanGridSize;
	prtyInt32	m_RmanRayDepth;
	prtyBoolean m_bRmanTonemapEnable;

	//---------------------------------------------------------------------------
	//	MentalRay data
	//---------------------------------------------------------------------------
	prtyInt32	m_MrayVerbosity;
	prtyInt32	m_MrayNumReflBounces;
	prtyInt32	m_MrayNumRefrBounces;
	prtyInt32	m_MrayMaxTraceDepth;
	prtyBoolean	m_bMrayAO;
	prtyInt32	m_MrayAOSamples;
	prtyBoolean	m_bMrayFinalGather;
	prtyBoolean	m_bMrayFGBlur;
	prtyInt32	m_MrayFGNDiffuse;
	prtyInt32	m_MrayFGNRefl;
	prtyInt32	m_MrayFGNRefr;
	prtyInt32	m_MrayFGNRays;
	prtyColor	m_MrayFGColor;
	prtyEnum	m_MrayOutputFormat;
	prtyInt32	m_MrayNumThreads;
	prtyInt32	m_MrayMemoryLimit;
	prtyBoolean	m_bMrayEnableReflections;
	prtyBoolean	m_bMrayEnableShadows;
	prtyEnum	m_MrayShadowType;
	prtyBoolean	m_bMrayRewriteAssets;
	prtyEnum	m_MrayVerbosityLevel;
	prtyBoolean	m_bMrayFGMapEnable;
	prtyEnum	m_MrayFGMapRebuild;
	prtyFilePath m_MrayFGMapPath;
	prtyInt32	m_MrayReflSamples;
	prtyBoolean	m_bMrayOverrideMSPSampling;
	prtyInt32	m_MrayMinCaptureSamples;
	prtyInt32	m_MrayMaxCaptureSamples;
	prtyFloat	m_MRayAAContrast;
	prtyBoolean	m_bMrayIgnoreBadTex;
	prtyBoolean	m_bMrayDisplayPreview;
	prtyBoolean m_bMRayTonemapEnable;
	prtyBoolean m_bEnableIBL;
	prtyFloat	m_IBLQuality;
	prtyEnum	m_IBLMapRes;
	prtyFloat	m_IBLScale;
	prtyInt32	m_IBLSampleNum;
	prtyBoolean	m_bMrayProgressive;
	prtyInt32	m_MrayProgSubsamplingSize;
	prtyEnum	m_MrayProgSubsamplingMode;
	prtyEnum	m_MrayProgSubsamplingPattern;
	prtyInt32	m_MrayProgMinSamples;
	prtyInt32	m_MrayProgMaxSamples;
	prtyInt32	m_MrayProgMaxTime;
	prtyFloat	m_MrayProgErrorThreshold;

};
