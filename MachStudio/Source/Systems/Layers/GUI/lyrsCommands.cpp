/*****************************************************************************
**	lyrsCommands.cpp
**
**	Sets up menu buttons for system lyrs
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/


#include "Systems/Layers/GUI/lyrsCommands.hpp"
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


namespace lyrsCommands
{

	namespace
	{
	}	// end of namespace

	//--------------------------------------------------------------------
	//  Add commands specific to system to menu bar
	//--------------------------------------------------------------------
	void  SetupMenu()
	{
		//	COMMAND: Create -> Layer
		cmaCommand* pCmd = new cmaCommandSimple("Layer", 
									"Create", 
									"Creates new empty layer",
									&lyrsOperations::AddObject );
		int menu_id = guiMenuMgr::AddMenuItem( "Create", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
	}


}	// end of namespace
