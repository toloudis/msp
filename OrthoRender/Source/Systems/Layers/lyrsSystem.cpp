/*****************************************************************************
**  lyrsSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Layers/lyrsSystem.hpp"

#include "Systems/Layers/GUI/lyrsCommands.hpp"
#include "Systems/Layers/GUI/lyrsDialogInterest.hpp"
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"
#include "Systems/Layers/Data/lyrsDocumentInterest.hpp"
#include "Systems/Layers/Object/lyrsObjectMgr.hpp"
#include "Systems/Layers/Object/lyrsSelectInterest.hpp"
#include "Systems/Layers/Data/lyrsVisibleInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/name/nameMgr.hpp"
#include "Support/vis/visMgr.hpp"

namespace lyrsSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<lyrsObjectMgr, lyrsLayerObject> lyrsNameInterest;

		lyrsNameInterest*		l_pLayersNI = 0;
		lyrsSelectInterest*		l_pLayersSI = 0;
		lyrsDialogInterest*		l_pLayersDI = 0;
		lyrsVisibleInterest*l_pLayersVI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		lyrsDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new lyrsDocumentInterest());
		//lyrsCommands::SetupMenu();

		// register the select interest
		if ( l_pLayersSI == 0 )
		{
			l_pLayersSI = new lyrsSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pLayersSI );
		}

		// register the name interest
		if ( l_pLayersNI == 0 )
		{
			l_pLayersNI = new lyrsNameInterest();
			nameMgr::RegisterNameInterest( l_pLayersNI );
		}
		// register the dialog interest
		if ( l_pLayersDI == 0 )
		{
			l_pLayersDI = new lyrsDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pLayersDI );
		}

		// register the visible interest
		if ( l_pLayersVI == 0 )
		{
			l_pLayersVI = new lyrsVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pLayersVI );
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pLayersSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pLayersSI );
			delete l_pLayersSI;
			l_pLayersSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pLayersNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pLayersNI );
			delete l_pLayersNI;
			l_pLayersNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pLayersDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pLayersDI );
			delete l_pLayersDI;
			l_pLayersDI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pLayersVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pLayersVI );
			delete l_pLayersVI;
			l_pLayersVI = 0;
		}

		// Clean up namespaces
		lyrsDialogUtil::CleanUp();

	}

}	// end of namespace
