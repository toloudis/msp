/*****************************************************************************
**	lsetCommands.cpp
**
**	Sets up menu buttons for system lset
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetCommands.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


namespace lsetCommands
{

	namespace
	{

	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//	COMMAND: Create -> Light Set
		cmaCommand* pCmd = new cmaCommandSimple("Light Set", 
									"Create", 
									"Creates new empty light set",
									&lsetOperations::AddObject );
		int menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		
		//	COMMAND: Create -> Light Set From Selection
		pCmd = new cmaCommandSimple("Light Set from Selection", 
									"Create", 
									"Creates new light set containing current selection",
									&lsetOperations::CreateLightSetFromSelection );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	}


}	// end of namespace
