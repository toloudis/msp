/*****************************************************************************
**  trfnSystem.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/trfnSystem.hpp"
#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"
#include "Systems/Transforms/GUI/trfnContextMenuInterest.hpp"
#include "Systems/Transforms/GUI/trfnCommands.hpp"
#include "Systems/Transforms/GUI/trfnDialogInterest.hpp"
#include "Systems/Transforms/GUI/trfnPython.hpp"
#include "Systems/Transforms/Object/trfnSelectInterest.hpp"
#include "Systems/Transforms/Timeline/trfnDriverCreator.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/Templates/cmmNameInterestTemplate.hpp"
#include "Systems/Common/Templates/cmmVisibleInterestTemplate.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/gf/gfPaths.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/doc/docDocumentInterest.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"

namespace trfnSystem
{
	namespace
	{
		// Interests implemented through templates
		typedef cmmNameInterestTemplate<trfnObjectMgr, trfnTransformObject> trfnNameInterest;
		typedef cmmVisibleInterestGeomTemplate<trfnObjectMgr> trfnVisibleInterest;

		trfnNameInterest*		l_pTransformsNI = 0;
		trfnSelectInterest*		l_pTransformsSI = 0;
		trfnDialogInterest*		l_pTransformsDI = 0;
		trfnVisibleInterest*	l_pTransformsVI = 0;
		trfnContextMenuInterest*	l_pTransformsCMI = NULL;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		//trfnDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new docSimpleDocumentInterest<trfnDocumentChunk>());
		trfnCommands::SetupMenu();

		// register the select interest
		if ( l_pTransformsSI == 0 )
		{
			l_pTransformsSI = new trfnSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pTransformsSI );
		}

		// register the name interest
		if ( l_pTransformsNI == 0 )
		{
			l_pTransformsNI = new trfnNameInterest();
			nameMgr::RegisterNameInterest( l_pTransformsNI );
		}
		// register the dialog interest
		if ( l_pTransformsDI == 0 )
		{
			l_pTransformsDI = new trfnDialogInterest();
			cmmDialogInterestMgr::RegisterInterest( l_pTransformsDI );
		}

		// register the visible interest
		if ( l_pTransformsVI == 0 )
		{
			l_pTransformsVI = new trfnVisibleInterest();
			visMgr::RegisterVisibleInterest( l_pTransformsVI );
		}

		// register the context menu interest
		if ( l_pTransformsCMI == 0 )
		{
			l_pTransformsCMI = new trfnContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pTransformsCMI );
		}

		// Timeline related creators
		tmlnDriverCreator* pDriverCreator = new trfnDriverCreator();
		tmlnCreator::AddDriverCreator(pDriverCreator);
		trfnDriverCreator::CreateParsers();

		// Python
		trfnPython::AddCommands("mach");

	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Visible Interest
		if ( l_pTransformsVI != 0 )
		{
			visMgr::UnRegisterVisibleInterest( l_pTransformsVI );
			delete l_pTransformsVI;
			l_pTransformsVI = 0;
		}

		//	Unregister the Select Interest
		if ( l_pTransformsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pTransformsSI );
			delete l_pTransformsSI;
			l_pTransformsSI = 0;
		}

		//	Unregister the Name Interest
		if ( l_pTransformsNI != 0 )
		{
			nameMgr::UnRegisterNameInterest( l_pTransformsNI );
			delete l_pTransformsNI;
			l_pTransformsNI = 0;
		}

		//	Unregister the Dialog Interest
		if ( l_pTransformsDI != 0 )
		{
			cmmDialogInterestMgr::UnRegisterInterest( l_pTransformsDI );
			delete l_pTransformsDI;
			l_pTransformsDI = 0;
		}

		//	Unregister the context menu Interest
		if ( l_pTransformsCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pTransformsCMI );
			delete l_pTransformsCMI;
			l_pTransformsCMI = 0;
		}

		// Clean up namespaces
		//trfnDialogUtil::CleanUp();

	}

}	// end of namespace
