/*****************************************************************************
**	todCommands.cpp
**
**	Sets up menu buttons for system tod
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "todCommands.hpp"

#include "todDialogUtil.hpp"

#include "guiCommandMgr.hpp"
#include "muiMenuMgr.hpp"

#include "dbgLog.hpp"


namespace todCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditTimeOfDayDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "TimeOfDayMenu Executed (%s)", pCmd->GetTag().c_str() );
			//todDialogUtil::ShowTimeOfDayDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//guiCommandMgr::Add( new cmaCommand( std::string("EditTimeOfDayData"), EditTimeOfDayDataExecute, NULL  ) );

		// create menu, no toolbar
		//int menu_id = muiMenuMgr::AddMenuItem( "Level", "Edit TimeOfDay ..." );

		//cmaCommandMgr::RegisterObject( std::string("EditTimeOfDayData"), menu_id );

	}


}	// end of namespace
