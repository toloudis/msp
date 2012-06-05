/*****************************************************************************
**	ptltCommands.cpp
**
**	Sets up menu buttons for system ptlt
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/GUI/ptltCommands.hpp"

#include "Systems/PtLt/GUI/ptltCreateFillLight.h"
#include "Systems/PtLt/Timeline/ptltDriverCreator.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"
#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Features/Channels/chnlCommandUtil.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Drivers/Attach/tmlnDriverAttachInfo.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"


//============================================================================
//============================================================================
namespace ptltCommands
{
	//
	//	Command Functions
	//
	namespace
	{
		const int c_NumLights = 4;
		const char* c_FillNames[] = {
			"FILL_%s_LEFT", "FILL_%s_RIGHT", "FILL_%s_FRONT", "FILL_%s_BACK"
		};
		maVector3d c_Offsets[] = {
			maVector3d(-1, 0.25, 0), maVector3d(1, 0.25, 0), maVector3d(0, 0.25, -1), maVector3d(0, 0.25, 1)
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateFillRing()
		{
		}
	}


	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Create -> Point Light
		pCmd = new cmaCommandSimple("Point Light", 
									"Create", 
									"Creates new point light",
									&ptltOperations::AddObject );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Create -> Point Light At Camera Position
		pCmd = new cmaCommandSimple("Point Light at Camera Position", 
									"Create", 
									"Creates new point light at current camera position",
									&ptltOperations::AddLightAtCameraPosition );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		const char* lc_SYSTEMNAME = "Point Light";
		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Point Light" );

		//
		//	Commands
		//
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icon
		pCmd = new cmaCommandToggle("Point Light Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&ptltObjectMgr::ShowIcons, 
									&ptltObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Point Lights", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Enabled", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "Color", "Point Light" );
		chnlCommandUtil::CreateDriverButton( "ColorFlicker", "Point Light" );
	}


	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
