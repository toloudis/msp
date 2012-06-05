/*****************************************************************************
**	setsOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/Undo/setsOperations.hpp"

#include "Systems/Sets/Undo/setsAddOperation.hpp"
#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/Undo/setsDeleteOperation.hpp"
#include "Systems/Sets/GUI/setsDialogDataUtil.hpp"
#include "Systems/Sets/Data/setsDocumentChunk.hpp"

#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
//============================================================================
namespace setsOperations
{
	//--------------------------------------------------------------------
	//  Add new item to set
	//--------------------------------------------------------------------
	void  AddSetItem(setsScriptData &i_Item)
	{
		undoUndoMgr::AddOperation(new setsAddOperation(i_Item));
		setsDataMgr::AddSetItem(i_Item);

		setsDialogDataUtil::UpdateListDialog();
	}

	//--------------------------------------------------------------------
	//  Removes an item from the set
	//--------------------------------------------------------------------
	void  RemoveSetItem(int i_Index)
	{
		undoUndoMgr::AddOperation(new setsDeleteOperation(setsDataMgr::GetItemData(i_Index)));
		setsDataMgr::RemoveSetItem(i_Index);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of set item with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		setsDataMgr::SetEditorVisible(i_Index, i_bVisible);
	}

	//--------------------------------------------------------------------
	//  Returns the visible state of set item with given index
	//--------------------------------------------------------------------
	bool  GetEditorVisible(int i_Index)
	{
		return setsDataMgr::GetEditorVisible(i_Index);
	}


}	// end of namespace
