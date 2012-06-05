/*****************************************************************************
**  mtrlPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Support/mtrl/mtrlPackage.hpp"

#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/mtrlCommands.hpp"
#include "Support/mtrl/mtrlDriverCreator.hpp"
#include "Support/mtrl/mtrlHighlight.hpp"
#include "Support/mtrl/mtrlIconUtil.hpp"
#include "Support/mtrl/mtrlSelectInterest.hpp"

#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"


//============================================================================
//============================================================================
namespace mtrlPackage
{
	namespace
	{
		mtrlSelectInterest*			l_pMaterialsSI = NULL;
		mtrlContextMenuInterest*	l_pMaterialsCMI = NULL;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		mtrlDialogUtil::Init();
		mtrlHighlight::Initialize();

		// register the select interest
		if ( l_pMaterialsSI == 0 )
		{
			l_pMaterialsSI = new mtrlSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pMaterialsSI );
		}

		// register the context menu interest
		if ( l_pMaterialsCMI == 0 )
		{
			l_pMaterialsCMI = new mtrlContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pMaterialsCMI );
		}

		// Timeline related creators
		tmlnCreator::AddDriverCreator(new mtrlDriverCreator);
		mtrlDriverCreator::CreateParsers();

		// Create commands to enable hot keys
		mtrlCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pMaterialsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pMaterialsSI );
			delete l_pMaterialsSI;
			l_pMaterialsSI = 0;
		}

		//	Unregister the context menu Interest
		if ( l_pMaterialsCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pMaterialsCMI );
			delete l_pMaterialsCMI;
			l_pMaterialsCMI = 0;
		}

		// Clean up namespaces
		mtrlHighlight::DeInitialize();
		mtrlDialogUtil::CleanUp();
		mtrlIconUtil::cleanUpTemplate();
	}

}	// end of namespace