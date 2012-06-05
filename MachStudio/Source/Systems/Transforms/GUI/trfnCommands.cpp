/*****************************************************************************
**	trfnCommands.cpp
**
**	Sets up menu buttons for system trfn
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Transforms/GUI/trfnCommands.hpp"
#include "Systems/Transforms/Undo/trfnOperations.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


namespace trfnCommands
{

	namespace
	{
	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//	COMMAND: Create -> Parent
		cmaCommand* pCmd = new cmaCommandSimple("Parent", 
									"Create", 
									"Creates new empty parent node",
									&trfnOperations::AddObject );
		int menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		//	COMMAND: Create -> Parent from selection
		pCmd = new cmaCommandSimple("Parent from Selection", 
									"Create", 
									"Creates new parent node from selection",
									&trfnOperations::CreateTransformFromSelection );
		menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

	}


}	// end of namespace
