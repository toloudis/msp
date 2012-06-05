/*****************************************************************************
**	grupCommands.cpp
**
**	Sets up menu buttons for system grup
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Groups/GUI/grupCommands.hpp"

#include "Systems/Groups/GUI/grupDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/dbg/dbgLog.hpp"


namespace grupCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditGroupsDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "GroupsMenu Executed (%s)", pCmd->GetTag().c_str() );
			grupDialogUtil::ShowGroupsDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id = guiMenuMgr::AddMenuItem( "Windows", "Edit Groups" );
		cmaCommand* pCmd = new cmaCommand( std::string("EditGroupsData"), EditGroupsDataExecute, NULL  );
		guiCommandMgr::Add( pCmd, std::string("EditGroupsData"), menu_id );
	}


}	// end of namespace
