/*****************************************************************************
**  cmmCommands.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmCommands.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmPython.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/cma/cmaCommandToggle.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace cmmCommands
{
	//
	//	Command Functions
	//
	namespace
	{
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_ShowSystemDialog()
		{
			cmmSystemDialogUtil::Show();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Execute_ShowObjectDialog()
		{
			cmmObjectDialogUtil::Show();
		}
	}

	//--------------------------------------------------------------------
	// SetupMenu
	//--------------------------------------------------------------------
	void SetupMenu()
	{
		//
		//	Commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Show Scene Properties Dialog
		pCmd = new cmaCommandSimple("Scene Manager", 
									"Windows", 
									"Show the Scene Properties",
									&Execute_ShowSystemDialog );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Show Scene Properties", pCmd );

		//	COMMAND: Show Object Properties Dialog
		pCmd = new cmaCommandSimple("Object Properties", 
									"Windows", 
									"Show the Object Properties",
									&Execute_ShowObjectDialog );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Show Object Properties", pCmd );

		//	COMMAND: Duplicate
		pCmd = new cmaCommandSimple("Duplicate", 
									"Edit", 
									"Duplicate the selected object",
									&cmmSystemDialogUtil::DuplicateSelected );
		menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Reload
		pCmd = new cmaCommandSimple("Reload", 
									"Edit", 
									"Reload the selected object",
									&cmmSystemDialogUtil::ReloadSelected );
		menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Delete
		pCmd = new cmaCommandSimple("Delete", 
									"Edit", 
									"Delete the selected object",
									&cmmSystemDialogUtil::DeleteSelected );
		menu_id = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );


		//	COMMAND: Toggle Property Key Button
		pCmd = new cmaCommandToggle("Property Key Buttons", 
									"View", 
									"Toggle display of buttons for keying properties",
									&cmmObjectDialogUtil::SetViewPropertyKeyButtons,
									&cmmObjectDialogUtil::GetViewPropertyKeyButtons);
		menu_id = guiMenuMgr::AddCheckableMenuItem( "View", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		cmmPython::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
