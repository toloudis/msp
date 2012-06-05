/*****************************************************************************
**	dbgConsoleDataUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/dbgConsoleDataUtil.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"

#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"


#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

//============================================================================
//============================================================================
namespace dbgConsoleDataUtil
{

	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd = NULL;
		
		//	COMMAND: Render Log window
		pCmd = new cmaCommandSimple("Debug Console", 
									"Windows", 
									"View the Debug Console",
										
									&dbgConsoleDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Debug Console", pCmd );
		dbgConsoleDialogUtil::Init();
	}

}	// end of namespace dbgConsoleDataUtil
