/****************************************************************************\
**	mainInitApplication.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/mainInitApplication.hpp"

#include "MainApp/Commands/mainCommands.hpp"
#include "MainApp/mainPython.hpp"
#include "MainApp/mainResolvePath.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/FeaturesLayer.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/pyth/pythModules.hpp"
#include "Support/SupportLayer.hpp"
#include "Systems/AmbientOcclusion/aoSystem.hpp"
#include "Systems/Billboard/billSystem.hpp"
#include "Systems/Cameras/cmraSystem.hpp"
#include "Systems/Character/chtrSystem.hpp"
#include "Systems/Common/cmmSystem.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Systems/Environments/envtSystem.hpp"
#include "Systems/Fog/fogSystem.hpp"
#include "Systems/GlobalIllumination/giSystem.hpp"
#include "Systems/Groups/grupSystem.hpp"
#include "Systems/Layers/lyrsSystem.hpp"
#include "Systems/LightSets/lsetSystem.hpp"
#include "Systems/PrjLt/prjltSystem.hpp"
#include "Systems/PtLt/ptltSystem.hpp"
#include "Systems/Transforms/trfnSystem.hpp"

#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#if(SGPU_APP == MS_FUSION)
#include "FCSupport/FCSupportLayer.hpp"
#endif


//--------------------------------------------------------------------
// Initializes application packages
//--------------------------------------------------------------------
mainInitApplication::mainInitApplication(g2dSystem* i_pSystem)
{
	// Initalize here for our document type
	//
	fsLocator mru_file( gfPaths::GetPath( mnmPaths::e_Configs ) );
	mru_file.Push("RecentFiles.cfg");
	docSingleTypeMgr::SetMRUFile(mru_file);

	PrefsData& prefsdata = PrefsMgr::Data();
	docSingleTypeMgr::Init( prefsdata.m_MRUHistory.GetValue() );
	docSingleTypeMgr::SetFilter(".mab", "Scene Files");

	// grab the top item off of the MRU list and use that directory
	// as the default.  If the MRU is empty then set MS dir.
	//
	std::vector<fsLocator>	mru_list;
	docSingleTypeMgr::GetMRUList( mru_list );
	if ((mru_list.size()) > 0 && (mru_list[0].GetNumNames() > 0))
	{
		fsLocator dir = mru_list[0];
		dir.Pop();	// remove the filename
		guiSingleDocHandler::SetInitialDirectory( dir );
	}
	else
	{
		guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
	}
	guiSingleDocHandler::SetInitialDirectoryToBeLast(true);

	DBG_TRACE("Document Init complete.");

	//--------------------------------------------------
	// Initialize the systems in the application

	// Init common first
	cmmSystem::Init();
	DBG_TRACE("Common System Init complete.");
	SupportLayer::Init(i_pSystem);
	DBG_TRACE("Support Init complete.");
#if(SGPU_APP == MS_FUSION)
	FCSupportLayer::Init();
	DBG_TRACE("FC3D Support Init complete.");
#endif
	FeaturesLayer::Init(i_pSystem);
	DBG_TRACE("Features Init complete.");
	
	//	set-up the main menu items
	//
	mainCommands::SetupMenu();

	// Set up main python commands
	mainPython::AddCommands("mach");

	// Set up dialog handler for missing files.
	//Note: OrthoRender probably doesn't want to do this:
	mainResolvePath::Init();

#if(SGPU_APP == MS_FUSION)
	//	allow fc3d to load scenes even if assets are missing
	mainResolvePath::SetStrictMode(false);
#endif

	//SplashScreen::SetStatus("Initializing Systems"); System::Threading::Thread::Sleep(1);

	// Initialize MachStudio systems here
	//
	fsLocator app_dir( gfPaths::GetPath(gfPaths::e_AppPath) );
	
	// Geometry systems
//	setsSystem::Init(app_dir, itString("Sets"));
//	propSystem::Init(itString("Props"));
	chtrSystem::Init(app_dir, itString("Characters"));
//	prtclSystem::Init(app_dir, itString("Effects"));
	billSystem::Init(app_dir, itString("Effects"));

	guiMenuMgr::AddSeparator( "Create" );

	// Light systems
	ptltSystem::Init();
	prjltSystem::Init(app_dir, itString("Effects"));
//	dirltSystem::Init();

	guiMenuMgr::AddSeparator( "Create" );

	// Camera systems
	cmraSystem::Init(app_dir, itString("Sets")); // could have Cameras dir later?
//	dcutSystem::Init(i_pSystem);

	guiMenuMgr::AddSeparator( "Create" );

	// Ambient systems & Grouping systems
	fogSystem::Init();
	aoSystem::Init();
	giSystem::Init();
	//todSystem::Init();
//	skySystem::Init();
//	rcdSystem::Init();
//	sbrdSystem::Init(app_dir, itString("Storyboards"));
	envtSystem::Init(app_dir, itString("Effects"));
	grupSystem::Init();
	trfnSystem::Init();
	// Putting these last makes sure that the lights and objects 
	// are written to file before the light sets and layers
	lsetSystem::Init();
	lyrsSystem::Init();

	DBG_TRACE("Systems Init complete.");
	
	// Add the user interface for the Features packages
	FeaturesLayer::AddToMenu();

	// After all of the systems have initialized and created the
	// python commands they will need, we can now submit these
	// python commands to the interpretor. No more commands can be
	// created after this point.
	pythModules::SubmitModules();
	DBG_TRACE("Python Modules Init complete.");

}

//--------------------------------------------------------------------
// DeInitializes library packages
//--------------------------------------------------------------------
mainInitApplication::~mainInitApplication()
{
	// Clean up systems
	lyrsSystem::CleanUp();
	lsetSystem::CleanUp();
	trfnSystem::CleanUp();
	grupSystem::CleanUp();
//	sbrdSystem::CleanUp();
	envtSystem::CleanUp();
//	rcdSystem::CleanUp();
//	skySystem::CleanUp();
	//todSystem::CleanUp();
	giSystem::CleanUp();
	aoSystem::CleanUp();
	fogSystem::CleanUp();
//	dirltSystem::CleanUp();
//	dcutSystem::CleanUp();
	cmraSystem::CleanUp();
	prjltSystem::CleanUp();
	ptltSystem::CleanUp();
	billSystem::CleanUp();
//	prtclSystem::CleanUp();
//	propSystem::CleanUp();
	chtrSystem::CleanUp();
//	setsSystem::CleanUp();
	
	cmmSystem::CleanUp();

	FeaturesLayer::CleanUp();
#if(SGPU_APP == MS_FUSION)
	FCSupportLayer::CleanUp();
#endif
	SupportLayer::CleanUp();
}
