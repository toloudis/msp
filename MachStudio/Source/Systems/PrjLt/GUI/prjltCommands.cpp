/*****************************************************************************
**	prjltCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/GUI/prjltCommands.hpp"

#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#include "Systems/PrjLt/Timeline/prjltDriverCreator.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

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
namespace prjltCommands
{
	//
	//	Command Functions
	//
	namespace
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateKeyLight()
		{

		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateProjectedLightAtView()
		{
			prjltOperations::AddLightAtCameraPosition(prjltData::e_ProjectedLight);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateSpotLight()
		{
			prjltOperations::AddObject(prjltData::e_SpotLight);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateSpotLightAtView()
		{
			prjltOperations::AddLightAtCameraPosition(prjltData::e_SpotLight);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_CreateDirectionalLight()
		{
			prjltOperations::AddLightAtCameraPosition(prjltData::e_DirectionalLight);
		}
	}

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar and tabs
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Create -> Projected Light
		pCmd = new cmaCommandSimple("Projected Light", 
									"Create", 
									"Creates new projected light",
									&prjltOperations::AddObject );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Create -> Projected Light At Camera Position
		pCmd = new cmaCommandSimple("Projected Light at Camera Position", 
									"Create", 
									"Creates new projected light at current camera position",
									&Execute_CreateProjectedLightAtView );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Create -> Spot Light
		pCmd = new cmaCommandSimple("Spot Light", 
									"Create", 
									"Creates new spot light",
									&Execute_CreateSpotLight );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


		//	COMMAND: Create -> Spot Light At Camera Position
		pCmd = new cmaCommandSimple("Spot Light at Camera Position", 
									"Create", 
									"Creates new spot light at current camera position",
									&Execute_CreateSpotLightAtView );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


		//	COMMAND: Create -> Directional Light
		pCmd = new cmaCommandSimple("Directional Light", 
									"Create", 
									"Creates new directional light",
									&Execute_CreateDirectionalLight );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	create the system tab page
		//System::Windows::Forms::TabPage^ pTP = tmaCommandTabControlUtil::CreateSystemTabPage( "Projected Lights" );

		//
		//	commands
		//
		guiMenuMgr::AddMenu( "View", "Icons" );

		//	COMMAND: view icons
		pCmd = new cmaCommandToggle("Projected Light Icons", 
									"Icons", 
									"Toggle Icon Visibility",
									&prjltObjectMgr::ShowIcons, 
									&prjltObjectMgr::IconsVisible);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "Icons", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Projected Lights", "Toggle Icon Visibility", pCmd );

		//
		//	driver commands (create ALL of them)
		//
		chnlCommandUtil::CreateDriverButton( "Spline", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Attach", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Static Position", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Enabled", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Color", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Spline", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Attach", "Projected Lights" );
		chnlCommandUtil::CreateDriverButton( "Target Static Position", "Projected Lights" );
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
