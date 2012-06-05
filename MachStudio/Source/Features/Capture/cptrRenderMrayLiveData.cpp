//****************************************************************************
//	cptrRenderMrayLiveData.hpp
//
//	Preferences Data
//
//	StudioGPU
//	Copyright(c) 2004-7 - All Rights Reserved
//****************************************************************************
#include "Features/Capture/cptrRenderMrayLiveData.hpp"
#include "Features/ObjectManip/mnpModeObjectManip.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/Env/envThreadGroup.hpp"
#include "Core/prty/prtyUnits.hpp"
#include "Tool/icn/icnIconScale.hpp"


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
cptrRenderMrayLiveData::cptrRenderMrayLiveData()
:	
	// Mental Ray
	m_bMrayAO("Mray Live Enable AO", false),
	m_MrayAOSamples("Mray Live Num AO Samples", 64),
	m_bMrayFinalGather("Enable", false),
	//m_bMrayFGBlur("Mray Live Enable Blur", true),
	m_MrayFGNDiffuse("Mray Live Num Diffuse Bounces", 0),
	m_MrayFGNRefl("Mray Live Num Reflection Bounces", 1),
	m_MrayFGNRefr("Mray Live Num Refraction Bounces", 1),
	m_MrayFGNRays("Mray Live Num Rays", 100),
	m_MrayFGColor("Mray Live Color Tint", maFloatRGBA(0.2f, 0.2f, 0.2f, 1)),
	m_MrayNumReflBounces("Mray Live Num Reflection Bounces", 2),
	m_MrayNumRefrBounces("Mray Live Num Refraction Bounces", 2),
	m_MrayMaxTraceDepth("Mray Live Max Trace Depth", 4),
	m_MrayVerbosity("Mray Live Verbosity Level", 3),
	m_MrayOutputFormat("Mray Live Output Format", 0),
	m_MrayNumThreads("Mray Live Num Render Threads", 4),
	m_MrayMemoryLimit("Mray Live Memory Limit (MB)", 2048),
	m_bMrayEnableReflections("Mray Live Enable Reflections", true),
	m_bMrayEnableShadows("Mray Live Enable Shadows", true),
	m_MrayShadowType("Mray Live Shadow Type", 0 ),
	m_bMrayRewriteAssets("Mray Live Rewrite All Assets", false),
	m_MrayVerbosityLevel("Mray Live Verbosity Level", 4),
	m_bMrayFGMapEnable("Mray Live Enable Map",false),
	m_MrayFGMapRebuild("Mray Live Map Rebuilding", 0),
	m_MrayFGMapPath("Mray Live Map Location"),
	m_MrayReflSamples("Mray Live Num Glossy Samples", 8),
	m_bMrayIgnoreBadTex("Mray Live Ignore Unsupported Textures", true),
	m_bMrayDisplayPreview("Mray Live Display Render Result", true),
	m_bMrayOverrideMSPSampling("Mray Live Override MSP Sampling", true),
	m_MrayMinCaptureSamples("Mray Live Min Samples", 0),
	m_MrayMaxCaptureSamples("Mray Live Max Samples", 2),
	m_MRayAAContrast("Mray Live Antialiasing Contrast",0.1f),
	m_bMRayTonemapEnable("Mray Live Capture Tonemapped Pixels",true),
	m_bEnableIBL("Mray Live Enable IBL", false),
	m_IBLQuality("Mray Live IBL Quality", 0.2f),
	m_IBLMapRes("Mray Live IBL Resolution", 0),
	m_IBLScale("Mray Live IBL Scale", 1.0f),
	m_IBLSampleNum("Mray Live Num IBL Samples", 2),
	m_bMrayProgressive("Mray Live Enable Progressive", false),
	m_MrayProgSubsamplingSize("Mray Live Progressive Subsampling Size", 1),
	m_MrayProgSubsamplingMode("Mray Live Progressive Subsampling Mode", 1),
	m_MrayProgSubsamplingPattern("Mray Live Progressive Subsampling Pattern", 1),
	m_MrayProgMinSamples("Mray Live Progressive Min Samples", 4),
	m_MrayProgMaxSamples("Mray Live Progressive Max Samples", 100),
	m_MrayProgMaxTime("Mray Live Progressive Max Subsampling Time (secs)", 0),
	m_MrayProgErrorThreshold("Mray Live Progressive Error Threshold", 0.05f),
	m_MrayRenderPass("Mray Live Render Pass",0),
	m_bStartStop("Mray Live Render"),
	m_MrayPixelFilterSize("Mray Live Pixel Filter Size",1),
	m_MrayPixelFilterType("Mray Live Pixel Filter Type",0),
	m_CompositeRefl("Mray Live Render Beauty"),
	m_CompositeAO("Mray Live Render Ambient Occlusion"),
	m_CompositeFG("Mray Live Render Final Gather"),
	m_CompositeBeauty("Mray Live Render Reflections")
{
	m_MrayPixelFilterType.SetEnumTag(0,"Box");
	m_MrayPixelFilterType.SetEnumTag(1,"Gauss");
	m_MrayPixelFilterType.SetEnumTag(2,"Mitchell");
	m_MrayPixelFilterType.SetEnumTag(3,"Triangle");
	m_MrayPixelFilterType.SetEnumTag(4,"Lanczos");

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


	m_MrayRenderPass.SetEnumTag(0, "Beauty");
	m_MrayRenderPass.SetEnumTag(1, "Diffuse");
	m_MrayRenderPass.SetEnumTag(2, "Diffuse Environment");
	m_MrayRenderPass.SetEnumTag(3, "Diffuse Lights");
	m_MrayRenderPass.SetEnumTag(4, "Specular");
	m_MrayRenderPass.SetEnumTag(5, "Specular Environment");
	m_MrayRenderPass.SetEnumTag(6, "Specular Lights");
	m_MrayRenderPass.SetEnumTag(7, "Emissive");
	m_MrayRenderPass.SetEnumTag(8, "Ambient Occlusion");
	m_MrayRenderPass.SetEnumTag(9, "Shadow Mask");
	m_MrayRenderPass.SetEnumTag(10, "Illumination");
	m_MrayRenderPass.SetEnumTag(11, "Normals");
	m_MrayRenderPass.SetEnumTag(12, "Reflections");
	m_MrayRenderPass.SetEnumTag(13, "Final Gather");
	
	m_IBLMapRes.SetEnumTag(0, "512");
	m_IBLMapRes.SetEnumTag(1, "1024");
	m_IBLMapRes.SetEnumTag(2, "2048");
	m_IBLMapRes.SetEnumTag(3, "4096");
	m_IBLMapRes.SetEnumTag(4, "8192");
}

