/*****************************************************************************
**  propCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/GUI/propCommands.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace propCommands
{
	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Props" );

		//
		//	commands
		//
		//guiMenuMgr::AddMenu( "View", "Icons" );

		////	COMMAND: view icons
		//cmaCommand* pCmd = new cmaCommandToggle("Prop Icons", 
		//										"Icons", 
		//										"Toggle Icon Visibility",
		//										&propObjectMgr::ShowIcons, 
		//										&propObjectMgr::IconsVisible);
		//int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	add the system command(s)
		//cmmSystemDialogUtil::AddSystemCommand( "Props", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		//chnlCommandUtil::CreateDriverButton( "Spline", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Attach", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Static Position", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Static Orientation", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Static Orientation Euler", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Spline Oriented", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Visible", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Anim-Full", "Props" );
		//chnlCommandUtil::CreateDriverButton( "SubAnim", "Props" );
		//chnlCommandUtil::CreateDriverButton( "Play Sound", "Props" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
