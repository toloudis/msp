/*****************************************************************************
**  sbrdCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/GUI/sbrdCommands.hpp"

#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace sbrdCommands
{
	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Storyboards" );

		//
		//	commands
		//
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		cmaCommand* pCmd = new cmaCommandToggle("Storyboard Icons", 
												"Icons", 
												"Toggle Icon Visibility",
												&sbrdObjectMgr::ShowIcons, 
												&sbrdObjectMgr::IconsVisible);
		int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Storyboards", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Storyboards" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Storyboards" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Storyboards" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
