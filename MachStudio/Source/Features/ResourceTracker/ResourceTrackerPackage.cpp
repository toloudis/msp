/*****************************************************************************
**	ResourceTrackerPackage.cpp
**
**		see .hpp
**
**  Studio GPU
**  Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/ResourceTracker/ResourceTrackerPackage.hpp"

#include "Features/ResourceTracker/ResourceTrackerDialogUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

#include <string>


//============================================================================
//============================================================================
namespace ResourceTrackerPackage
{
	//------------------------------------------------------------------------
	//  Init() - Initialize the package
	//------------------------------------------------------------------------
	void  Init()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd = NULL;
		
		//	COMMAND: Render Log window
		pCmd = new cmaCommandSimple("Resource Tracker", 
									"Windows", 
									"Show the resources used for this scene",
									&ResourceTrackerDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Resource Tracker", pCmd );
		ResourceTrackerDialogUtil::Init();
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}
}
