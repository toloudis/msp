/*****************************************************************************
**	captRenderOutputData.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Support/capt/captRenderOutputData.hpp"

//---------------------------------------------------------------------------
// Constants for initial value
//---------------------------------------------------------------------------
// TIME - keeping this -1 in seconds to match earlier usage, but could be -1 in time units also
const maTime captRenderOutputData::c_InitRenderPosTime = maTime::FromSeconds(-1);
const maTime captRenderOutputData::c_InitEndTime = maTime::FromSeconds(-1);

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
captRenderOutputData::captRenderOutputData()
:	m_bChunkData("Data was read from .mab file"),
	m_fCaptureFPS("Capture FPS"),
	m_nMotionSamplesPerFrame("Motion Samples Per Frame", 1),
	m_nCaptureSampling("Capture Sampling"),
	m_bJitteredSampling("Jittered Sampling", true),
	m_PixelAspect("Pixel Aspect Ratio"),
	m_FilterWidth("Filter Width"),
	m_FilterFunc("Filter Type"),
	m_nCaptureMax("Capture Max"),
	m_OutputDirectory("Output Directory"),
	m_OutputDirectoryRoot("Output Directory Root"),
	m_SoundFile("Sound File"),
	m_Prefix("Prefix"),
	m_bCaptureMovie("Capture Movie"),
	m_CaptureFormat("Capture Format"),
	m_CompressCode("Compress Code"),
	m_nWidth("Width"),
	m_nHeight("Height"),
	m_ShadowQuality("Global Shadow Quality", 0),
	m_bUseSceneFilenameInFilename("Use Scene Filename in Filename"),
	m_bUseCameraNameInFilename("Use Camera Name in Filename"),
	m_bUseLayerNameInFilename("Use Render Layer Name in Filename"),
	m_bUseCompressionInFilename("Use Compression Type in Filename"),
	m_bUseRenderPassInFilename("Use Render Pass in Filename", true),
	m_bUseSceneFilenameAsDirectory("Use Scene Filename as Directory"),
	m_bUseCameraNameAsDirectory("Use Camera Name as Directory"),
	m_bUseLayerNameAsDirectory("Use Render Layer Name as Directory"),
	m_bUseRenderPassAsDirectory("Use Render Pass Name as Directory", false),
	m_bUseResolutionAsDirectory("Use Resolution as Directory"),
	m_bUseRenderDriversAsUniqueCameras("Treat Capture Drivers as Unique Cameras"),
	m_bCaptureAllCameras("Capture all Cameras"),
	m_bDisplayTimeCode("Display Time Code"),
	m_bOutputTitleCard("Output Title Card"),
	m_bKeepFrameOpenAfterRender("Keep Frame Open After Render"),
	m_bShowRenderProgressDialog("Show Render Progress Dialog"),
	m_fLeadIn("Lead-In (seconds)"),
	m_fLeadOut("Lead-Out (seconds)"),
	m_RenderFrameRange("Render Frame Range", captRenderOutputData::eCaptureDrivers),
	m_fStartTime("Start Time"),
	m_fEndTime("End Time"),
	m_nCounterDigits("Counter Digits"),
	//m_nSubdivLevel("Subdiv Level"),
	m_bSmoothing("Smoothing"),			// Subdivision smoothing
	//m_bUseCaptureDrivers("Use Capture Drivers"),
	//m_fMarkerInTime("Marker In Time"),
	//m_fMarkerOutTime("Marker Out Time"),
	//m_bUseMarkerTimes("Use Marker Times"),
	m_bSendPostEmailAddress("Post Render Email Send"),
	m_PostEmailAddress("Post Render Email Send Addresses"),
	m_bExecutePostCommand("Post Render Command or Python File to Execute"),
	m_PostCommand("Post Render Command to Execute"),
	m_bBatchMode("Batch Mode"),
	m_bBatchSkipDialog("Skip all pop up dialogs in batch rendering"),
	m_bCannotOpen("Cannot open the current file"),
	m_nCurrentFrame("Current Frame"),
	m_CurrentScene("Current Scene"),
	m_CurrentCamera("Current Camera"),
	m_CurrentLayer("Current Layer"),
	m_CurrentSceneFilename("Current Scene Filename"),
	m_NumberOfScenes("Number Of Scenes"),
	m_CurrentSceneNumber("Current Scene Number"),
	m_RenderPosTime("Render Pos Time", c_InitRenderPosTime),
	m_RenderPosScene("Render Pos Scene"),
	m_RenderPosCamera("Render Pos Camera"),
	m_RenderPosLayer("Render Pos Render Layer"),
	m_RenderPosSaveFile("Render Pos Save File"),
	m_RenderOutputDataFile("Render Output Data File"),
	m_OutputFileName("Output File Name"),
	m_OutputMovieDirectory("Output Movie Directory"),
	m_Cameras("Cameras"),
	m_bRenderPosUse("Resume"),
	m_tRenderPosDelete("Delete Resume Point"),
	m_Resolution("Resolutions"),
	m_OutputFiles("Rendered Files"),
	m_bMoviePerDriver("Movie Per Capture Driver"),
	m_bUseLayerVisibility("Use Layer Visibility"),
	m_CurrentRenderPass("Current Render Pass"),
	m_CurrentRenderPassNoSpaces("Current Render Pass without Spaces"),
	m_CurrentStereoMode("Current Stereo Mode"),
	m_OutputDirectoryTags("Output Directory Tags"),
	m_bOutputDirectoryCustomize("Customize Output Directory", false),
	m_OutputFileTags("Output Filename Tags"),
	m_bOutputFileCustomize("Customize Output Filename", false),
	m_bHardwareAA("Hardware Antialiasing", true),
	m_BeautyPassName("Beauty",itString("BTY")),
	m_AOOnlyPassName("AO Only",itString("AO")),
	m_DepthPassName("Depth",itString("Z")),
	m_ShadowMaskPassName("Shadow Mask",itString("SHDW")),
	m_IlluminationOnlyPassName("Illumination Only",itString("ILLUM")),
	m_NormalsPassName("Normals",itString("NOR")),
	m_DirtyMattePassName("Dirty Matte",itString("MAT")),
	m_WireframePassName("Wireframe",itString("WIRE")),
	m_MaterialsPassName("Materials",itString("MTL")),
	m_ReflectionsOnlyPassName("Reflections Only",itString("REFL")),
	m_VelocityPassName("Velocity",itString("VEL")),
	m_DiffusePassName("Diffuse",itString("DIFF")),
	m_SpecularPassName("Specular",itString("SPEC")),
	m_BloomPassName("Bloom",itString("BLOOM")),
	m_StarPassName("Star",itString("STAR")),
	m_CameraDOFPassName("Camera DOF",itString("DOF")),
	m_EmissivePassName("Emissive",itString("EMI")),
	m_SpecEnvPassName("Specular Environment",itString("SPECENV")),
	m_SpecLitPassName("Specular Lights",itString("SPECLIT")),
	m_DiffEnvPassName("Diffuse Environment",itString("DIFFENV")),
	m_DiffLitPassName("Diffuse Lights",itString("DIFFLIT")),
	m_PreviewPassName("Preview",itString("PREV")),
	m_GIPassName("Global Illumination",itString("GI")),
	m_GlowPassName("Glow",itString("GLOW")),
	m_MrayFinalGatherPassName("MrayFinalGather",itString("FG")),
	m_RmanColorBleedPassName("RmanColorBleed",itString("COLBL"))

{
	m_FilterFunc.SetEnumTag(eFilterBox, "Box");
	m_FilterFunc.SetEnumTag(eFilterGaussian, "Gaussian");
	m_FilterFunc.SetEnumTag(eFilterMitchell, "Mitchell");
	m_FilterFunc.SetEnumTag(eFilterTriangle, "Triangle");
	m_FilterFunc.SetEnumTag(eFilterSinc, "Sinc");
	m_FilterFunc.SetEnumTag(eFilterLanczos, "Lanczos");
	m_FilterFunc.SetEnumTag(eFilterBlackmanHarris, "Blackman-Harris");
	m_FilterFunc.SetEnumTag(eFilterCatmullRom, "Catmull-Rom");

	m_ShadowQuality.SetEnumTag(0, "No Override");
	m_ShadowQuality.SetEnumTag(1, "Low");
	m_ShadowQuality.SetEnumTag(2, "Medium");
	m_ShadowQuality.SetEnumTag(3, "High");
	m_ShadowQuality.SetEnumTag(4, "Very High");

	m_PixelAspect.SetEnumTag(0, "Square");
	m_PixelAspect.SetEnumTag(1, "PAL (1.06667)");
	m_PixelAspect.SetEnumTag(2, "NTSC (0.9)");

	m_RenderFrameRange.SetEnumTag(eCaptureDrivers, "Use Capture Drivers");
	m_RenderFrameRange.SetEnumTag(eTimeline, "Timeline");
	m_RenderFrameRange.SetEnumTag(eSpecifyRange, "Specify Range");
};