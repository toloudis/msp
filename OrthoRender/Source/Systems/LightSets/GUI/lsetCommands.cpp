/*****************************************************************************
**	lsetCommands.cpp
**
**	Sets up menu buttons for system lset
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/LightSets/GUI/lsetCommands.hpp"

#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include "Core/dbg/dbgLog.hpp"


namespace lsetCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditLightSetsDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "LightSetsMenu Executed (%s)", pCmd->GetTag().c_str() );
			lsetDialogUtil::ShowLightSetsDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		int menu_id = guiMenuMgr::AddMenuItem( "Windows", "Edit LightSets" );
		cmaCommand* pCmd = new cmaCommand( std::string("EditLightSetsData"), EditLightSetsDataExecute, NULL  );
		guiCommandMgr::Add( pCmd, std::string("EditLightSetsData"), menu_id );
	}


}	// end of namespace
