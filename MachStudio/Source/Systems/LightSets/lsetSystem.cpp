/*****************************************************************************
**  lsetSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/LightSets/lsetSystem.hpp"

#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"

#include "Systems/LightSets/GUI/lsetCommands.hpp"
#include "Systems/LightSets/GUI/lsetContextMenuInterest.hpp"
#include "Systems/LightSets/GUI/lsetDialogInterest.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentInterest.hpp"
#include "Systems/LightSets/Timeline/lsetDriverCreator.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Object/lsetSelectInterest.hpp"
#include "Systems/LightSets/Object/lsetRendermanExportInterest.hpp"
#include "Systems/LightSets/Object/lsetMRayExportInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"

//#include "Core/name/nameMgr.hpp"

namespace lsetSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<lsetObjectMgr, lsetLightSetObject> lsetNameInterest;

		lsetNameInterest*				l_pLightSetsNI = NULL;
		lsetSelectInterest*				l_pLightSetsSI = NULL;
		lsetDialogInterest*				l_pLightSetsDI = NULL;
		lsetContextMenuInterest*		l_pLightSetsCMI = NULL;
		lsetRendermanExportInterest*	l_pLightSetsREI = NULL;
		lsetMRayExportInterest*			l_pLightSetsMRayEI = NULL;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		lsetDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new lsetDocumentInterest());
		lsetCommands::SetupMenu();

		// register the select interest
		if ( l_pLightSetsSI == 0 )
		{
			l_pLightSetsSI = new lsetSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pLightSetsSI );
		}

		// register the name interest
		if ( l_pLightSetsNI == 0 )
		{
			l_pLightSetsNI = new lsetNameInterest();
			nameMgr::RegisterNameInterest( l_pLightSetsNI );
		}
		// register the dialog interest
		if ( l_pLightSetsDI == 0 )
		{
			l_pLightSetsDI = new lsetDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pLightSetsDI );
		}

		// register the context menu interest
		if ( l_pLightSetsCMI == 0 )
		{
			l_pLightSetsCMI = new lsetContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pLightSetsCMI );
		}

		// register the renderman export interest
		if ( l_pLightSetsREI == 0 )
		{
			l_pLightSetsREI = new lsetRendermanExportInterest();
			rmanMgr::RegisterExportInterest( l_pLightSetsREI );
		}

		// register the mray export interest
		if ( l_pLightSetsMRayEI == 0 )
		{
			l_pLightSetsMRayEI = new lsetMRayExportInterest();
			mrayMgr::RegisterExportInterest( l_pLightSetsMRayEI );
		}
		

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new lsetDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		lsetDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pLightSetsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pLightSetsSI );
			delete l_pLightSetsSI;
			l_pLightSetsSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pLightSetsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pLightSetsNI );
			delete l_pLightSetsNI;
			l_pLightSetsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pLightSetsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pLightSetsDI );
			delete l_pLightSetsDI;
			l_pLightSetsDI = 0;
		}

		//	Unregister the context menu Interest
		if ( l_pLightSetsCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pLightSetsCMI );
			delete l_pLightSetsCMI;
			l_pLightSetsCMI = 0;
		}

		//	Unregister the Renderman Export Interest
		if ( l_pLightSetsREI != 0 )
		{
			rmanMgr::UnRegisterExportInterest( l_pLightSetsREI );
			delete l_pLightSetsREI;
			l_pLightSetsREI = 0;
		}

		//	Unregister the mray Export Interest
		if ( l_pLightSetsMRayEI != 0 )
		{
			mrayMgr::UnRegisterExportInterest( l_pLightSetsMRayEI );
			delete l_pLightSetsMRayEI;
			l_pLightSetsMRayEI = 0;
		}

		// Clean up namespaces
		lsetDialogUtil::CleanUp();

	}

}	// end of namespace
