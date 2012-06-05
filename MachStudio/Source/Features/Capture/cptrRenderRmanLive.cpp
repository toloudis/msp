/*****************************************************************************
**	cptrRenderRmanLive.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Features/Capture/cptrRenderRmanLive.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileBin.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/it/itStringUtil.hpp"

#include "Features/Capture/cptrRenderRmanLiveData.hpp"
#include "Features/Capture/cptrRenderUtil.hpp"

#include "ImportExport/rman/live/rmanLive.hpp"
#include "ImportExport/rman/export/rmanExportData.hpp"

#include "Support/cams/camsCameraMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/rman/rmanExporter.hpp"
#include "Support/rman/rmanMgr.hpp"

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
	fsLocator	l_RManLoc;
	fsLocator	l_RManViewerLoc;
}

//----------------------------------------------------------------------------
// GetRmanLiveOptions()
//----------------------------------------------------------------------------
rmanOptionsData GetRmanLiveOptions(cptrRenderRmanLiveData m_Data, int i_RenderPass)
{
	rmanOptionsData options;

	bool allowReflections = false;
	if ( i_RenderPass == rmanLiveRenderPasses::e_Beauty || i_RenderPass == rmanLiveRenderPasses::e_Reflections )
	{
		allowReflections = true;
	}

	options.m_RmanShadingRate = m_Data.m_RmanShadingRate.GetValue();
	options.m_RmanAOsamples = m_Data.m_RmanAOsamples.GetValue();
	options.m_bRmanRewriteAssets = m_Data.m_bRmanCacheTextures.GetValue();
	options.m_bRmanDisableWarnings = m_Data.m_bRmanDisableWarnings.GetValue();
	options.m_bRmanAOEnable = m_Data.m_bRmanAOEnable.GetValue();
	options.m_RmanAOMaxVariation = m_Data.m_RmanAOMaxVariation.GetValue();
	options.m_bRmanReflEnable = m_Data.m_bRmanReflEnable.GetValue() & allowReflections;
	options.m_RmanReflType = m_Data.m_RmanReflType.GetValue();
	options.m_bRmanShadowEnable = m_Data.m_bRmanShadowEnable.GetValue();
	options.m_RmanShadowType = m_Data.m_RmanShadowType.GetValue();
	options.m_bRmanGIEnable = m_Data.m_bRmanGIEnable.GetValue();
	options.m_RmanGIsamples = m_Data.m_RmanGIsamples.GetValue();
	options.m_RmanGIMaxVariation = m_Data.m_RmanGIMaxVariation.GetValue();
	options.m_RmanNumCores = m_Data.m_RmanNumCores.GetValue();
	options.m_RmanTexMemory = m_Data.m_RmanTexMemory.GetValue();
	options.m_RmanBucketOrder = m_Data.m_RmanBucketOrder.GetValue();
	options.m_RmanBucketSize = m_Data.m_RmanBucketSize.GetValue();
	options.m_RmanRayDepth = m_Data.m_RmanRayDepth.GetValue();
	options.m_bTonemapEnable = m_Data.m_bRmanTonemapEnable.GetValue();
	return options;
}

//--------------------------------------------------------------------
// SetupPasses()
//--------------------------------------------------------------------
void SetupPasses( rmanGlobalData & io_GlobalData, int i_RenderPass)
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
		case rmanLiveRenderPasses::e_Diffuse:
			io_GlobalData.m_bRenderSpecular = false;
			break;
		case rmanLiveRenderPasses::e_DiffuseEnvironment:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case rmanLiveRenderPasses::e_DiffuseLights:
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case rmanLiveRenderPasses::e_Specular:
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case rmanLiveRenderPasses::e_SpecularEnvironment:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case rmanLiveRenderPasses::e_SpecularLights:
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderDiffuse = false;		
			break;
		case rmanLiveRenderPasses::e_Emissive:
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;
			io_GlobalData.m_bRenderSpecular = false;			
			break;
		case rmanLiveRenderPasses::e_AmbientOcclusion:
			io_GlobalData.m_bRenderingAOOnly = true;
			io_GlobalData.m_bRenderEnvironments = false;
			io_GlobalData.m_bRenderLit = false;
			io_GlobalData.m_bRenderDiffuse = false;
			io_GlobalData.m_bRenderSpecular = false;
			break;
		case rmanLiveRenderPasses::e_ShadowMask:
			io_GlobalData.m_bRenderingShadowsOnly = true;
			break;
		case rmanLiveRenderPasses::e_Illumination:
			io_GlobalData.m_bRenderingIlluminationOnly = true;
			break;
		case rmanLiveRenderPasses::e_Normals:
			io_GlobalData.m_bRenderingNormalsOnly = true;
			break;
		case rmanLiveRenderPasses::e_Reflections:
			io_GlobalData.m_bRenderingReflectionsOnly = true;
			break;
		case rmanLiveRenderPasses::e_ColorBleed:
			io_GlobalData.m_bRenderingGIOnly = true;
			break;
		case rmanLiveRenderPasses::e_Beauty:
			io_GlobalData.m_bRenderingBeautyOnly = true;
			break;
	};
}

//------------------------------------------------------------------------
// Start()
//------------------------------------------------------------------------
void cptrRenderRmanLive::Start(cptrRenderRmanLiveData i_Data)
{

	//open a dialog to notify the user that the packaging process is taking place
	guiSplashScreen::SetDoSplashTimeout(false);
	fsLocator splashPath = gfPaths::GetPath(gfPaths::e_ExePath);
	splashPath.Push("Data");
	splashPath.Push("Load.png");
	if (fsFileUtil::FileExists(splashPath))
	{
		guiSplashScreen::StartUp(splashPath, "Exporting scene to .RIB and rewriting textures... please wait.");
	}
	//set wait cursor
	guiCursor::SetWaitCursor();

	rmanMgr::ClearWrittenTextures();

	camCamera cam = cam3dMgr::GetCamera();

	itString sceneName = docSingleDocumentMgr::GetFilename().GetLastName();
	sceneName.StripExtension();
	itString sceneNameWithExt = sceneName;
	sceneNameWithExt += itString(".rib");

	fsLocator rmanLiveRoot = gfPaths::GetPath(gfPaths::e_UserDataPath);
	rmanLiveRoot.Push("rmanLiveCache");

	fsLocator currSceneRoot = rmanLiveRoot;
	currSceneRoot.Push( sceneName );

	fsLocator fullSceneLoc = currSceneRoot;
	fullSceneLoc.Push( sceneNameWithExt );

	maPoint2d winsize = tma3dScreenUtil::GetWindowSize();

	////////////////

	fsLocator rootLoc = fullSceneLoc;
	rootLoc.Pop();

	itString currFrameNameIt = fullSceneLoc.GetLastName();
	currFrameNameIt.StripExtension();
	std::string currFrameName = itStringUtil::GetStdString(currFrameNameIt);
	//std::string sceneName = itStringUtil::GetStdString(sceneName);

	std::string engine = "prman";
	std::string renderer = "";

	fsLocator prmanLoc = rmanMgr::GetPrmanLoc();

	fsFileUtil::LocatorToANSIFilename( prmanLoc , renderer );

	fsLocator ribDirectory = fullSceneLoc;
	ribDirectory.Pop();
	ribDirectory.Push("rman_assets");

	rmanGlobalData globalData;

	rmanMgr::SetupRendermanDirectories(ribDirectory,globalData);

	rmanExportData myExportData = rmanMgr::GetPotentialExportData(globalData);

	globalData.m_Options = GetRmanLiveOptions(i_Data,i_Data.m_RmanRenderPass.GetValue());

	globalData.m_FrameName = currFrameName;
	globalData.m_RmanFilterType = 0;
	globalData.m_RmanFilterWidth = 1;
	globalData.m_RmanAArate = 4;
	globalData.m_SceneCamera = &cam;
	globalData.m_width = (int)winsize.GetX();
	globalData.m_height = (int)winsize.GetY();
	globalData.m_PixelAspectRatio = cam.GetAspect();
	globalData.m_RibPath = fullSceneLoc;
	globalData.m_bRenderDOF = false;
	globalData.m_Engine = "prman";
	globalData.m_CommandNumber = 0;

	globalData.m_RmanFilterType = i_Data.m_RmanFilterType.GetValue();
	globalData.m_RmanFilterWidth = i_Data.m_RmanFilterWidth.GetValue();
	globalData.m_RmanAArate = i_Data.m_nRmanAArate.GetValue();


	SetupPasses(globalData,i_Data.m_RmanRenderPass.GetValue());

	fsLocator worldLoc = globalData.m_ArchivesLoc;
	std::string worldFileName = globalData.m_FrameName + "_WORLD.rib";
	worldLoc.Push(worldFileName.c_str());

	rmanExporter exporter(globalData);

	rmanMgr::DoExport( exporter , myExportData );

	rmanMgr::WriteTiffTextures( globalData );


	////////////////

	//stop wait cursor
	guiCursor::EndWaitCursor();
	//Close dialog
	guiSplashScreen::ShutDown();
	//reset splash screen to timeout
	guiSplashScreen::SetDoSplashTimeout(true);

	fsLocator batchFileLoc = rmanMgr::SetupRendermanBatch(ribDirectory,myExportData,fullSceneLoc,renderer,exporter.GetGlobalData());

	std::string batchFile;
	fsFileUtil::LocatorToANSIFilename(batchFileLoc,batchFile);

	WinExec(batchFile.c_str(),true);
}

//------------------------------------------------------------------------
// Stop()
//------------------------------------------------------------------------
void cptrRenderRmanLive::Stop()
{
	rmanLive::StopRender();
}

//------------------------------------------------------------------------
// CleanUp()
//------------------------------------------------------------------------
void cptrRenderRmanLive::CleanUp()
{
	rmanLive::CleanUp();
}