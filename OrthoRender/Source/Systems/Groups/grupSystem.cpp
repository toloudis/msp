/*****************************************************************************
**  grupSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Groups/grupSystem.hpp"

#include "Systems/Groups/GUI/grupCommands.hpp"
#include "Systems/Groups/GUI/grupDialogInterest.hpp"
#include "Systems/Groups/GUI/grupDialogUtil.hpp"
#include "Systems/Groups/Data/grupDocumentInterest.hpp"
#include "Systems/Groups/Object/grupObjectMgr.hpp"
#include "Systems/Groups/Object/grupSelectInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
#include "Core/name/nameMgr.hpp"

namespace grupSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<grupObjectMgr, grupGroupObject> grupNameInterest;

		grupNameInterest*		l_pGroupsNI = 0;
		grupSelectInterest*		l_pGroupsSI = 0;
		grupDialogInterest*		l_pGroupsDI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		grupDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new grupDocumentInterest());
		//grupCommands::SetupMenu();

		// register the select interest
		if ( l_pGroupsSI == 0 )
		{
			l_pGroupsSI = new grupSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pGroupsSI );
		}

		// register the name interest
		if ( l_pGroupsNI == 0 )
		{
			l_pGroupsNI = new grupNameInterest();
			nameMgr::RegisterNameInterest( l_pGroupsNI );
		}
		// register the dialog interest
		if ( l_pGroupsDI == 0 )
		{
			l_pGroupsDI = new grupDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pGroupsDI );
		}

	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pGroupsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pGroupsSI );
			delete l_pGroupsSI;
			l_pGroupsSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pGroupsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pGroupsNI );
			delete l_pGroupsNI;
			l_pGroupsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pGroupsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pGroupsDI );
			delete l_pGroupsDI;
			l_pGroupsDI = 0;
		}

		// Clean up namespaces
		grupDialogUtil::CleanUp();

	}

}	// end of namespace
