/*****************************************************************************
**  FeaturesLayer.cpp
**
**      FeaturesLayer contains the initialization functions
**	for the all packages within the Features folder.
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/FeaturesLayer.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Channels/chnlPackage.hpp"
#include "Features/Capture/cptrRenderStatsDataUtil.hpp"
#include "Features/Capture/cptrPackage.hpp"
#include "Features/Capture/cptrRenderMrayLiveUtil.hpp"
#include "Features/Capture/cptrRenderMrayLiveMgr.hpp"
#include "Features/Capture/cptrRenderRmanLiveUtil.hpp"
#include "Features/Capture/cptrRenderRmanLiveMgr.hpp"
#include "Features/Debug/dbgConsoleDataUtil.hpp"
//#include "Features/EONReality/eonCommands.hpp"
#include "Features/Export/ExportUtil.hpp"
#include "Features/Export/FBXExportUtil.hpp"
#include "Features/FilmGates/fgtPackage.hpp"
#include "Features/Keyframing/keyfPackage.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "Features/MaterialBrowse/mbrwPackage.hpp"
//#include "Features/MayaExport/MayaExportUtil.hpp"
#include "Features/ObjectManip/mnpPackage.hpp"
#include "Features/Playback/plbkPackage.hpp"
#include "Features/Prefs/prefsCommands.hpp"
#include "Features/Prefs/wxGUI/prefsLayoutMgr.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
//#include "Features/Prefs/prefsQuickMgr.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Features/PythonObject/pytFeature.hpp"
#include "Features/RenderLayers/rdrLayersDataUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Features/RenderPanels/rpnPackage.hpp"
#include "Features/Reports/rptPackage.hpp"
#include "Features/ResourceTracker/ResourceTrackerPackage.hpp"
#include "Features/SceneSetup/SceneSetupMgr.hpp"
//#include "Features/SelectList/SelectListDialogUtil.hpp"
#include "Features/UndoHistory/UndoHistoryUtil.hpp"
#include "Features/UndoHistory/UndoHistoryDialogUtil.hpp"

#include "Tool/gui/guiDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
namespace
{
	int l_RefCount = 0;
}


//----------------------------------------------------------------------------
//	Init
//----------------------------------------------------------------------------
void FeaturesLayer::Init(g2dSystem *i_pSystem)
{
	// use ref count to only initialize once
	if ( l_RefCount == 0 )
	{
		// initialize packages in the layer
		chnlPackage::Init();
		cptrPackage::Init(i_pSystem);
		mnpPackage::Init();
		keyfPackage::Init();
		fgtPackage::Init();
		plbkPackage::Init();
		mbrwPackage::Init();
		rptPackage::Init();
		rpnPackage::Init();
		ProjectSetupMgr::Init();
		SceneSetupMgr::Init();
		//SelectListDialogUtil::Init();
		UndoHistoryDialogUtil::Init();
		ResourceTrackerPackage::Init();
		//LoadPrefsMgr::Init();
		pytFeature::Init();

#ifdef USE_WXWIDGETS
		// Read in layout configurations
		prefsLayoutMgr::ReadLayouts();
#endif
	}

	//	increment the ref count
	l_RefCount++;
}

//----------------------------------------------------------------------------
//	CleanUp
//----------------------------------------------------------------------------
void FeaturesLayer::CleanUp() throw()
{
	l_RefCount--;

	// use ref count to make sure we only clean up once, when everyone is done
	if ( l_RefCount == 0 )
	{
		// clean up our internal stuff, in reverse order
		//
		pytFeature::CleanUp();
		//prefsQuickMgr::CleanUp();
		//LoadPrefsMgr::CleanUp();
		PrefsMgr::CleanUp();
		rndrPrefsMgr::CleanUp();
		cptrRenderMrayLiveMgr::CleanUp();
		cptrRenderRmanLiveMgr::CleanUp();
		ResourceTrackerPackage::CleanUp();
		UndoHistoryDialogUtil::CleanUp();
		//SelectListDialogUtil::CleanUp();
		SceneSetupMgr::CleanUp();
		ProjectSetupMgr::CleanUp();
		plbkPackage::CleanUp();
		mbrwPackage::CleanUp();
		fgtPackage::CleanUp();
		keyfPackage::CleanUp();
		mnpPackage::CleanUp();
		rpnPackage::CleanUp();
		rptPackage::CleanUp();
		cptrPackage::CleanUp();
		chnlPackage::CleanUp();
	}
}

//------------------------------------------------------------------------
//	AddToMenu - add user interface elements for Features packages
//------------------------------------------------------------------------
void FeaturesLayer::AddToMenu()
{
	ImportUtil::AddToMenu();
	prefsCommands::AddToMenu();
	rndrPrefsMgr::AddToMenu();
	UndoHistoryUtil::AddToMenu();
	cptrRenderStatsDataUtil::AddToMenu();
	cptrRenderMrayLiveUtil::AddToMenu();
	cptrRenderRmanLiveUtil::AddToMenu();
	//MayaExportUtil::AddToMenu();
	//eonCommands::AddToMenu();
	dbgConsoleDataUtil::AddToMenu();
	rdrLayersDataUtil::AddToMenu();
	ExportUtil::AddToMenu();
	FBXExportUtil::AddToMenu();

	chnlDialogUtil::CreateChannelEditor();

	// Create other tabbed dialogs now so that the saved layout can restore 
	// their positions and visibility
	guiDialogTabbedMgr::Create("Driver", "Driver Properties");

	//	need to initialize this at the end of system inits, so all commands have been registered.
	//
	//prefsQuickMgr::Init();
}