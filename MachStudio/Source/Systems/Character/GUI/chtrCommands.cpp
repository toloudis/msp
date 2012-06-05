/*****************************************************************************
**  chtrCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrCommands.hpp"
#include "Systems/Character/GUI/chtrCommandSubdivLevel.hpp"
#include "Systems/Character/GUI/chtrDialogUtil.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


namespace
{
	//--------------------------------------------------------------------
	// Smoothing subdivisions on/off
	//--------------------------------------------------------------------
	void Set_Smoothing(bool i_bSmoothing)
	{
		// Toggle between levels 0 and 1
		int level = (api3dSubdiv::GetSubdivLevel() == 0) ? 1 : 0;
		api3dSubdiv::SetSubdivLevel(level);

		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Subdiv, ((level == 0) ? "" : "Smooth") );
		std::string tool_tip("Subdivision Level: ");
		tool_tip += ((level == 0) ? "Off" : "On");
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Subdiv, tool_tip.c_str());

	}
	bool Get_Smoothing()
	{
		return (api3dSubdiv::GetSubdivLevel() != 0);
	}
}

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
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Objects" );

		//
		//	commands
		//
		cmaCommand* pCmd;
		int menu_id;

		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		pCmd = new cmaCommandToggle("Object Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&chtrObjectMgr::ShowIcons, 
									&chtrObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Objects", "Toggle Icon Visibility", pCmd );

		//bga -Smoothing toggle replaces Sudivision level submenu
		//	COMMAND: Smoothing
		pCmd = new cmaCommandToggle("Smoothing", 
									"View", 
									"Toggle subdivision surface smoothing",
									&Set_Smoothing, 
									&Get_Smoothing);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: subdiv
		//guiMenuMgr::AddMenu( "View", "Subdivision Level" );
		//int level0 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Base Mesh" );
		//chtrCommandSubdivLevel *cmd0 = new chtrCommandSubdivLevel(0);
		//guiCommandMgr::Add( cmd0, cmd0->GetTag(), level0 );
		//cmmSystemDialogUtil::AddSystemCommand( "Objects", "Subdiv - Base Mesh", cmd0 );

		//int level1 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 1" );
		//chtrCommandSubdivLevel *cmd1 = new chtrCommandSubdivLevel(1);
		//guiCommandMgr::Add( cmd1, cmd1->GetTag(), level1 );
		//cmmSystemDialogUtil::AddSystemCommand( "Objects", "Subdiv - Level 1", cmd1 );

		//int level2 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 2" );
		//chtrCommandSubdivLevel *cmd2 = new chtrCommandSubdivLevel(2);
		//guiCommandMgr::Add( cmd2, cmd2->GetTag(), level2 );
		//cmmSystemDialogUtil::AddSystemCommand( "Objects", "Subdiv - Level 2", cmd2 );

		//int level3 = guiMenuMgr::AddCheckableMenuItem( "Subdivision Level", "Level 3" );
		//chtrCommandSubdivLevel *cmd3 = new chtrCommandSubdivLevel(3);
		//guiCommandMgr::Add( cmd3, cmd3->GetTag(), level3 );
		//cmmSystemDialogUtil::AddSystemCommand( "Objects", "Subdiv - Level 3", cmd3 );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Anim-Full", "Objects" );
		//chnlCommandUtil::CreateDriverButton( "SubAnim", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Play Sound", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Visible", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Spline", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Static Orientation", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Static Orientation Euler", "Objects" );
		chnlCommandUtil::CreateDriverButton( "Static Oriented", "Objects" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
