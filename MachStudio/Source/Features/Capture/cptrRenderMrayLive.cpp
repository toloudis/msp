/*****************************************************************************
**	cptrRenderMrayLive.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderMrayLive.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"

#include "Features/Capture/cptrRenderMrayLiveData.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"

#include "ImportExport/mray/live/mrayLive.hpp"
#include "ImportExport/mray/export/mrayExportData.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mray/mrayExporter.hpp"
#include "Support/mray/mrayMgr.hpp"

#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiSplashScreen.hpp"
#include "Tool/gui/guiCursor.hpp"
#include "Tool/tma3d/tma3dScreenUtil.hpp"

#include <windows.h>

//----------------------------------------------------------------------------
// anonymous namespace
//----------------------------------------------------------------------------
namespace
{
	fsLocator	l_MRayLoc;
	fsLocator	l_MRayViewerLoc;
}

//----------------------------------------------------------------------------
// GetMrayLiveOptions()
//----------------------------------------------------------------------------
mrayOptionsData GetMrayLiveOptions(cptrRenderMrayLiveData m_Data, int i_RenderPass)
{

	bool allowReflections = false;
	if ( i_RenderPass == mrayLiveRenderPasses::e_Beauty || i_RenderPass == mrayLiveRenderPasses::e_Reflections )
	{
		allowReflections = true;
	}

	mrayOptionsData options;
	options.m_Verbosity = m_Data.m_MrayVerbosity.GetValue();
	options.m_NumReflBounces = m_Data.m_MrayNumReflBounces.GetValue();
	options.m_NumRefrBounces = m_Data.m_MrayNumRefrBounces.GetValue();
	options.m_MrayMaxTraceDepth = m_Data.m_MrayMaxTraceDepth.GetValue();
	options.m_bAO = m_Data.m_bMrayAO.GetValue();
	options.m_AOSamples = m_Data.m_MrayAOSamples.GetValue();
	options.m_bFinalGather = m_Data.m_bMrayFinalGather.GetValue();
	options.m_FGNDiffuse = m_Data.m_MrayFGNDiffuse.GetValue();
	options.m_FGNRefl = m_Data.m_MrayFGNRefl.GetValue();
	options.m_FGNRefr = m_Data.m_MrayFGNRefr.GetValue();
	options.m_FGNRays = m_Data.m_MrayFGNRays.GetValue();
	options.m_FGColor = m_Data.m_MrayFGColor.GetValue();
	options.m_OutputFormat = m_Data.m_MrayOutputFormat.GetValue();
	options.m_NumThreads = m_Data.m_MrayNumThreads.GetValue();
	options.m_MemoryLimit = m_Data.m_MrayMemoryLimit.GetValue();
	options.m_bEnableReflections = m_Data.m_bMrayEnableReflections.GetValue() & allowReflections;
	options.m_bEnableShadows = m_Data.m_bMrayEnableShadows.GetValue();
	options.m_MrayShadowType = m_Data.m_MrayShadowType.GetValue();
	options.m_bRewriteAssets = m_Data.m_bMrayRewriteAssets.GetValue();
	options.m_VerbosityLevel = m_Data.m_MrayVerbosityLevel.GetValue();
	options.m_bFGMapEnable = m_Data.m_bMrayFGMapEnable.GetValue();
	options.m_FGMapRebuild = m_Data.m_MrayFGMapRebuild.GetValue();
	options.m_FGMapPath = m_Data.m_MrayFGMapPath.GetValue();
	options.m_ReflSamples = m_Data.m_MrayReflSamples.GetValue();
	options.m_bOverrideMSPSampling = m_Data.m_bMrayOverrideMSPSampling.GetValue();
	options.m_MinCaptureSamples = m_Data.m_MrayMinCaptureSamples.GetValue();
	options.m_MaxCaptureSamples = m_Data.m_MrayMaxCaptureSamples.GetValue();
	options.m_AAContrast = m_Data.m_MRayAAContrast.GetValue();
	options.m_bProgressive = m_Data.m_bMrayProgressive.GetValue();
	options.m_bTonemapEnable = m_Data.m_bMRayTonemapEnable.GetValue();
	options.m_bEnableIBL = m_Data.m_bEnableIBL.GetValue();
	options.m_IBLQuality = m_Data.m_IBLQuality.GetValue();
	options.m_IBLMapRes = m_Data.m_IBLMapRes.GetValue();
	options.m_IBLScale = m_Data.m_IBLScale.GetValue();
	options.m_IBLSampleNum = m_Data.m_IBLSampleNum.GetValue();
	options.m_bIgnoreBadTex = m_Data.m_bMrayIgnoreBadTex.GetValue();
	options.m_bDisplayPreviewer = m_Data.m_bMrayDisplayPreview.GetValue();
	options.m_ProgSubsamplingSize = m_Data.m_MrayProgSubsamplingSize.GetValue();
	options.m_ProgSubsamplingMode = m_Data.m_MrayProgSubsamplingMode.GetValue();
	options.m_ProgSubsamplingPattern = m_Data.m_MrayProgSubsamplingPattern.GetValue();
	options.m_ProgMinSamples = m_Data.m_MrayProgMinSamples.GetValue();
	options.m_ProgMaxSamples = m_Data.m_MrayProgMaxSamples.GetValue();
	options.m_ProgMaxTime = m_Data.m_MrayProgMaxTime.GetValue();
	options.m_ProgErrorThreshold = m_Data.m_MrayProgErrorThreshold.GetValue();
	return options;
}

//--------------------------------------------------------------------
// SetupPasses()
//--------------------------------------------------------------------
void SetupPasses( mrayGlobalData & io_GlobalData, int i_RenderPass)
{

	io_GlobalData.m_bRenderEnvironments = true;
	io_GlobalData.m_bRenderLit = true;
	io_GlobalData.m_bRenderDiffuse = true;
	io_GlobalData.m_bRenderSpecular = true;
	io_GlobalData.m_bRenderTransparent = true;

	io_GlobalData.m_bRenderingBeautyOnly = false;
	io_GlobalData.m_bRenderingShadowsOnly = false;
	io_GlobalData.m_bRenderingNormalsOnly = false;
	io_GlobalData.m_bRenderingReflectionsOnly = false;
	io_GlobalData.m_bRenderingIlluminationOnly = false;
	io_GlobalData.m_bRenderingGIOnly = false;
	io_GlobalData.m_bRenderingAOOnly = false;

	switch(i_RenderPass)
	{
		case mrayLiveRenderPasses::e_Diffuse:
			io_GlobalData.m_bRenderSpecular = false;
			break;
		case mrayLiveRenderPasses::e_DiffuseEnvironment:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case mrayLiveRenderPasses::e_DiffuseLights:
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case mrayLiveRenderPasses::e_Specular:
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case mrayLiveRenderPasses::e_SpecularEnvironment:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case mrayLiveRenderPasses::e_SpecularLights:
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case mrayLiveRenderPasses::e_Emissive:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case mrayLiveRenderPasses::e_AmbientOcclusion:
			io_GlobalData.m_bRenderingAOOnly = true;
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;
			io_GlobalData.m_bRenderSpecular = false;
			break;
		case mrayLiveRenderPasses::e_ShadowMask:
			io_GlobalData.m_bRenderingShadowsOnly = true;
			break;
		case mrayLiveRenderPasses::e_Illumination:
			io_GlobalData.m_bRenderingIlluminationOnly = true;
			break;
		case mrayLiveRenderPasses::e_Normals:
			io_GlobalData.m_bRenderingNormalsOnly = true;
			break;
		case mrayLiveRenderPasses::e_Reflections:
			io_GlobalData.m_bRenderingReflectionsOnly = true;
			break;
		case mrayLiveRenderPasses::e_FinalGather:
			io_GlobalData.m_bRenderingGIOnly = true;
			break;
		case mrayLiveRenderPasses::e_Beauty:
			io_GlobalData.m_bRenderingBeautyOnly = true;
			break;
	};
}

//--------------------------------------------------------------------
// SetMRayLoc()
//--------------------------------------------------------------------
void cptrRenderMrayLive::SetMRayLoc( fsLocator i_Loc )
{
	l_MRayLoc = i_Loc;
}

//--------------------------------------------------------------------
// SetMRayViewerLoc()
//--------------------------------------------------------------------
void cptrRenderMrayLive::SetMRayViewerLoc( fsLocator i_Loc )
{
	l_MRayViewerLoc = i_Loc;
}

//------------------------------------------------------------------------
// Start()
//------------------------------------------------------------------------
void cptrRenderMrayLive::Start(cptrRenderMrayLiveData i_Data)
{
	//open a dialog to notify the user that the packaging process is taking place
	guiSplashScreen::SetDoSplashTimeout(false);
	fsLocator splashPath = gfPaths::GetPath(gfPaths::e_ExePath);
	splashPath.Push("Data");
	splashPath.Push("Load.png");
	if (fsFileUtil::FileExists(splashPath))
	{
		guiSplashScreen::StartUp(splashPath, "Exporting scene to .MI... please wait.");
	}
	//set wait cursor
	guiCursor::SetWaitCursor();

	mrayMgr::ClearWrittenTextures();

	camCamera cam = cam3dMgr::GetCamera();

	itString sceneName = docSingleDocumentMgr::GetFilename().GetLastName();
	sceneName.StripExtension();
	itString sceneNameWithExt = sceneName;
	sceneNameWithExt += itString(".mi");

	fsLocator mrayLiveRoot = gfPaths::GetPath(gfPaths::e_UserDataPath);
	mrayLiveRoot.Push("mrayLiveCache");

	fsLocator currSceneRoot = mrayLiveRoot;
	currSceneRoot.Push( sceneName );

	fsLocator fullSceneLoc = currSceneRoot;
	fullSceneLoc.Push( sceneNameWithExt );

	mrayGlobalData renderData;
	renderData.m_Options = GetMrayLiveOptions(i_Data,i_Data.m_MrayRenderPass.GetValue());

	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();

	renderData.m_AspectRatio = cam.GetAspect();
	renderData.m_Camera = &cam;
	renderData.m_FileName = fullSceneLoc;
	renderData.m_CurrentScene = sceneName;
	renderData.m_Width = (int)winsize.GetX();
	renderData.m_Height = (int)winsize.GetY();
	renderData.m_FilterFunc = i_Data.m_MrayPixelFilterType.GetValue();
	renderData.m_FilterWidth = i_Data.m_MrayPixelFilterSize.GetValue();
	renderData.m_CaptureSampling = 1;
	renderData.m_MRayLoc = l_MRayLoc;

	SetupPasses(renderData,i_Data.m_MrayRenderPass.GetValue());

	fsLocator rootLoc = renderData.m_FileName;
	rootLoc.Pop();

	mrayMgr::SetupMRayDirectories(rootLoc,renderData, false);

	itString worldName = fullSceneLoc.GetLastName();
	worldName.StripExtension();
	worldName += itString("_WORLD.mi");
	renderData.m_WorldName = renderData.m_ArchivesLoc;
	renderData.m_WorldName.Push(worldName);

	// Gather everything
	mrayExportData myExportData = mrayMgr::GetPotentialExportData(renderData);

	// Write textures
	mrayMgr::WriteTextures( renderData );
	mrayMgr::WriteShaders( renderData );

	// Export everything
	itString itFileName;
	fsFileUtil::LocatorToUnicodeString( fullSceneLoc, itFileName );
	mrayExporter exporter( itFileName.GetString() , renderData );
	mrayMgr::DoExport( myExportData , exporter );


	//stop wait cursor
	guiCursor::EndWaitCursor();
	//Close dialog
	guiSplashScreen::ShutDown();
	//reset splash screen to timeout
	guiSplashScreen::SetDoSplashTimeout(true);

	// bin library path will be the mental ray exe path for now:
	fsLocator libPathLoc = l_MRayLoc;
	libPathLoc.Pop();
	std::string libPathStr;
	fsFileUtil::LocatorToANSIFilename(libPathLoc,libPathStr);

	// Launch MR
	std::string rootStr;
	fsFileUtil::LocatorToANSIFilename(rootLoc,rootStr);
	std::ostringstream params;
	params << "-verbose " << (renderData.m_Options.m_VerbosityLevel+1) << 
		      " -threads " << renderData.m_Options.m_NumThreads << 
			  " -memory " << renderData.m_Options.m_MemoryLimit << 
			  " -texture_continue " << (renderData.m_Options.m_bIgnoreBadTex?"on":"off") <<
			  " -L \"" << libPathStr << "\"" <<
			  " -file_dir \"" << rootStr << "\"";

	fsLocator batchFileLoc = mrayMgr::SetupMRayBatch(rootLoc,renderData,params.str(),true);

	std::string batchFile;
	fsFileUtil::LocatorToANSIFilename(batchFileLoc,batchFile);

	WinExec(batchFile.c_str(),true);

	//mrayLive::StartRender(fullSceneLoc,m_Data.m_MrayRenderPass.GetValue());
}

//------------------------------------------------------------------------
// Stop()
//------------------------------------------------------------------------
void cptrRenderMrayLive::Stop()
{
	mrayLive::StopRender();
}

//------------------------------------------------------------------------
// CleanUp()
//------------------------------------------------------------------------
void cptrRenderMrayLive::CleanUp()
{
	mrayLive::CleanUp();
}