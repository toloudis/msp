/*****************************************************************************
**	sbrdOperations.cpp
**
**	Utility for doing operations that are undoable in SystemStoryboards
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/Undo/sbrdOperations.hpp"

#include "Systems/Storyboards/Undo/sbrdActualOperations.hpp"
#include "Systems/Storyboards/Data/sbrdListData.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/Undo/sbrdSwapOperation.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
//============================================================================
namespace sbrdOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Storyboard";
		typedef cmmAddOperationTemplate< sbrdScriptData, sbrdActualOperations> sbrdAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Storyboard";
		typedef cmmDeleteOperationTemplate< sbrdScriptData, sbrdActualOperations> sbrdDeleteOperation;

		const char *c_SwapOperationDisplayName = "Swap Storyboards";

		// Maybe this should be different for what changed (i.e. Prop Position, etc.)
		//const char *c_SetOperationDisplayName = "Billboard Settings";
		//typedef cmmSetOperationTemplate< sbrdScriptData, sbrdActualOperations, sbrdObjectMgr> sbrdSetOperation;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void  AddStoryboard(const itString& i_Filename, bool i_bAddTo3DWorld)
	{
		int index = sbrdActualOperations::AddStoryboard(i_Filename, i_bAddTo3DWorld);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		std::string file_name = itStringUtil::GetStdString(i_Filename);
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, file_name.c_str());

		sbrdScriptData script_data = sbrdObjectMgr::GetScriptData(index);
		undoUndoMgr::AddOperation( new sbrdAddOperation( index, script_data, displaytext ) );
	}
 
	//------------------------------------------------------------------------
	//  Removes an item from the sbrd
	//------------------------------------------------------------------------
	void  RemoveStoryboard(int i_Index)
	{
		sbrdScriptData script_data = sbrdObjectMgr::GetScriptData( i_Index );

		l_LastOp = NULL;
		char displaytext[128];
		std::string file_name = itStringUtil::GetStdString(script_data.m_BaseData.m_Filename.GetValue());
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, file_name.c_str());

		sbrdActualOperations::DeleteStoryboard(i_Index);

		undoUndoMgr::AddOperation( new sbrdDeleteOperation( i_Index, script_data, displaytext ) );
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SwapStoryboards(int i_Index1, int i_Index2)
	{
		sbrdActualOperations::SwapStoryboards(i_Index1, i_Index2);

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %d with %d", c_SwapOperationDisplayName, i_Index1, i_Index2);

		sbrdListData list_data = sbrdObjectMgr::GetData();
		undoUndoMgr::AddOperation( new sbrdSwapOperation( i_Index1, i_Index2, list_data, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Add new item to Billboard
	//--------------------------------------------------------------------
	void  AddObject(const sbrdScriptData &i_Item)
	{
		int index = sbrdActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new sbrdAddOperation( index, i_Item, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, sbrdObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new sbrdDeleteOperation(i_Index, sbrdObjectMgr::GetScriptData(i_Index), displaytext));

		sbrdActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Billboard
	//--------------------------------------------------------------------
	void  RemoveBillboard(int i_Index)
	{
//		char displaytext[128];
//		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, sbrdObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
//		undoUndoMgr::AddOperation( new sbrdDeleteOperation( i_Index, sbrdObjectMgr::GetScriptData(i_Index), displaytext ) );
//		sbrdActualOperations::DeleteObject(i_Index);
	}

	//------------------------------------------------------------------------
	//  Duplicates an item from the sbrd
	//------------------------------------------------------------------------
	void  DuplicateBillboard(int i_Index)
	{
//		sbrdScriptData clone_data = sbrdObjectMgr::GetScriptData(i_Index);
//		// clear out name, let object mgr make new one
//		clone_data.m_BaseData.m_Name = nameString(); 
//		int index = sbrdActualOperations::AddObject(clone_data);
//
//		l_LastOp = NULL;
//		char displaytext[128];
//		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
//		undoUndoMgr::AddOperation(new sbrdAddOperation(index, clone_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select sbrd with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		sbrdObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = sbrdObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			sbrdObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = sbrdObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			sbrdObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//  Keep track of index of sbrd being edited so that calls to
	//  ChangeBillboardData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}
}	// end of namespace
