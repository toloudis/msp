/*****************************************************************************
**	UndoHistoryUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/UndoHistoryUtil.hpp"

#include "Features/UndoHistory/UndoHistoryDialogUtil.hpp"

#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Core/undo/undoUndoMgr.hpp"

#include <string>


//============================================================================
//============================================================================
//using namespace StudioFramework;


//============================================================================
//============================================================================
namespace UndoHistoryUtil
{
	namespace
	{
		UndoHistoryData l_Data;
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add UndoHistory actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		int menu_id;
		cmaCommand* pCmd;

		//	COMMAND: Import
		pCmd = new cmaCommandSimple("Undo History", 
									"Windows", 
									"Undo History window",
									
									&UndoHistoryDialogUtil::Show );
		menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		cmmSystemDialogUtil::AddSystemCommand( "Windows", "Undo History", pCmd );
	}

	//------------------------------------------------------------------------
	//	Build a data list from the passed in doc.
	//------------------------------------------------------------------------
	void BuildDataList( UndoHistoryData& o_Data )
	{
		std::vector<std::string> stack;
		undoUndoMgr::GetUndoStack(stack);
		o_Data.m_NextUndoIndex = undoUndoMgr::GetUndoOperationIndex();

		o_Data.m_HistoryList.resize( stack.size() );
		for (int i = 0; i < stack.size(); ++i)
		{
			o_Data.m_HistoryList[i].m_Name = stack[i];
			o_Data.m_HistoryList[i].m_Size = 0;
		}
	}
}
