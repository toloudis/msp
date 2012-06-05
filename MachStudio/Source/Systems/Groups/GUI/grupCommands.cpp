/*****************************************************************************
**	grupCommands.cpp
**
**	Sets up menu buttons for system grup
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/GUI/grupCommands.hpp"
#include "Systems/Groups/Undo/grupOperations.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

namespace grupCommands
{

	namespace
	{


	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//	COMMAND: Create -> Group
		cmaCommand* pCmd = new cmaCommandSimple("Selection Set", 
									"Create", 
									"Creates new empty selection set",
									&grupOperations::AddObject );
		int menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Create -> Group From Selection
		pCmd = new cmaCommandSimple("Selection Set from Current Selection", 
									"Create", 
									"Creates new selection set containing current selection",
									&grupOperations::CreateGroupFromSelection );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	}


}	// end of namespace
