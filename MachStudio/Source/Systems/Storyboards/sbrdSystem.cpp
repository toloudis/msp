/*****************************************************************************
**  sbrdSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/sbrdSystem.hpp"

#include "Systems/Storyboards/GUI/sbrdCommands.hpp"
#include "Systems/Storyboards/GUI/sbrdDialogUtil.hpp"
#include "Systems/Storyboards/GUI/sbrdDialogInterest.hpp"
#include "Systems/Storyboards/Data/sbrdDocumentInterest.hpp"
#include "Systems/Storyboards/Timeline/sbrdDriverCreator.hpp"
#include "Systems/Storyboards/GUI/sbrdGeomList.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/Object/sbrdSelectInterest.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmPickInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

//	tools
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/name/nameMgr.hpp"
#include "Tool/pick3d/pick3dMgr.hpp"
#include "Support/vis/visMgr.hpp"

//	lib
#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfPaths.hpp"


//============================================================================
//============================================================================
namespace sbrdSystem
{

namespace
{
	// Interests implemented through templates
	typedef cmmNameInterestTemplate<sbrdObjectMgr, sbrdBillboardObject> sbrdNameInterest;
	typedef cmmPickInterestTemplate<sbrdObjectMgr> sbrdPickInterest;
	typedef cmmVisibleInterestTemplate<sbrdObjectMgr> sbrdVisibleInterest;

	sbrdPickInterest*		l_pStoryboardPI = 0;
	sbrdSelectInterest*		l_pStoryboardSI = 0;
	sbrdNameInterest*		l_pStoryboardNI = 0;
	sbrdVisibleInterest*	l_pStoryboardVI = 0;
	sbrdDialogInterest*		l_pStoryboardDI = 0;

}

	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_BillboardDataDir)
	{
		/*sbrdGeomList::Init( i_BillboardDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Textures)) );
		sbrdGeomList::SetAppDirectory(i_AppDir);*/

		// Initialize namespaces
		/*sbrdDialogUtil::Init();
		sbrdObjectMgr::Init();*/

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new sbrdDocumentInterest());

		// register the pick interest
		if ( l_pStoryboardPI == 0 )
		{
			l_pStoryboardPI = new sbrdPickInterest();
			pick3dMgr::RegisterPickInterest( l_pStoryboardPI );
		}

		// register the select interest
		if ( l_pStoryboardSI == 0 )
		{
			l_pStoryboardSI = new sbrdSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pStoryboardSI );
		}

		// register the name interest
		if ( l_pStoryboardNI == 0 )
		{
			l_pStoryboardNI = new sbrdNameInterest();
			nameMgr::RegisterNameInterest( l_pStoryboardNI );
		}

		// register the visible interest
		if ( l_pStoryboardVI == 0 )
		{
			l_pStoryboardVI = new sbrdVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pStoryboardVI );
		}

		// register the dialog interest
		/*if ( l_pStoryboardDI == 0 )
		{
			l_pStoryboardDI = new sbrdDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pStoryboardDI );
		}*/

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new sbrdDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		sbrdDriverCreator::CreateParsers();

		//	commands
		//sbrdCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pStoryboardPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pStoryboardPI );
			delete l_pStoryboardPI;
			l_pStoryboardPI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pStoryboardSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pStoryboardSI );
			delete l_pStoryboardSI;
			l_pStoryboardSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pStoryboardNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pStoryboardNI );
			delete l_pStoryboardNI;
			l_pStoryboardNI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pStoryboardVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pStoryboardVI );
			delete l_pStoryboardVI;
			l_pStoryboardVI = 0;
		}

		//	Unregister the Dialog Interest
		/*if ( l_pStoryboardDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pStoryboardDI );
			delete l_pStoryboardDI;
			l_pStoryboardDI = 0;
		}*/

		// Clean up namespaces
		/*sbrdCommands::CleanUp();
		sbrdObjectMgr::CleanUp();
		sbrdDialogUtil::CleanUp();
		sbrdGeomList::CleanUp();*/
	}

}	// end of namespace
