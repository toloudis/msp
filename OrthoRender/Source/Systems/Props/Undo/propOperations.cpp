/*****************************************************************************
**	propOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Undo/propOperations.hpp"

#include "Systems/Props/Undo/propActualOperations.hpp"
#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
//============================================================================
namespace propOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Prop Item";
		typedef cmmAddOperationTemplate< propScriptData, propActualOperations> propAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Prop Item";
		typedef cmmDeleteOperationTemplate< propScriptData, propActualOperations> propDeleteOperation;

		// Maybe this should be different for what changed (i.e. Prop Position, etc.)
		//const char *c_SetOperationDisplayName = "Prop Settings";
		//typedef cmmSetOperationTemplate< propScriptData, propActualOperations, propObjectMgr> propSetOperation;

	}

	//--------------------------------------------------------------------
	//  Add new item to Prop
	//--------------------------------------------------------------------
	void  AddObject(const propScriptData &i_Item)
	{
		int index = propActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new propAddOperation( index, i_Item, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Prop
	//--------------------------------------------------------------------
	void  RemoveProp(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, propObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new propDeleteOperation( i_Index, propObjectMgr::GetScriptData(i_Index), displaytext ) );
		propActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		propScriptData clone_data = propObjectMgr::GetScriptData(i_Index);

		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = propActualOperations::AddObject(clone_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new propAddOperation(index, clone_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select prop with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		propObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString i_Name, bool i_bAppend)
	{
		int index = propObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			propObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = propObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			propObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//	Select object part, like surface or material
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName, 
						  bool i_bAppend)
	{
		int index = propObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			propScriptObject *pObejct = propObjectMgr::GetObject(index);
			pObejct->SelectObjectPart(i_PartName, i_CategoryName, i_bAppend);
			SetSelectedIndex(index);
		}
	}

	//--------------------------------------------------------------------
	//  Keep track of index of prop being edited so that calls to
	//  ChangePropData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const propScriptData& i_Data)
	{
		// we could do undo here
		propActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of prop with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		propObjectMgr::SetEditorVisible(i_Index, i_bVisible);
	}

}	// end of namespace
