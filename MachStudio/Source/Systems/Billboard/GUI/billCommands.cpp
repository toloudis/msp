/*****************************************************************************
**  billCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/GUI/billCommands.hpp"

#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Billboard/Undo/billOperations.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace billCommands
{
	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Billboards" );

		//
		//	commands
		//
		//	COMMAND: Create -> Billboard
		cmaCommand* pCmd = new cmaCommandSimple("Billboard", 
												"Create", 
												"Creates new billboard object",
												&billOperations::AddObject );
		int menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		guiMenuMgr::AddMenu( "View", "Icons" );
		//	COMMAND: view icons
		pCmd = new cmaCommandToggle("Billboard Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&billObjectMgr::ShowIcons, 
									&billObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Billboards", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Billboards" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Billboards" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Billboards" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
