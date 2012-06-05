/*****************************************************************************
**	dbgConsole.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "dbgConsole.hpp"

#include "dbgConsoleDialogUtil.hpp"
#include "wxGUI/dbgConsoleDialog.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

//============================================================================
//============================================================================
namespace dbgConsole
{

	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd = NULL;
		
		guiMenuMgr::AddMenu("Window", "");

		//	COMMAND: Render Log window
		pCmd = new cmaCommandSimple("Debug Console", 
									"Window", 
									"View the Debug Console",
									&dbgConsoleDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Window", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );

		dbgConsoleDialogUtil::Init();		
	}

}	// end of namespace dbgConsole
