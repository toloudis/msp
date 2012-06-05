/*****************************************************************************
**  setsSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/setsSystem.hpp"

#include "Systems/Sets/GUI/setsCommands.hpp"
#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/GUI/setsDialogInterest.hpp"
#include "Systems/Sets/GUI/setsDialogUtil.hpp"
#include "Systems/Sets/Data/setsDocumentInterest.hpp"
#include "Systems/Sets/setsExportInterest.hpp"
#include "Systems/Sets/GUI/setsGeomList.hpp"
#include "Systems/Sets/setsNameInterest.hpp"
#include "Systems/Sets/setsPickInterest.hpp"
#include "Systems/Sets/setsSelectInterest.hpp"
#include "Systems/Sets/setsReportMemInterest.hpp"
#include "Systems/Sets/setsVisibleInterest.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

//	tools
#include "Support/mexp/mexpMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/gf/gfPaths.hpp"



//============================================================================
//============================================================================
namespace setsSystem
{
	namespace
	{
		setsPickInterest*		l_pSetsPI = 0;
		setsSelectInterest*		l_pSetsSI = 0;
		setsVisibleInterest*	l_pSetsVI = 0;
		setsNameInterest*		l_pSetsNI = 0;
		setsExportInterest*		l_pSetsEI = 0;
		setsDialogInterest*		l_pSetsDI = 0;
		setsReportMemInterest*	l_pSetsMI = 0;
	}

	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_Dir)
	{
		setsGeomList::Init( i_Dir, 
							itString(gfPaths::GetSubPath(gfPaths::e_Data)) );
		setsGeomList::SetAppDirectory( i_AppDir );

		// Initialize namespaces
		setsDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new setsDocumentInterest());
		setsCommands::SetupMenu();

		// register the pick interest
		if ( l_pSetsPI == 0 )
		{
			l_pSetsPI = new setsPickInterest();
			pick3dMgr::RegisterPickInterest( l_pSetsPI );
		}

		// register the select interest
		if ( l_pSetsSI == 0 )
		{
			l_pSetsSI = new setsSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pSetsSI );
		}

		// register the visible interest
		if ( l_pSetsVI == 0 )
		{
			l_pSetsVI = new setsVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pSetsVI );
		}

		// register the name interest
		if ( l_pSetsNI == 0 )
		{
			l_pSetsNI = new setsNameInterest();
			nameMgr::RegisterNameInterest( l_pSetsNI );
		}

		// register the export interest
		if ( l_pSetsEI == 0 )
		{
			l_pSetsEI = new setsExportInterest();
			mexpMgr::RegisterExportInterest( l_pSetsEI );
		}

		// register the dialog interest
		if ( l_pSetsDI == 0 )
		{
			l_pSetsDI = new setsDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pSetsDI );
		}

		// register the memory report interest
		if ( l_pSetsMI == 0 )
		{
			l_pSetsMI = new setsReportMemInterest();
			rptReportMemMgr::RegisterReportMemInterest( l_pSetsMI );
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pSetsPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pSetsPI );
			delete l_pSetsPI;
			l_pSetsPI = 0;
		}

		//	Unregister the select Interest
		if ( l_pSetsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pSetsSI );
			delete l_pSetsSI;
			l_pSetsSI = 0;
		}

		//	Unregister the visible Interest
		if ( l_pSetsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pSetsVI );
			delete l_pSetsVI;
			l_pSetsVI = 0;
		}

		// register the name interest
		if ( l_pSetsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pSetsNI );
			delete l_pSetsNI;
			l_pSetsNI = 0;
		}

		// register the export interest
		if ( l_pSetsEI != 0 )
		{
			mexpMgr::UnRegisterExportInterest( l_pSetsEI );
			delete l_pSetsEI;
			l_pSetsEI = 0;
		}

		// register the dialog interest
		if ( l_pSetsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pSetsDI );
			delete l_pSetsDI;
			l_pSetsDI = 0;
		}

		//	Unregister the Memory Report Interest
		if ( l_pSetsMI != 0 )
		{
			rptReportMemMgr::UnRegisterReportMemInterest( l_pSetsMI );
			delete l_pSetsMI;
			l_pSetsMI = 0;
		}

		// Clean up namespaces
		setsDialogUtil::CleanUp();
		setsGeomList::CleanUp();
	}

}	// end of namespace
