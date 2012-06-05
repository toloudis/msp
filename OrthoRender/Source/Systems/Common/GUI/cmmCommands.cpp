/*****************************************************************************
**  cmmCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmCommands.hpp"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmPython.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
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
		pCmd = new cmaCommandSimple("Scene Properties", 
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

		cmmPython::AddCommands("mach");
	}

	//--------------------------------------------------------------------
	// CleanUp
	//--------------------------------------------------------------------
	void CleanUp()
	{
	}

}	// end of namespace
