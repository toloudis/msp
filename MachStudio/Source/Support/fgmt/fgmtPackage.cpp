/*****************************************************************************
**  fgmtPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtPackage.hpp"

#include "Support/fgmt/fgmtCommands.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/fgmtSelectInterest.hpp"

#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"



//============================================================================
//============================================================================
namespace fgmtPackage
{
	namespace
	{
		fgmtSelectInterest*	l_pFragmentsSI = 0;
		fgmtContextMenuInterest*	l_pFragmentCMI = 0;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(g2dSystem* i_pSystem)
	{
		// Initialize namespaces
		fgmtDialogUtil::Init(i_pSystem);
		fgmtHighlight::Initialize();

		// register the select interest
		if ( l_pFragmentsSI == 0 )
		{
			l_pFragmentsSI = new fgmtSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pFragmentsSI );
		}
		// register the context menu interest
		if ( l_pFragmentCMI == 0 )
		{
			l_pFragmentCMI = new fgmtContextMenuInterest();
			ctxmContextMenu::RegisterInterest( l_pFragmentCMI );
		}

		// Timeline related creators
		//tmlnCreator::AddDriverCreator(new fgmtDriverCreator);
		//fgmtDriverCreator::CreateParsers();

		// Create commands to enable hot keys
		fgmtCommands::SetupMenu();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//	Unregister the Select Interest
		if ( l_pFragmentsSI != 0 )
		{
			sel3dMgr::UnRegisterSelectInterest( l_pFragmentsSI );
			delete l_pFragmentsSI;
			l_pFragmentsSI = 0;
		}
		//	Unregister the context menu Interest
		if ( l_pFragmentCMI != 0 )
		{
			ctxmContextMenu::UnRegisterInterest( l_pFragmentCMI );
			delete l_pFragmentCMI;
			l_pFragmentCMI = 0;
		}

		// Clean up namespaces
		fgmtHighlight::DeInitialize();
		fgmtDialogUtil::CleanUp();
		fgmtOperations::CleanUp();
	}

}	// end of namespace