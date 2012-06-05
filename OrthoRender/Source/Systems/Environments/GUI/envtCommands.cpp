/*****************************************************************************
**	envtCommands.cpp
**
**	Sets up menu buttons for system envt
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Environments/GUI/envtCommands.hpp"

#include "Systems/Environments/GUI/envtDialogUtil.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/Dbg/dbgLog.hpp"


namespace envtCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditEnvironmentsDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "EnvironmentsMenu Executed (%s)", pCmd->GetTag().c_str() );
			envtDialogUtil::ShowEnvironmentsDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id = guiMenuMgr::AddMenuItem( "Windows", "Edit Environments" );
		cmaCommand* pCmd = new cmaCommand( std::string("EditEnvironmentsData"), EditEnvironmentsDataExecute, NULL  );
		cmaCommandMgr::Add( pCmd, std::string("EditEnvironmentsData"), menu_id );
	}


}	// end of namespace
