/*****************************************************************************
**  ptltSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Systems/PtLt/ptltSystem.hpp"

#include "Systems/PtLt/GUI/ptltCommands.hpp"
#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"
#include "Systems/PtLt/GUI/ptltDialogInterest.hpp"
#include "Systems/PtLt/GUI/ptltDialogUtil.hpp"
#include "Systems/PtLt/Data/ptltDocumentInterest.hpp"
#include "Systems/PtLt/Timeline/ptltDriverCreator.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Object/ptltSelectInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/vis/visMgr.hpp"


namespace ptltSystem
{

	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<ptltObjectMgr, ptltPointLightObject> ptltNameInterest;
		typedef cmmPickInterestTemplate<ptltObjectMgr> ptltPickInterest;
		typedef cmmVisibleInterestTemplate<ptltObjectMgr> ptltVisibleInterest;

		ptltNameInterest*		l_pPointLightsNI = 0;
		ptltPickInterest*		l_pPointLightsPI = 0;
		ptltSelectInterest*		l_pPointLightsSI = 0;
		ptltVisibleInterest*	l_pPointLightsVI = 0;
		ptltDialogInterest*		l_pPointLightsDI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		ptltDialogUtil::Init();
		//ptltObjectMgr::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new ptltDocumentInterest());
//WXGUI
		/*
				ptltCommands::SetupMenu();
*/

		// register the pick interest
		if ( l_pPointLightsPI == 0 )
		{
			l_pPointLightsPI = new ptltPickInterest();
			pick3dMgr::RegisterPickInterest( l_pPointLightsPI );
		}

		// register the select interest
		if ( l_pPointLightsSI == 0 )
		{
			l_pPointLightsSI = new ptltSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pPointLightsSI );
		}

		// register the visible interest
		if ( l_pPointLightsVI == 0 )
		{
			l_pPointLightsVI = new ptltVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pPointLightsVI );
		}

		// register the name interest
		if ( l_pPointLightsNI == 0 )
		{
			l_pPointLightsNI = new ptltNameInterest();
			nameMgr::RegisterNameInterest( l_pPointLightsNI );
		}

		// register the dialog interest
		if ( l_pPointLightsDI == 0 )
		{
			l_pPointLightsDI = new ptltDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pPointLightsDI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new ptltDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		ptltDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pPointLightsPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pPointLightsPI );
			delete l_pPointLightsPI;
			l_pPointLightsPI = 0;
		}

		//	Unregister the Select Interest
		if ( l_pPointLightsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pPointLightsSI );
			delete l_pPointLightsSI;
			l_pPointLightsSI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pPointLightsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pPointLightsVI );
			delete l_pPointLightsVI;
			l_pPointLightsVI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pPointLightsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pPointLightsNI );
			delete l_pPointLightsNI;
			l_pPointLightsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pPointLightsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pPointLightsDI );
			delete l_pPointLightsDI;
			l_pPointLightsDI = 0;
		}

		// Clean up namespaces
		ptltCommands::CleanUp();
		//ptltObjectMgr::CleanUp();
		ptltDialogUtil::CleanUp();
	}

}	// end of namespace