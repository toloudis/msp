/*****************************************************************************
**  prjltSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/prjltSystem.hpp"

#include "Systems/PrjLt/GUI/prjltCommands.hpp"
#include "Systems/PrjLt/GUI/prjltDialogInterest.hpp"
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#include "Systems/PrjLt/Data/prjltDocumentInterest.hpp"
#include "Systems/PrjLt/Timeline/prjltDriverCreator.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Object/prjltReportMemInterest.hpp"
#include "Systems/PrjLt/Object/prjltRendermanExportInterest.hpp"
#include "Systems/PrjLt/Object/prjltMRayExportInterest.hpp"
#include "Systems/PrjLt/Object/prjltSelectInterest.hpp"
#include "Systems/PrjLt/GUI/prjltTextureList.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/vis/visMgr.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"

#include "Core/gf/gfPaths.hpp"
//#include "Core/name/nameMgr.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiContextMenu.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"

#include <boost/bind.hpp>

//============================================================================
//============================================================================
namespace prjltSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<prjltObjectMgr, prjltProjectedLightObject> prjltNameInterest;
		typedef cmmPickInterestTemplate<prjltObjectMgr> prjltPickInterest;
		typedef cmmVisibleInterestTemplate<prjltObjectMgr> prjltVisibleInterest;

		prjltNameInterest*				l_pProjectedLightsNI = 0;
		prjltPickInterest*				l_pProjectedLightsPI = 0;
		prjltSelectInterest*			l_pProjectedLightsSI = 0;
		prjltVisibleInterest*			l_pProjectedLightsVI = 0;
		prjltDialogInterest*			l_pProjectedLightsDI = 0;
		prjltReportMemInterest*			l_pProjectedLightsMI = 0;
		prjltRendermanExportInterest*	l_pProjectedLightsREI = 0;
		prjltMRayExportInterest*		l_pProjectedLightsMRayEI = 0;
		
		//============================================================================
		//============================================================================
		class prjltContextMenuInterest : public ctxmContextMenuInterest
		{
			public:
				//----------------------------------------------------------------------------
				// Add commands to the context menu based on what was selected.
				// i_pChosenObject and i_pPickInfo might be NULL.
				//----------------------------------------------------------------------------
				virtual void AddToContextMenu(guiContextMenu &io_ContextMenu,
											  const sel3dObject* i_pChosenObject,
											  const g3dPickInfo* i_pPickInfo) const
				{
					io_ContextMenu.AddMenu("Lighting", "");
					io_ContextMenu.AddMenuItem("Lighting", 
						"Create Projected Light at Camera Position", 
						boost::bind(&prjltOperations::AddLightAtCameraPosition,
									prjltData::e_ProjectedLight));
				}
		};
		prjltContextMenuInterest*	l_pProjectedLightsCMI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_ProjTexDir)
	{
		// assign directories for textures
		prjltTextureList::Init( i_ProjTexDir, itString(gfPaths::GetSubPath(gfPaths::e_Textures)) );
		prjltTextureList::SetAppDirectory( i_AppDir );

		// Initialize namespaces
		prjltDialogUtil::Init();
		//prjltObjectMgr::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new prjltDocumentInterest());
		prjltCommands::SetupMenu();

		// register the pick interest
		if ( l_pProjectedLightsPI == 0 )
		{
			l_pProjectedLightsPI = new prjltPickInterest();
			pick3dMgr::RegisterPickInterest( l_pProjectedLightsPI );
		}

		// register the select interest
		if ( l_pProjectedLightsSI == 0 )
		{
			l_pProjectedLightsSI = new prjltSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pProjectedLightsSI );
		}

		// register the visible interest
		if ( l_pProjectedLightsVI == 0 )
		{
			l_pProjectedLightsVI = new prjltVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pProjectedLightsVI );
		}

		// register the name interest
		if ( l_pProjectedLightsNI == 0 )
		{
			l_pProjectedLightsNI = new prjltNameInterest();
			nameMgr::RegisterNameInterest( l_pProjectedLightsNI );
		}

		// register the dialog interest
		if ( l_pProjectedLightsDI == 0 )
		{
			l_pProjectedLightsDI = new prjltDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pProjectedLightsDI );
		}

		// register the memory report interest
		if ( l_pProjectedLightsMI == 0 )
		{
			l_pProjectedLightsMI = new prjltReportMemInterest();
			rptReportMemMgr::RegisterReportMemInterest( l_pProjectedLightsMI );
		}

		// register the rman interest
		if ( l_pProjectedLightsREI == 0 )
		{
			l_pProjectedLightsREI = new prjltRendermanExportInterest();
			rmanMgr::RegisterExportInterest( l_pProjectedLightsREI );
		}

		// register the mray interest
		if ( l_pProjectedLightsMRayEI == 0 )
		{
			l_pProjectedLightsMRayEI = new prjltMRayExportInterest();
			mrayMgr::RegisterExportInterest( l_pProjectedLightsMRayEI );
		}

		// register the context menu interest
		if ( l_pProjectedLightsCMI == 0 )
		{
			l_pProjectedLightsCMI = new prjltContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pProjectedLightsCMI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new prjltDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		prjltDriverCreator::CreateParsers();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pProjectedLightsPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pProjectedLightsPI );
			delete l_pProjectedLightsPI;
			l_pProjectedLightsPI = 0;
		}

		//	Unregister the Select Interest
		if ( l_pProjectedLightsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pProjectedLightsSI );
			delete l_pProjectedLightsSI;
			l_pProjectedLightsSI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pProjectedLightsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pProjectedLightsVI );
			delete l_pProjectedLightsVI;
			l_pProjectedLightsVI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pProjectedLightsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pProjectedLightsNI );
			delete l_pProjectedLightsNI;
			l_pProjectedLightsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pProjectedLightsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pProjectedLightsDI );
			delete l_pProjectedLightsDI;
			l_pProjectedLightsDI = 0;
		}

		//	Unregister the Memory Report Interest
		if ( l_pProjectedLightsMI != 0 )
		{
			rptReportMemMgr::UnRegisterReportMemInterest( l_pProjectedLightsMI );
			delete l_pProjectedLightsMI;
			l_pProjectedLightsMI = 0;
		}

		//	Unregister the Renderman Export Interest
		if ( l_pProjectedLightsREI != 0 )
		{
			rmanMgr::UnRegisterExportInterest( l_pProjectedLightsREI );
			delete l_pProjectedLightsREI;
			l_pProjectedLightsREI = 0;
		}

		//	Unregister the mray Export Interest
		if ( l_pProjectedLightsMRayEI != 0 )
		{
			mrayMgr::UnRegisterExportInterest( l_pProjectedLightsMRayEI );
			delete l_pProjectedLightsMRayEI;
			l_pProjectedLightsMRayEI = 0;
		}

		//	Unregister the context menu Interest
		if ( l_pProjectedLightsCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pProjectedLightsCMI );
			delete l_pProjectedLightsCMI;
			l_pProjectedLightsCMI = 0;
		}

		// Clean up namespaces
		prjltCommands::CleanUp();
		//prjltObjectMgr::CleanUp();
		prjltDialogUtil::CleanUp();
		prjltTextureList::CleanUp();
	}

}	// end of namespace