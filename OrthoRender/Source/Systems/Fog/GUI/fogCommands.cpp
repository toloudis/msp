/*****************************************************************************
**	fogCommands.cpp
**
**	Sets up menu buttons for system fog
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "fogCommands.hpp"

#include "fogDialogUtil.hpp"

#include "cmaCommandMgr.hpp"
#include "muiMenuMgr.hpp"

#include "dbgLog.hpp"


namespace fogCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditFogDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "FogMenu Executed (%s)", pCmd->GetTag().c_str() );
			//fogDialogUtil::ShowFogDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//cmaCommandMgr::Add( new cmaCommand( std::string("EditFogData"), EditFogDataExecute, NULL  ) );

		// create menu, no toolbar
		//int menu_id = muiMenuMgr::AddMenuItem( "Level", "Edit Fog ..." );

		//cmaCommandMgr::RegisterObject( std::string("EditFogData"), menu_id );

	}


}	// end of namespace
