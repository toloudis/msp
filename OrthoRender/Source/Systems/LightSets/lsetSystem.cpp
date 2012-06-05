/*****************************************************************************
**  lsetSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/LightSets/lsetSystem.hpp"

#include "Systems/LightSets/GUI/lsetCommands.hpp"
#include "Systems/LightSets/GUI/lsetDialogInterest.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentInterest.hpp"
#include "Systems/LightSets/Timeline/lsetDriverCreator.hpp"
#include "Systems/LightSets/Object/lsetObjectMgr.hpp"
#include "Systems/LightSets/Object/lsetSelectInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/name/nameMgr.hpp"

namespace lsetSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<lsetObjectMgr, lsetLightSetObject> lsetNameInterest;

		lsetNameInterest*		l_pLightSetsNI = 0;
		lsetSelectInterest*		l_pLightSetsSI = 0;
		lsetDialogInterest*		l_pLightSetsDI = 0;
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
		//lsetCommands::SetupMenu();

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

		// Clean up namespaces
		lsetDialogUtil::CleanUp();

	}

}	// end of namespace
