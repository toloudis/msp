/*****************************************************************************
**	billOperations.cpp
**
**	Utility for doing operations that are undoable in SystemBillboards
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Undo/billOperations.hpp"

#include "Systems/Billboard/Undo/billActualOperations.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


namespace billOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Billboard Item";
		typedef cmmAddOperationTemplate< billScriptData, billActualOperations> billAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Billboard Item";
		typedef cmmDeleteOperationTemplate< billScriptData, billActualOperations> billDeleteOperation;

		// Maybe this should be different for what changed (i.e. Prop Position, etc.)
		//const char *c_SetOperationDisplayName = "Billboard Settings";
		//typedef cmmSetOperationTemplate< billScriptData, billActualOperations, billObjectMgr> billSetOperation;

	}

	//--------------------------------------------------------------------
	//  Add new item to Billboard
	//--------------------------------------------------------------------
	void  AddObject(const billScriptData &i_Item)
	{
		int index = billActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new billAddOperation( index, i_Item, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Billboard
	//--------------------------------------------------------------------
	void  RemoveBillboard(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, billObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new billDeleteOperation( i_Index, billObjectMgr::GetScriptData(i_Index), displaytext ) );
		billActualOperations::DeleteObject(i_Index);
	}

	//------------------------------------------------------------------------
	//  Duplicates an item from the bill
	//------------------------------------------------------------------------
	void  DuplicateBillboard(int i_Index)
	{
		billScriptData clone_data = billObjectMgr::GetScriptData(i_Index);
		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = billActualOperations::AddObject(clone_data);

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new billAddOperation(index, clone_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select bill with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		billObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = billObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			billObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = billObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			billObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//  Keep track of index of bill being edited so that calls to
	//  ChangeBillboardData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}
}	// end of namespace
