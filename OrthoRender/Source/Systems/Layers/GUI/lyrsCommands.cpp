/*****************************************************************************
**	lyrsCommands.cpp
**
**	Sets up menu buttons for system lyrs
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Layers/GUI/lyrsCommands.hpp"

#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/dbg/dbgLog.hpp"


namespace lyrsCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditLayersDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "LayersMenu Executed (%s)", pCmd->GetTag().c_str() );
			lyrsDialogUtil::ShowLayersDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id = guiMenuMgr::AddMenuItem( "Windows", "Edit Layers" );
		cmaCommand* pCmd = new cmaCommand( std::string("EditLayersData"), EditLayersDataExecute, NULL  );
		guiCommandMgr::Add( pCmd, std::string("EditLayersData"), menu_id );
	}


}	// end of namespace
