/*****************************************************************************
**	cmraCommands.cpp
**
**	Sets up menu buttons for system cmra
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/GUI/cmraCommands.hpp"

#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace cmraCommands
{
	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//
		//	commands
		//
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		cmaCommand* pCmd = new cmaCommandToggle("Camera Icons", 
												"Icons", 
												"Toggle Icon Visibility",
												&cmraObjectMgr::ShowIcons, 
												&cmraObjectMgr::IconsVisible);
		int menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Cameras", "Toggle Icon Visibility", pCmd );

		//	COMMAND: edit camera configuration
		//cmaCommand* pCmd = new cmaCommandSimple("Editor Cam Config", 
		//										"Tools", 
		//										"Configure the Edit cam",
		//										&cmraDialogUtil::ShowEditorCameraConfiguration);
		//menu_id = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Cameras", ""Editor Cam Config", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Position Spline", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Target Spline", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Target Object", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Camera Key", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Camera Key Position", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Camera Key Target", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Capture", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "FOV", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Tilt", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Circle", "Cameras" );
		chnlCommandUtil::CreateDriverButton( "Follow", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Record Tilt", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Record FOV", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Framed Arc", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Framed Close-Up", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Framed Medium-Long", "Cameras" );
		//chnlCommandUtil::CreateDriverButton( "Framed Overhead", "Cameras" );
	}

	//--------------------------------------------------------------------
	// CleanUp 
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
