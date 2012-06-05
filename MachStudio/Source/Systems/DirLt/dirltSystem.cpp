/*****************************************************************************
**  dirltSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltSystem.hpp"

#include "dirltCommands.hpp"
#include "dirltCommandIconsVisible.hpp"
#include "dirltDialogUtil.hpp"
#include "dirltScriptObject.hpp"
#include "dirltDocumentInterest.hpp"
#include "dirltDriverCreator.hpp"
#include "dirltObjectMgr.hpp"
#include "dirltSelectInterest.hpp"

#include "chnlCommandUtil.hpp"
#include "cmaCommandMgr.hpp"
#include "cmmNameInterestTemplate.hpp"
#include "cmmPickInterestTemplate.hpp"
#include "cmmVisibleInterestTemplate.hpp"
#include "dayPackage.hpp"
#include "daySunMoonLights.hpp"
#include "docSingleTypeMgr.hpp"
#include "muiMenuMgr.hpp"
#include "nameMgr.hpp"
#include "pick3dMgr.hpp"
#include "tmaCommandTabControlUtil.hpp"
#include "visMgr.hpp"

#include <string>


namespace dirltSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<dirltObjectMgr, dirltDirLightObject> dirltNameInterest;
		typedef cmmPickInterestTemplate<dirltObjectMgr> dirltPickInterest;
		typedef cmmVisibleInterestTemplate<dirltObjectMgr> dirltVisibleInterest;

		dirltNameInterest*		l_pDirLightsNI = 0;
		dirltPickInterest*		l_pDirLightsPI = 0;
		dirltSelectInterest*	l_pDirLightsSI = 0;
		dirltVisibleInterest*	l_pDirLightsVI = 0;

		//--------------------------------------------------------------------
		//	GetLights - returns a reference to the directional light list
		//--------------------------------------------------------------------
		void add_day_lights()
		{
			std::vector<g3dDirectionalLight*> lights = daySunMoonLights::GetLights();
			int index;
			index = dirltObjectMgr::AddDirLight( lights[daySunMoonLights::e_Sun] );
			dirltObjectMgr::GetObject( index )->SetName( std::string("Sun") );
			index = dirltObjectMgr::AddDirLight( lights[daySunMoonLights::e_Moon] );
			dirltObjectMgr::GetObject( index )->SetName( std::string("Moon") );
			//dirltObjectMgr::SelectObject(0);
		}
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		//	add the day lights to the dir light list
		//
		// FIX: add_day_lights();

		// Initialize namespaces
		dirltDialogUtil::Init();
		//dirltObjectMgr::Init();

		dayPackage::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new dirltDocumentInterest());
		dirltCommands::SetupMenu();

		// register the pick interest
		if ( l_pDirLightsPI == 0 )
		{
			l_pDirLightsPI = new dirltPickInterest();
			pick3dMgr::RegisterPickInterest( l_pDirLightsPI );
		}

		// register the select interest
		if ( l_pDirLightsSI == 0 )
		{
			l_pDirLightsSI = new dirltSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pDirLightsSI );
		}

		// register the visible interest
		if ( l_pDirLightsVI == 0 )
		{
			l_pDirLightsVI = new dirltVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pDirLightsVI );
		}

		// register the name interest
		if ( l_pDirLightsNI == 0 )
		{
			l_pDirLightsNI = new dirltNameInterest();
			nameMgr::RegisterNameInterest( l_pDirLightsNI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new dirltDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		dirltDriverCreator::CreateParsers();

		//	create the system tab page
		System::Windows::Forms::TabPage* pTP = tmaCommandTabControlUtil::CreateSystemTabPage( S"Dir Light" );

		//
		//	commands
		//
		int menu_id = muiMenuMgr::AddMenuItem( "View", "Dir Light Icons" );
		cmaCommand* pCmd = new dirltCommandIconsVisible();
		cmaCommandMgr::AddAndRegister( pCmd, dirltCommandIconsVisible::GetConstTagName(), menu_id );
		cmaCommandMgr::CommandSetChecked( pCmd, true );
		tmaCommandTabControlUtil::AddShortcutButton( pTP->Name, pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( S"Spline", pTP->Name );
		chnlCommandUtil::CreateDriverButton( S"Attach", pTP->Name );
		chnlCommandUtil::CreateDriverButton( S"Static Position", pTP->Name );
		chnlCommandUtil::CreateDriverButton( S"Enabled", pTP->Name );
		chnlCommandUtil::CreateDriverButton( S"Color", pTP->Name );
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	remove the tab page
		tmaDialogTabbedMgr::RemoveTabPage( S"System", S"Dir Light" );

		dayPackage::CleanUp();

		//	Unregister the Pick Interest
		if ( l_pDirLightsPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pDirLightsPI );
			delete l_pDirLightsPI;
			l_pDirLightsPI = 0;
		}

		//	Unregister the Select Interest
		if ( l_pDirLightsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pDirLightsSI );
			delete l_pDirLightsSI;
			l_pDirLightsSI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pDirLightsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pDirLightsVI );
			delete l_pDirLightsVI;
			l_pDirLightsVI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pDirLightsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pDirLightsNI );
			delete l_pDirLightsNI;
			l_pDirLightsNI = 0;
		}

		// Clean up namespaces
		//dirltObjectMgr::CleanUp();
		dirltDialogUtil::CleanUp();
	}

}	// end of namespace