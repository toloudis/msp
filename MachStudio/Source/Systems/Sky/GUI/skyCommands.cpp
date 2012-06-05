/*****************************************************************************
**	skyCommands.cpp
**
**	Sets up menu buttons for system sky
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "skyCommands.hpp"

#include "skyDialogUtil.hpp"

#include "guiCommandMgr.hpp"
#include "muiMenuMgr.hpp"

#include "dbgLog.hpp"


namespace skyCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditSkyDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "SkyMenu Executed (%s)", pCmd->GetTag().c_str() );
			//skyDialogUtil::ShowSkyDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//guiCommandMgr::Add( new cmaCommand( std::string("EditSkyData"), EditSkyDataExecute, NULL  ) );

		// create menu, no toolbar
		//int menu_id = muiMenuMgr::AddMenuItem( "Level", "Edit Sky ..." );

		//cmaCommandMgr::RegisterObject( std::string("EditSkyData"), menu_id );

	}


}	// end of namespace
