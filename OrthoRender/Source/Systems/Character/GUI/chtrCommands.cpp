/*****************************************************************************
**  chtrCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Character/GUI/chtrCommands.hpp"

#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#include "Systems/Character/GUI/chtrCommandSubdivLevel.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace chtrCommands
{
	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Characters" );

		//
		//	commands
		//
		cmaCommand* pCmd;
		int menu_id;
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		pCmd = new cmaCommandToggle("Character Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&chtrObjectMgr::ShowIcons, 
									&chtrObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Characters", "Toggle Icon Visibility", pCmd );

		//	COMMAND: subdiv
		guiMenuMgr::AddMenu( "View", "Subdivision Level" );
		int level0 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Base Mesh" );
		chtrCommandSubdivLevel *cmd0 = new chtrCommandSubdivLevel(0);
		guiCommandMgr::Add( cmd0, cmd0->GetTag(), level0 );
		cmmSystemDialogUtil::AddSystemCommand( "Characters", "Subdiv - Base Mesh", cmd0 );

		int level1 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 1" );
		chtrCommandSubdivLevel *cmd1 = new chtrCommandSubdivLevel(1);
		guiCommandMgr::Add( cmd1, cmd1->GetTag(), level1 );
		cmmSystemDialogUtil::AddSystemCommand( "Characters", "Subdiv - Level 1", cmd1 );

		int level2 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 2" );
		chtrCommandSubdivLevel *cmd2 = new chtrCommandSubdivLevel(2);
		guiCommandMgr::Add( cmd2, cmd2->GetTag(), level2 );
		cmmSystemDialogUtil::AddSystemCommand( "Characters", "Subdiv - Level 2", cmd2 );

		int level3 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 3" );
		chtrCommandSubdivLevel *cmd3 = new chtrCommandSubdivLevel(3);
		guiCommandMgr::Add( cmd3, cmd3->GetTag(), level3 );
		cmmSystemDialogUtil::AddSystemCommand( "Characters", "Subdiv - Level 3", cmd3 );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Anim-Full", "Characters" );
		//chnlCommandUtil::CreateDriverButton( "SubAnim", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Play Sound", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Visible", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Spline", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Static Orientation", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Static Orientation Euler", "Characters" );
		chnlCommandUtil::CreateDriverButton( "Static Oriented", "Characters" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
