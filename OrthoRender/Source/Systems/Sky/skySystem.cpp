/*****************************************************************************
**  skySystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "skySystem.hpp"

#include "skyCommands.hpp"
#include "skyDialogUtil.hpp"
#include "skyDocumentInterest.hpp"
#include "skyThinkInterest.hpp"

#include "mnmThinkMgr.hpp"

#include "api3dScene.hpp"
#include "daySkyMgr.hpp"
#include "docSingleTypeMgr.hpp"
#include "gfPaths.hpp"


namespace skySystem
{
	namespace
	{	
		skyThinkInterest*		l_pSkyTI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		skyDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest( new skyDocumentInterest() );
		skyCommands::SetupMenu();

		//	create the sky layer.
		//
		int sceneroot_index;
		sceneroot_index = api3dScene::AddLayer( g3dLayer::e_ZBuffer,
												g3dLayer::e_World, g3dLayer::e_Additive,
												false, false, false );

		//
		// Directory for sky domes
		fsLocator sky_dir = gfPaths::e_AppPath;
		sky_dir.Push("Data");
		sky_dir.Push("Sets");
		sky_dir.Push(gfPaths::GetSubPath(gfPaths::e_Models));

		// Initialize systems here
		daySkyMgr::Initialize( sky_dir );
		daySkyMgr::SetParentNode( api3dScene::GetRoot( sceneroot_index ) );

		// register the think interest
		if ( l_pSkyTI == 0 )
		{
			l_pSkyTI = new skyThinkInterest();
			mnmThinkMgr::RegisterThinkInterest( l_pSkyTI );
		}
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		daySkyMgr::DeInitialize();

		// Clean up namespaces
		skyDialogUtil::CleanUp();
		
		//	Unregister the Think Interest
		if ( l_pSkyTI != 0 )
		{
			mnmThinkMgr::UnRegisterThinkInterest( l_pSkyTI );
			delete l_pSkyTI;
			l_pSkyTI = 0;
		}
	}

}	// end of namespace