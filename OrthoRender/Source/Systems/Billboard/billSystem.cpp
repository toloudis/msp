/*****************************************************************************
**  billSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/billSystem.hpp"

#include "Systems/Billboard/GUI/billCommands.hpp"
#include "Systems/Billboard/GUI/billDialogUtil.hpp"
#include "Systems/Billboard/GUI/billDialogInterest.hpp"
#include "Systems/Billboard/Data/billDocumentInterest.hpp"
#include "Systems/Billboard/Timeline/billDriverCreator.hpp"
#include "Systems/Billboard/GUI/billGeomList.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Object/billSelectInterest.hpp"

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


namespace billSystem
{

namespace
{
	// Interests implemented through templates
	typedef cmmNameInterestTemplate<billObjectMgr, billBillboardObject> billNameInterest;
	typedef cmmPickInterestTemplate<billObjectMgr> billPickInterest;
	typedef cmmVisibleInterestTemplate<billObjectMgr> billVisibleInterest;

	billPickInterest*	l_pBillboardPI = 0;
	billSelectInterest*	l_pBillboardSI = 0;
	billNameInterest*	l_pBillboardNI = 0;
	billVisibleInterest*	l_pBillboardVI = 0;
	billDialogInterest* l_pBillboardDI = 0;
}

	//--------------------------------------------------------------------
	// Init -- initialize system with directory to use to look for
	//		geometry files
	//--------------------------------------------------------------------
	void Init(const fsLocator& i_AppDir, const itString& i_BillboardDataDir)
	{
		billGeomList::Init( i_BillboardDataDir, itString(gfPaths::GetSubPath(gfPaths::e_Textures)) );
		billGeomList::SetAppDirectory(i_AppDir);

		// Initialize namespaces
		billDialogUtil::Init();
		billObjectMgr::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new billDocumentInterest());

		// register the pick interest
		if ( l_pBillboardPI == 0 )
		{
			l_pBillboardPI = new billPickInterest();
			pick3dMgr::RegisterPickInterest( l_pBillboardPI );
		}

		// register the select interest
		if ( l_pBillboardSI == 0 )
		{
			l_pBillboardSI = new billSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pBillboardSI );
		}

		// register the name interest
		if ( l_pBillboardNI == 0 )
		{
			l_pBillboardNI = new billNameInterest();
			nameMgr::RegisterNameInterest( l_pBillboardNI );
		}

		// register the visible interest
		if ( l_pBillboardVI == 0 )
		{
			l_pBillboardVI = new billVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pBillboardVI );
		}

		// register the dialog interest
		if ( l_pBillboardDI == 0 )
		{
			l_pBillboardDI = new billDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pBillboardDI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new billDriverCreator;
		tmlnCreator::AddDriverCreator(pDriverCreator);
		billDriverCreator::CreateParsers();

		//	commands
//WXGUI
/*
		billCommands::SetupMenu();
*/
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Pick Interest
		if ( l_pBillboardPI != 0 )
		{
			pick3dMgr::UnRegisterPickInterest( l_pBillboardPI );
			delete l_pBillboardPI;
			l_pBillboardPI = 0;
		}

		//	Unregister the Pick Interest
		if ( l_pBillboardSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pBillboardSI );
			delete l_pBillboardSI;
			l_pBillboardSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pBillboardNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pBillboardNI );
			delete l_pBillboardNI;
			l_pBillboardNI = 0;
		}

		//	Unregister the Visible Interest
		if ( l_pBillboardVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pBillboardVI );
			delete l_pBillboardVI;
			l_pBillboardVI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pBillboardDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pBillboardDI );
			delete l_pBillboardDI;
			l_pBillboardDI = 0;
		}

		// Clean up namespaces
		billCommands::CleanUp();
		billObjectMgr::CleanUp();
		billDialogUtil::CleanUp();
		billGeomList::CleanUp();
	}

}	// end of namespace
