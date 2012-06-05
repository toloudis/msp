/*****************************************************************************
**  envtSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/Environments/envtSystem.hpp"

#include "Systems/Environments/GUI/envtCommands.hpp"
#include "Systems/Environments/GUI/envtDialogInterest.hpp"
#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#include "Systems/Environments/GUI/envtContextMenuInterest.hpp"
#include "Systems/Environments/Data/envtDocumentInterest.hpp"
#include "Systems/Environments/Timeline/envtDriverCreator.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Object/envtSelectInterest.hpp"
#include "Systems/Environments/Object/envtReportMemInterest.hpp"
#include "Systems/Environments/Object/envtRendermanExportInterest.hpp"
#include "Systems/Environments/Object/envtMRayExportInterest.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

#include "Features/Reports/ReportMem/rptReportMemMgr.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
//#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
//#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Core/gf/gfPaths.hpp"
//#include "Core/name/nameMgr.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/rman/rmanMgr.hpp"
#include "Support/mray/mrayMgr.hpp"
#include "Support/vis/visMgr.hpp"

namespace envtSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<envtObjectMgr, envtEnvironmentObject> envtNameInterest;
		//typedef cmmPickInterestTemplate<envtObjectMgr> envtPickInterest;
		//typedef cmmVisibleInterestTemplate<envtObjectMgr> envtVisibleInterest;

		envtNameInterest*				l_pEnvironmentsNI = 0;
		//envtPickInterest*				l_pEnvironmentsPI = 0;
		envtSelectInterest*				l_pEnvironmentsSI = 0;
		//envtVisibleInterest*			l_pEnvironmentsVI = 0;
		envtDialogInterest*				l_pEnvironmentsDI = 0;
		envtReportMemInterest*			l_pEnvironmentsMI = 0;
		envtContextMenuInterest*		l_pEnvironmentsCMI = NULL;
		envtRendermanExportInterest*	l_pEnvironmentsREI = 0;
		envtMRayExportInterest*			l_pEnvironmentsMRayEI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_EnvtTexDir)
	{
		// assign directories for textures
		envtTextureList::Init( i_EnvtTexDir, 
			itString(gfPaths::GetSubPath(gfPaths::e_Textures)) );
		envtTextureList::SetAppDirectory( i_AppDir );

		// Initialize namespaces
		envtDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new envtDocumentInterest());
		//envtCommands::SetupMenu();

		// register the pick interest
		//if ( l_pEnvironmentsPI == 0 )
		//{
		//	l_pEnvironmentsPI = new envtPickInterest();
		//	pick3dMgr::RegisterPickInterest( l_pEnvironmentsPI );
		//}

		// register the select interest
		if ( l_pEnvironmentsSI == 0 )
		{
			l_pEnvironmentsSI = new envtSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pEnvironmentsSI );
		}

		// register the visible interest
		//if ( l_pEnvironmentsVI == 0 )
		//{
		//	l_pEnvironmentsVI = new envtVisibleInterest();
		//	visMgr::RegisterVisibleInterest( l_pEnvironmentsVI );
		//}

		// register the name interest
		if ( l_pEnvironmentsNI == 0 )
		{
			l_pEnvironmentsNI = new envtNameInterest();
			nameMgr::RegisterNameInterest( l_pEnvironmentsNI );
		}
		// register the dialog interest
		if ( l_pEnvironmentsDI == 0 )
		{
			l_pEnvironmentsDI = new envtDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pEnvironmentsDI );
		}
		// register the memory report interest
		if ( l_pEnvironmentsMI == 0 )
		{
			l_pEnvironmentsMI = new envtReportMemInterest();
			rptReportMemMgr::RegisterReportMemInterest( l_pEnvironmentsMI );
		}
		// register the context menu interest
		if ( l_pEnvironmentsCMI == 0 )
		{
			l_pEnvironmentsCMI = new envtContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pEnvironmentsCMI );
		}
		// register the renderman export interest
		if ( l_pEnvironmentsREI == 0 )
		{
			l_pEnvironmentsREI = new envtRendermanExportInterest();
			rmanMgr::RegisterExportInterest( l_pEnvironmentsREI );
		}
		// register the mray export interest
		if ( l_pEnvironmentsMRayEI == 0 )
		{
			l_pEnvironmentsMRayEI = new envtMRayExportInterest();
			mrayMgr::RegisterExportInterest( l_pEnvironmentsMRayEI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new envtDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		envtDriverCreator::CreateParsers();

		//	commands
		envtCommands::SetupMenu();

		envtObjectMgr::CreateDefaultEnvironment();
		envtObjectMgr::CreateSwlEnvironment();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		envtObjectMgr::DestroyDefaultEnvironment();
		envtObjectMgr::DestroySwlEnvironment();

		//	Unregister the Pick Interest
		//if ( l_pEnvironmentsPI != 0 )
		//{
		//	pick3dMgr::UnRegisterPickInterest( l_pEnvironmentsPI );
		//	delete l_pEnvironmentsPI;
		//	l_pEnvironmentsPI = 0;
		//}

		//	Unregister the Select Interest
		if ( l_pEnvironmentsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pEnvironmentsSI );
			delete l_pEnvironmentsSI;
			l_pEnvironmentsSI = 0;
		}

		//	Unregister the Visible Interest
		//if ( l_pEnvironmentsVI != 0 )
		//{
		//	visMgr::UnRegisterVisibleInterest( l_pEnvironmentsVI );
		//	delete l_pEnvironmentsVI;
		//	l_pEnvironmentsVI = 0;
		//}

		//	Unregister the Name Interest
		if ( l_pEnvironmentsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pEnvironmentsNI );
			delete l_pEnvironmentsNI;
			l_pEnvironmentsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pEnvironmentsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pEnvironmentsDI );
			delete l_pEnvironmentsDI;
			l_pEnvironmentsDI = 0;
		}

		//	Unregister the Memory Report Interest
		if ( l_pEnvironmentsMI != 0 )
		{
			rptReportMemMgr::UnRegisterReportMemInterest( l_pEnvironmentsMI );
			delete l_pEnvironmentsMI;
			l_pEnvironmentsMI = 0;
		}

		//	Unregister the context menu Interest
		if ( l_pEnvironmentsCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pEnvironmentsCMI );
			delete l_pEnvironmentsCMI;
			l_pEnvironmentsCMI = 0;
		}

		//	Unregister the Renderman Export Interest
		if ( l_pEnvironmentsREI != 0 )
		{
			rmanMgr::UnRegisterExportInterest( l_pEnvironmentsREI );
			delete l_pEnvironmentsREI;
			l_pEnvironmentsREI = 0;
		}

		//	Unregister the Renderman Export Interest
		if ( l_pEnvironmentsMRayEI != 0 )
		{
			mrayMgr::UnRegisterExportInterest( l_pEnvironmentsMRayEI );
			delete l_pEnvironmentsMRayEI;
			l_pEnvironmentsMRayEI = 0;
		}

		// Clean up namespaces
		envtDialogUtil::CleanUp();
		envtTextureList::CleanUp();

	}

}	// end of namespace
