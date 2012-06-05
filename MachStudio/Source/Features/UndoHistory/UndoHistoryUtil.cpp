/*****************************************************************************
**	UndoHistoryUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Features/UndoHistory/UndoHistoryUtil.hpp"

#include "Features/UndoHistory/UndoHistoryDialogUtil.hpp"

#include "Support/mnm/mnmConstants.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

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

		int l_MenuItemUndo = -1;
		int l_MenuItemRedo = -1;


		void Execute_Undo()
		{
			std::string undo_msg = "Undo: " + undoUndoMgr::GetUndoOperationName();
			guiStatusBarMgr::SetText(mnmConstants::e_SBPanel_Messages, undo_msg.c_str());
			undoUndoMgr::Undo();
		}

		void Execute_Redo()
		{
			std::string undo_msg = "Redo: " + undoUndoMgr::GetRedoOperationName();
			guiStatusBarMgr::SetText(mnmConstants::e_SBPanel_Messages, undo_msg.c_str());
			undoUndoMgr::Redo();
		}
	}

	//------------------------------------------------------------------------
	//  AddToMenu() - add UndoHistory actions to menus
	//------------------------------------------------------------------------
	void  AddToMenu()
	{
		//
		//	commands
		//
		cmaCommand* pCmd;

		//	COMMAND: Undo
		pCmd = new cmaCommandSimple("Undo", 
									"Edit", 
									"Undo the last action",
									&Execute_Undo );
		l_MenuItemUndo = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuItemUndo );

		//	COMMAND: Redo
		pCmd = new cmaCommandSimple("Redo", 
									"Edit", 
									"Redo the last undone action",
									&Execute_Redo );
		l_MenuItemRedo = guiMenuMgr::AddMenuItem( "Edit", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), l_MenuItemRedo );

		//bga - Removed from product
		//	COMMAND: Undo History
		//pCmd = new cmaCommandSimple("Undo History", 
		//							"Windows", 
		//							"Undo History window",
		//							&UndoHistoryDialogUtil::Show );
		//menu_id = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		//guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), menu_id );
		//cmmSystemDialogUtil::AddSystemCommand( "Windows", "Undo History", pCmd );
			
	}

	//------------------------------------------------------------------------
	// Update state and labels for the undo and redo buttons
	//------------------------------------------------------------------------
	void UpdateMenuItems()
	{
		if (l_MenuItemUndo > 0)
		{
			//std::string undo_msg = "Undo  " + undoUndoMgr::GetUndoOperationName();
			//guiMenuMgr::( l_MenuItemUndo, undoUndoMgr::CanUndo() );
			
			guiMenuMgr::MenuObjectsEnable( l_MenuItemUndo, undoUndoMgr::CanUndo() );
		}
		if (l_MenuItemRedo > 0)
		{
			guiMenuMgr::MenuObjectsEnable( l_MenuItemRedo, undoUndoMgr::CanRedo() );
		}
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
