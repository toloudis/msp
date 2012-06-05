/*****************************************************************************
**	dirltCommands.cpp
**
**	Sets up menu buttons for system dirlt
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/


#include "dirltCommands.hpp"

#include "dirltDialogUtil.hpp"

#include "cmaCommandMgr.hpp"
#include "muiMenuMgr.hpp"

#include "dbgLog.hpp"


namespace dirltCommands
{

	namespace
	{

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void EditPtltDataExecute( cmaCommand* pCmd )
		{
			//DBG_LOG1( "PtltMenu Executed (%s)", pCmd->GetTag().c_str() );
			//dirltDialogUtil::ShowDirLightsDialog();
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//cmaCommandMgr::Add( new cmaCommand( std::string("EditPtltData"), EditPtltDataExecute, NULL  ) );

		// create menu, no toolbar
		//int menu_id = muiMenuMgr::AddMenuItem( "Level", "Dir Lights ..." );

		//cmaCommandMgr::RegisterObject( std::string("EditPtltData"), menu_id );

	}


}	// end of namespace
