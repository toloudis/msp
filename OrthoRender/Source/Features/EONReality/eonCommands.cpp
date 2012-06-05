/*****************************************************************************
**	eonCommands.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/EONReality/eonCommands.hpp"
#include "Features/EONReality/eonExportUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"


//============================================================================
//============================================================================
namespace eonCommands
{
	namespace
	{
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add Import actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
#ifdef EON_REALITY

		//	COMMAND:  Bake textures and export EON Reality files	
		cmaCommand *pCmd = new cmaCommandSimple("Bake EON Reality", 
			"Tools",
			"Bake lighting into textures and export to EON Studio",
			eonExportUtil::DoExportDialog);
		int menu_id = guiMenuMgr::AddMenuItem( "Tools", pCmd->GetTag().c_str() ); 
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
#endif
	}

}
