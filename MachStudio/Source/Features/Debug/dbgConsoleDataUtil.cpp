/*****************************************************************************
**	dbgConsoleDataUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/dbgConsoleDataUtil.hpp"

#include "Features/Debug/dbgConsoleDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Features/Debug/qtGUI/dbgConsoleDialog.hpp"
#else
#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"
#endif


#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif
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
