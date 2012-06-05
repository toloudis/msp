/****************************************************************************\
**	rprfPrefsData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/rprf/rprfPrefsData.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
rprfPrefsData::rprfPrefsData()
:	m_bMultipassOn("Multipass Lighting",true),
	m_bEnableDOF("Enable DOF",true),
	m_bEnableGlow("Enable Glow",true),
	m_bEnableOutline("Enable Outline",true),
	//m_bEnableAmbientPass("Render Ambient",true),
	m_bEnableLitPass("Enable Lights",true),
	m_bEnableTransparent("Enable Transparent",true),
	m_bEnableDiffuseLighting("Enable Diffuse", true),
	m_bEnableSpecularLighting("Enable Specular", true),
	m_bMatteMode("Render Alpha",false),
	//m_bHeadlightOn("Headlight On",false),
	m_RendererType("Render Type", 0),
	m_RendererTypeVP("Render Type Viewport", 0),
	m_RenderPassVP("Pass Viewport", 0),
	m_RendererEngine("Render Engine", 0),
	//m_bBlueShift("Blue Shift", false),
	m_bToneMap("Tone Map", true),
	m_bCaptureToneMapped("Capture Tonemapped Pixels", false),
	m_HDRDebugMode("HDR Layer", 0),
	m_bEnableReflection("Enable Reflections", true),
	m_bEnableEnvironment("Enable Environments", true),
	m_bLowResolution("Low Resolution", false),
	//m_bEnableAO("Ambient Occlusion", false),
	//m_bRecalcAOPerFrame("Recalc Per Frame", false),
	m_bProjLightFrustumCull("Projected Light Frustum Cull", true),
	m_bProjLightsOn("Projected Lights On", true),
	m_bPtLightsOn("Point Lights On", true),
	m_bDoShadowMapGen("Shadow Map Gen Pass", true),
	m_bEnableShadows("Enable Shadows", true),
	m_bEnableInvisibleCastShadows("Enable Invisible Objects Cast Shadows", true),
	m_bEnableInvisibleMaskBlack("Enable Invisible Objects Mask", true),
	m_bEnableInvisibleInReflections("Enable Invisible In Reflections", true),
	//m_bEnableDeferredTransparency("Draw Particles Last", false),
	m_bHDRAA("Enable Antialiasing", true),

	m_bEnableSSAO("Enable AO", false),
	m_SSAOQuality("AO Sampling Preset", e_SSAOCustom),  
	m_SSAOQualityVP("AO Sampling Preset Viewport", e_SSAOCustomVP),
	m_SSAONumSteps("AO Num Steps", 8),
	m_SSAONumDirs("AO Num Dirs", 16),
	m_SSAOEnableBlur("AO Enable Blur", false),
	m_bEnableSSAODepthPeeling("AO Enable Multiple Depths",false),
	m_SSAONumLayers("AO Num Depth Layers",1),	
	m_bEnableBeautySSAO("Enable Beauty AO", true),

	m_bEnableSSGI("Enable GI", false),
	m_SSGIQuality("GI Sampling Preset", e_SSGICustom),  
	m_SSGIQualityVP("GI Sampling Preset Viewport", e_SSGICustomVP),
	m_SSGINumSteps("GI Num Steps", 8),
	m_SSGINumDirs("GI Num Dirs", 16),
	m_SSGIEnableBlur("GI Enable Blur", false),
	m_bEnableSSGIDepthPeeling("GI Enable Multiple Depths",false),
	m_SSGINumLayers("GI Num Depth Layers",1),
	m_bEnableBeautySSGI("Enable Beauty GI", true),

	m_bUseHardwareTessellation("Hardware Tessellation", false),
	m_PixelSubdivLimit("Tesselation Limit", 4.0f ),
	m_bRenderWireframe("Wireframe", false ),
	m_bMotionBlurEnable("Enable Motion Blur",false),
	m_MotionBlurSamples("Motion Blur Samples", 20 ),
	m_MotionBlurPercent("Motion Blur Percent", 100.0f ),
	m_TransparencyMode("Transparency Mode",0),
	m_bDebugDepthPeel("Enable Depth Limiting", false ),
	m_bDebugSinglePeel("Enable Debug Single Transparency Depth Peel", false ),
	m_nDebugDepthPeelLayers( "Depth Layers", 0 ),
	m_bIlluminationUsesNormals("Use Normals", true),
	m_bEnableHair("Enable Hair",false),
	m_bHairLines("Hair Lines",false),
	m_HairTransparencyMode("Hair Transparency",0),
	m_HairShadowRes("Hair Shadow Res", 0 ),
//	m_HairShadowType("Shadow Type", HAIR_SHADOW_OSM4 ),
	m_HairTessellation("Hair Tessellation", 1.0f ),
	m_HairVertexLimit("Hair Vertex Limit (K)", 50 ),
	m_HairStrandSkip("Skip Strands", 0 ),
	m_HairSubPixelPower("Hair Sub-Pixel Power", 1.0f ),
	m_HairInterpolationCount("Hair Interpolation", 1 ),
	m_HairClumpRadius("Hair Clump Radius", 0.0f ),
	m_bLoadingScene("Loading Scene", false),
	m_bHairDepthPeel("Hair Enable Depth Limiting", false ),
	m_nHairDepthPeelLayers( "Hair Depth Layers", 0 ),

	// Render Pass from File
	m_bRPF_AO("Use AO Driver", true),
	m_bRPF_GI("Use GI Driver", true),
	m_bRPF_Refl("Use Reflections Driver", true),
	m_bRPF_ShadowMask("Use Shadow Mask Driver", true),
	m_bRPF_Beauty("Use Beauty Driver", true),

	// Renderman
	m_bRmanRenderRIB("Renderman Launch Renderer Each Frame", false),
	m_bRmanGenShadowMaps("Renderman Generate Shadow Maps", false),
	m_bRmanGenReflectionMaps("Renderman Generate Reflection Maps", false),
	m_nRmanAArate("Renderman Sampling Rate",4),
	m_RmanShadingRate("Renderman Shading Rate",0.25f),
	m_bRmanTextureBatch("Renderman Launch Batch File", true),
	m_RmanOutType("Renderman Output Format", 0 ),
	m_bRmanCacheTextures("Renderman Rewrite All Assets", false ),
	m_bRmanDisableWarnings("Renderman Disable Warnings", false ),
	m_RmanBatchContent("Renderman Batch File Action", 2 ),
	m_bRmanAOEnable("Renderman Enable AO",false),
	m_RmanAOsamples("Renderman Num AO Samples",64),
	m_RmanAOMaxDist("Renderman AO Occluder Distance", 100 ),
	m_RmanAOMaxVariation("Renderman AO Quality", 50 ),
	m_RmanAOConeAngle("Renderman Cone Angle", 90 ),
	m_bRmanReflEnable("Renderman Reflection Enable", true ),
	m_RmanReflType("Renderman Reflection Type", 0 ),
	m_bRmanShadowEnable("Renderman Enable Shadows", true ),
	m_RmanShadowType("Renderman Shadow Type", 0 ),
	m_RmanShadowMinSamples("Renderman Min Samples", 1 ),
	m_RmanShadowSamples("Renderman Num Samples", 8 ),
	m_RmanShadowBias("Renderman Global Bias", 100 ),
	m_RmanShadowSoftness("Renderman Softness", 0.01f ),
	m_bRmanGIEnable("Renderman Enable GI",false),
	m_RmanGIsamples("Renderman Num GI Samples",64),
	m_RmanGIMaxDist("Renderman GI Surface Distance", 100 ),
	m_RmanGIMaxVariation("Renderman GI Bleed Quality", 50 ),
	m_RmanGIConeAngle("Renderman GI Cone Angle", 90 ),
	m_RmanFilterType("Renderman Filter Type", 0 ),
	m_RmanFilterWidth("Renderman Filter Width", 1 ),
	m_RmanNumCores("Renderman Num Render Threads", 4 ),
	m_RmanTexMemory("Renderman Available Texture Memory (MB)", 2048 ),
	m_RmanBucketOrder("Renderman Bucket Order", 0 ),
	m_RmanBucketSize("Renderman Bucket Size", 16 ),
	m_RmanGridSize("Renderman Grid Size", 256 ),
	m_RmanRayDepth("Renderman Ray Tracing Depth", 10 ),
	m_bRmanTonemapEnable("Renderman Capture Tonemapped Pixels",true),

	// Mental Ray
	m_bMrayAO("mental ray Enable AO", false),
	m_MrayAOSamples("mental ray Num AO Samples", 64),
	m_bMrayFinalGather("mental ray Enable Final Gather", false),
	//m_bMrayFGBlur("Enable Blur", true),
	m_MrayFGNDiffuse("mental ray Num Diffuse Bounces", 0),
	m_MrayFGNRefl("mental ray Num Reflection Bounces", 1),
	m_MrayFGNRefr("mental ray Num Refraction Bounces", 1),
	m_MrayFGNRays("mental ray Num Rays", 100),
	m_MrayFGColor("mental ray Color Tint", maFloatRGBA(0.2f, 0.2f, 0.2f, 1)),
	m_MrayNumReflBounces("mental ray Num Reflection Bounces", 2),
	m_MrayNumRefrBounces("mental ray Num Refraction Bounces", 2),
	m_MrayMaxTraceDepth("mental ray Max Trace Depth", 4),
	m_MrayVerbosity("mental ray Verbosity Level", 3),
	m_MrayOutputFormat("mental ray Output Format", 0),
	m_MrayNumThreads("mental ray Num Render Threads", 4),
	m_MrayMemoryLimit("mental ray Memory Limit (MB)", 2048),
	m_bMrayEnableReflections("mental ray Enable Reflections", true),
	m_bMrayEnableShadows("mental ray Enable Shadows", true),
	m_MrayShadowType("mental ray Shadow Type", 0 ),
	m_bMrayRewriteAssets("mental ray Rewrite All Assets", false),
	m_MrayVerbosityLevel("mental ray Verbosity Level", 4),
	m_bMrayFGMapEnable("mental ray Enable Map",false),
	m_MrayFGMapRebuild("mental ray Map Rebuilding", 0),
	m_MrayFGMapPath("mental ray Map Location"),
	m_MrayReflSamples("mental ray Num Glossy Samples", 8),
	m_bMrayIgnoreBadTex("mental ray Ignore Unsupported Textures", true),
	m_bMrayDisplayPreview("mental ray Display Render Result", true),
	m_bMrayOverrideMSPSampling("mental ray Override MSP Sampling", true),
	m_MrayMinCaptureSamples("mental ray Min Samples", 0),
	m_MrayMaxCaptureSamples("mental ray Max Samples", 2),
	m_MRayAAContrast("mental ray Antialiasing Contrast",0.1f),
	m_bMRayTonemapEnable("mental ray Capture Tonemapped Pixels",true),
	m_bEnableIBL("Enable IBL", false),
	m_IBLQuality("IBL Quality", 0.2f),
	m_IBLMapRes("IBL Resolution", 0),
	m_IBLScale("IBL Scale", 1.0f),
	m_IBLSampleNum("Num IBL Samples", 2),
	m_bMrayProgressive("mental ray Enable", false),
	m_MrayProgSubsamplingSize("mental ray Subsampling Size", 1),
	m_MrayProgSubsamplingMode("mental ray Subsampling Mode", 1),
	m_MrayProgSubsamplingPattern("mental ray Subsampling Pattern", 1),
	m_MrayProgMinSamples("mental ray Min Prog Samples", 4),
	m_MrayProgMaxSamples("mental ray Max Prog Samples", 100),
	m_MrayProgMaxTime("mental ray Max Subsampling Time (secs)", 0),
	m_MrayProgErrorThreshold("mental ray Error Threshold", 0.05f),

	m_bUseAOVolumes("Use AO Volumes", false),
	m_bUseLPVGI("Use LPV GI", false)
{
	m_RendererEngine.SetEnumTag(0, "MachStudio Rasterizer");
	m_RendererEngine.SetEnumTag(1, "RenderMan (PRMan)");

//#ifdef _DEBUG
	m_RendererEngine.SetEnumTag(2, "mental ray");
//#endif

	m_MrayShadowType.SetEnumTag(0,"Shadow map based");
	m_MrayShadowType.SetEnumTag(1,"Ray traced");

	m_MrayVerbosityLevel.SetEnumTag(0, "Fatal Errors");
	m_MrayVerbosityLevel.SetEnumTag(1, "Errors");
	m_MrayVerbosityLevel.SetEnumTag(2, "Warnings");
	m_MrayVerbosityLevel.SetEnumTag(3, "Informational Messages");
	m_MrayVerbosityLevel.SetEnumTag(4, "Progress Reports");
	m_MrayVerbosityLevel.SetEnumTag(5, "Debug Messages");
	m_MrayVerbosityLevel.SetEnumTag(6, "Verbose Debug Messages");

	m_MrayFGMapRebuild.SetEnumTag(0, "Off");
	m_MrayFGMapRebuild.SetEnumTag(1, "On");
	m_MrayFGMapRebuild.SetEnumTag(2, "Freeze");

	m_MrayOutputFormat.SetEnumTag(0, "BMP");
	m_MrayOutputFormat.SetEnumTag(1, "JPG");
	m_MrayOutputFormat.SetEnumTag(2, "TGA");
	m_MrayOutputFormat.SetEnumTag(3, "PNG");
	m_MrayOutputFormat.SetEnumTag(4, "PPM");
	m_MrayOutputFormat.SetEnumTag(5, "HDR");
	m_MrayOutputFormat.SetEnumTag(6, "TIFF");
	m_MrayOutputFormat.SetEnumTag(7, "TIFU");
	m_MrayOutputFormat.SetEnumTag(8, "OpenEXR");
	m_MrayOutputFormat.SetEnumTag(9, "Softimage");
	m_MrayOutputFormat.SetEnumTag(10, "Alias");
	m_MrayOutputFormat.SetEnumTag(11, "SGIF");
	m_MrayOutputFormat.SetEnumTag(12, "MayaIFF");
	m_MrayOutputFormat.SetEnumTag(13, "RLA");
	m_MrayOutputFormat.SetEnumTag(14, "RLB");
	m_MrayOutputFormat.SetEnumTag(15, "CatiaPicture");

	m_MrayProgSubsamplingMode.SetEnumTag(0, "Sparse");
	m_MrayProgSubsamplingMode.SetEnumTag(1, "Detail");

	m_MrayProgSubsamplingPattern.SetEnumTag(0, "Linear");
	m_MrayProgSubsamplingPattern.SetEnumTag(1, "Scatter");

	m_IBLMapRes.SetEnumTag(0, "512");
	m_IBLMapRes.SetEnumTag(1, "1024");
	m_IBLMapRes.SetEnumTag(2, "2048");
	m_IBLMapRes.SetEnumTag(3, "4096");
	m_IBLMapRes.SetEnumTag(4, "8192");


	m_RmanOutType.SetEnumTag(0, "Display Window");
	m_RmanOutType.SetEnumTag(1, "TIFF");
	m_RmanOutType.SetEnumTag(2, "Targa");
	m_RmanOutType.SetEnumTag(3, "SGIF");
	m_RmanOutType.SetEnumTag(4, "Cineon");
	m_RmanOutType.SetEnumTag(5, "Softimage");
	m_RmanOutType.SetEnumTag(6, "MayaIFF");
	m_RmanOutType.SetEnumTag(7, "OpenEXR");
	m_RmanOutType.SetEnumTag(8, "Alias");
	m_RmanBatchContent.SetEnumTag(0, "Create Assets");
	m_RmanBatchContent.SetEnumTag(1, "Launch Renderer");
	m_RmanBatchContent.SetEnumTag(2, "Create Assets + Launch Renderer");

	m_RmanReflType.SetEnumTag(0,"Reflection map based");
	m_RmanReflType.SetEnumTag(1,"Ray traced");

	m_RmanShadowType.SetEnumTag(0,"Shadow map based");
	m_RmanShadowType.SetEnumTag(1,"Ray traced");

	m_RmanFilterType.SetEnumTag(0,"Box");
	m_RmanFilterType.SetEnumTag(1,"Gaussian");
	m_RmanFilterType.SetEnumTag(2,"Mitchell");
	m_RmanFilterType.SetEnumTag(3,"Triangle");
	m_RmanFilterType.SetEnumTag(4,"Sinc");
	m_RmanFilterType.SetEnumTag(5,"Blackman-Harris");
	m_RmanFilterType.SetEnumTag(6,"Catmull-Rom");
	m_RmanFilterType.SetEnumTag(7,"Separable-Catmull-Rom");

	m_RmanBucketOrder.SetEnumTag(0,"Space Fill");
	m_RmanBucketOrder.SetEnumTag(1,"Horizontal");
	m_RmanBucketOrder.SetEnumTag(2,"Vertical");
	m_RmanBucketOrder.SetEnumTag(3,"Zigzag - X");
	m_RmanBucketOrder.SetEnumTag(4,"Zigzag - Y");
	m_RmanBucketOrder.SetEnumTag(5,"Spiral");
	m_RmanBucketOrder.SetEnumTag(6,"Random");

#if(SGPU_APP != MS_CORE)
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
#else
	m_HDRDebugMode.SetEnumTag(0, "Full Render");
	m_HDRDebugMode.SetEnumTag(1, "[N/A] Clamped HDR Buffer");
	m_HDRDebugMode.SetEnumTag(2, "[N/A] Scaled HDR Buffer");
	m_HDRDebugMode.SetEnumTag(3, "[N/A] Pixel Luminances");
	m_HDRDebugMode.SetEnumTag(4, "[N/A] DOF Blurriness");
	m_HDRDebugMode.SetEnumTag(5, "[N/A] 1st Luminance pass");
	m_HDRDebugMode.SetEnumTag(6, "[N/A] Bright pass");
	m_HDRDebugMode.SetEnumTag(7, "[N/A] Bloom source");
	m_HDRDebugMode.SetEnumTag(8, "[N/A] Bloom");
	m_HDRDebugMode.SetEnumTag(9, "[N/A] Star");
#endif
#if(SGPU_APP != MS_CORE)
	// any changes to this must be echoed in the rprfPrefsDataParser
	m_RendererType.SetEnumTag(0, "Default HDR");
	m_RendererType.SetEnumTag(1, "AO Only");
	m_RendererType.SetEnumTag(2, "Depth Buffer");
	m_RendererType.SetEnumTag(3, "Shadow Mask");	
	m_RendererType.SetEnumTag(4, "Illumination Only");
	m_RendererType.SetEnumTag(5, "Normals");	
	m_RendererType.SetEnumTag(6, "Dirty Matte");	
	m_RendererType.SetEnumTag(7, "Wireframe");	
	m_RendererType.SetEnumTag(8, "Materials");
	m_RendererType.SetEnumTag(9, "Reflections Only");
	m_RendererType.SetEnumTag(10, "GI Only");
	m_RendererType.SetEnumTag(11, "Velocity Map");

	m_RendererTypeVP.SetEnumTag(0, "Default HDR");
	m_RendererTypeVP.SetEnumTag(1, "AO Only");
	m_RendererTypeVP.SetEnumTag(2, "Depth Buffer");
	m_RendererTypeVP.SetEnumTag(3, "Shadow Mask");
	m_RendererTypeVP.SetEnumTag(4, "Illumination Only");
	m_RendererTypeVP.SetEnumTag(5, "Normals");
	m_RendererTypeVP.SetEnumTag(6, "Dirty Matte");
	m_RendererTypeVP.SetEnumTag(7, "Wireframe");
	m_RendererTypeVP.SetEnumTag(8, "Materials");
	m_RendererTypeVP.SetEnumTag(9, "Reflections Only");
	m_RendererTypeVP.SetEnumTag(10, "GI Only");
#ifdef ENABLE_MOTIONBLUR
	m_RendererTypeVP.SetEnumTag(11, "Velocity Map");
#endif
#else
	//**MS_CORE
	//	1) any changes to this must be echoed in the rprfPrefsDataParser
	//	2) also set the enabled true/false in rprfPresfUtil::RenderTypeEnabled()
	m_RendererType.SetEnumTag(0, "Default HDR");
	m_RendererType.SetEnumTag(1, "[N/A] AO Only");
	m_RendererType.SetEnumTag(2, "[N/A] Depth Buffer");
	m_RendererType.SetEnumTag(3, "[N/A] Shadow Mask");
	m_RendererType.SetEnumTag(4, "[N/A] Illumination Only");
	m_RendererType.SetEnumTag(5, "[N/A] Normals");
	m_RendererType.SetEnumTag(6, "[N/A] Dirty Matte");
	m_RendererType.SetEnumTag(7, "[N/A] Wireframe");
	m_RendererType.SetEnumTag(8, "[N/A] Materials");
	m_RendererType.SetEnumTag(9, "[N/A] Reflections Only");
	m_RendererType.SetEnumTag(10, "[N/A] GI Only");
	m_RendererType.SetEnumTag(11, "[N/A] Velocity Map");

	m_RendererTypeVP.SetEnumTag(0, "Default HDR");
	m_RendererTypeVP.SetEnumTag(1, "[N/A] AO Only");
	m_RendererTypeVP.SetEnumTag(2, "[N/A] Depth Buffer");
	m_RendererTypeVP.SetEnumTag(3, "[N/A] Shadow Mask");
	m_RendererTypeVP.SetEnumTag(4, "[N/A] Illumination Only");
	m_RendererTypeVP.SetEnumTag(5, "[N/A] Normals");
	m_RendererTypeVP.SetEnumTag(6, "[N/A] Dirty Matte");
	m_RendererTypeVP.SetEnumTag(7, "[N/A] Wireframe");
	m_RendererTypeVP.SetEnumTag(8, "[N/A] Materials");
	m_RendererTypeVP.SetEnumTag(9, "[N/A] Reflections Only");
	m_RendererTypeVP.SetEnumTag(10, "[N/A] GI Only");
#ifdef ENABLE_MOTIONBLUR
	m_RendererTypeVP.SetEnumTag(11, "[N/A] Velocity Map");
#endif
#endif	// #else of #if(SGPU_APP != MS_CORE)

	// which renderpass to show (viewport)
	m_RenderPassVP.SetEnumTag(e_PassBeauty, "Beauty");
	m_RenderPassVP.SetEnumTag(e_PassDiffuse, "Diffuse");
	m_RenderPassVP.SetEnumTag(e_PassDiffEnv, "Diffuse Environment");
	m_RenderPassVP.SetEnumTag(e_PassDiffLit, "Diffuse Lights");
	m_RenderPassVP.SetEnumTag(e_PassSpecular, "Specular");
	m_RenderPassVP.SetEnumTag(e_PassSpecEnv, "Specular Environment");
	m_RenderPassVP.SetEnumTag(e_PassSpecLit, "Specular Lights");
	m_RenderPassVP.SetEnumTag(e_PassEmissive, "Emissive");
	m_RenderPassVP.SetEnumTag(e_PassAO, "AO");
	m_RenderPassVP.SetEnumTag(e_PassGI, "GI");
	m_RenderPassVP.SetEnumTag(e_PassDepth, "Depth");
	m_RenderPassVP.SetEnumTag(e_PassShadowMask, "Shadow Mask");
	m_RenderPassVP.SetEnumTag(e_PassIllumination, "Illumination");
	m_RenderPassVP.SetEnumTag(e_PassNormals, "Normals");
	m_RenderPassVP.SetEnumTag(e_PassDirtyMatte, "Dirty Matte");
	m_RenderPassVP.SetEnumTag(e_PassWireframe, "Wireframe");
	m_RenderPassVP.SetEnumTag(e_PassMaterials, "Materials");
	m_RenderPassVP.SetEnumTag(e_PassReflections, "Reflections");
	m_RenderPassVP.SetEnumTag(e_PassBloom, "Bloom");
	m_RenderPassVP.SetEnumTag(e_PassStar, "Star");
	m_RenderPassVP.SetEnumTag(e_PassCameraDOF, "Camera DOF");
	m_RenderPassVP.SetEnumTag(e_PassPreview, "Preview");
	m_RenderPassVP.SetEnumTag(e_PassGlow, "Glow");

	m_SSAOQuality.SetEnumTag(e_SSAOLow, "Low");
	m_SSAOQuality.SetEnumTag(e_SSAOMed, "Medium");
	m_SSAOQuality.SetEnumTag(e_SSAOHigh, "High");
	m_SSAOQuality.SetEnumTag(e_SSAOCustom, "Custom");

	m_SSAOQualityVP.SetEnumTag(e_SSAOLowVP, "Low");
	m_SSAOQualityVP.SetEnumTag(e_SSAOMedVP, "Medium");	
	m_SSAOQualityVP.SetEnumTag(e_SSAOHighVP, "High");	
	m_SSAOQualityVP.SetEnumTag(e_SSAOCustomVP, "Custom");

	m_SSGIQuality.SetEnumTag(e_SSGILow, "Low");
	m_SSGIQuality.SetEnumTag(e_SSGIMed, "Medium");
	m_SSGIQuality.SetEnumTag(e_SSGIHigh, "High");
	m_SSGIQuality.SetEnumTag(e_SSGICustom, "Custom");

	m_SSGIQualityVP.SetEnumTag(e_SSGILowVP, "Low");
	m_SSGIQualityVP.SetEnumTag(e_SSGIMedVP, "Medium");	
	m_SSGIQualityVP.SetEnumTag(e_SSGIHighVP, "High");	
	m_SSGIQualityVP.SetEnumTag(e_SSGICustomVP, "Custom");

	m_TransparencyMode.SetEnumTag( 0, "Approximate");	//Object sorting
	m_TransparencyMode.SetEnumTag( 1, "Full");			//Depth peeling
//	m_TransparencyMode.SetEnumTag( 2, "Full (Reversed)");	//Depth peeling-Reversed (Front to Back)

	//hair controls
//	m_HairShadowType.SetEnumTag( HAIR_SHADOW_OSM4, "Opacity Shadow Map (4 layers)");
//	m_HairShadowType.SetEnumTag( HAIR_OSM16, "Opacity Shadow Map (16 layers)");
//	m_HairShadowType.SetEnumTag( HAIR_DOSM, "Deep Opacity Shadow Map (4 layers)");

	//res from 64 to 4096
	char number[12];
	for( int i = 0; i < 7; i++ )
	{
		_itoa_s( 1<<(i+6), number, 12, 10 );
		m_HairShadowRes.SetEnumTag( i, number );
	}

	m_HairTransparencyMode.SetEnumTag( 0, "Solid");			//No Sorting Solid
	m_HairTransparencyMode.SetEnumTag( 1, "Approximate");	//Object sorting
	m_HairTransparencyMode.SetEnumTag( 2, "Full");			//Depth peeling
}

