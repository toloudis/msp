/*****************************************************************************
**  billCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/GUI/billCommands.hpp"

#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
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
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		cmaCommand* pCmd = new cmaCommandToggle("Billboard Icons", 
												"Icons", 
												"Toggle Icon Visibility",
												&billObjectMgr::ShowIcons, 
												&billObjectMgr::IconsVisible);
		int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
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
