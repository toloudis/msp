/*****************************************************************************
**  fgmtPackage.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/

#include "Support/fgmt/fgmtPackage.hpp"

#include "Support/fgmt/fgmtCommands.hpp"
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/fgmtSelectInterest.hpp"

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
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init(g2dSystem* i_pSystem)
	{
		// Initialize namespaces
		fgmtDialogUtil::Init(i_pSystem);

		// register the select interest
		if ( l_pFragmentsSI == 0 )
		{
			l_pFragmentsSI = new fgmtSelectInterest();
			sel3dMgr::RegisterSelectInterest( l_pFragmentsSI );
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

		// Clean up namespaces
		fgmtDialogUtil::CleanUp();
	}

}	// end of namespace