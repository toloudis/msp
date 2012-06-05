/*****************************************************************************
**	chtrOperations.cpp
**
**	Utility for doing operations that are undoable in SystemCharacters
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Undo/chtrOperations.hpp"

#include "Systems/Character/Undo/chtrActualOperations.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


namespace chtrOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Character Item";
		typedef cmmAddOperationTemplate< chtrScriptData, chtrActualOperations> chtrAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Character Item";
		typedef cmmDeleteOperationTemplate< chtrScriptData, chtrActualOperations> chtrDeleteOperation;

		// Maybe this should be different for what changed (i.e. Prop Position, etc.)
		const char *c_SetOperationDisplayName = "Character Settings";
		typedef cmmSetOperationTemplate< chtrScriptData, chtrActualOperations, chtrObjectMgr> chtrSetOperation;

	}

	//--------------------------------------------------------------------
	//  Add new item to Character
	//--------------------------------------------------------------------
	void  AddObject(const chtrScriptData &i_Item)
	{
		int index = chtrActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new chtrAddOperation( index, i_Item, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Character
	//--------------------------------------------------------------------
	void  RemoveCharacter(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, chtrObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new chtrDeleteOperation( i_Index, chtrObjectMgr::GetScriptData(i_Index), displaytext ) );
		chtrActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		chtrScriptData clone_data = chtrObjectMgr::GetScriptData(i_Index);

		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = chtrActualOperations::AddObject(clone_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new chtrAddOperation(index, clone_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Reload geometry of character with given index
	//--------------------------------------------------------------------
	void  ReloadCharacter(int i_Index)
	{
		chtrObjectMgr::ReloadCharacter(i_Index);
	}

	//--------------------------------------------------------------------
	//  Select character with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		chtrObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrObjectMgr::SelectObject(index, i_bAppend);
			SetSelectedIndex(index);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			chtrObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//	Select object part, like surface or material
	//--------------------------------------------------------------------
	void SelectObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName, 
						  bool i_bAppend)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
			pCharacter->SelectObjectPart(i_PartName, i_CategoryName, i_bAppend);
			SetSelectedIndex(index);
		}
	}

	//--------------------------------------------------------------------
	//	ActivateObjectPart from Placed, represents a double-click
	//		on a part item in the placed menu.
	//--------------------------------------------------------------------
	void ActivateObjectPart(const nameString& i_Name, 
							const std::string i_PartName, 
							const std::string i_CategoryName)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrScriptObject *pCharacter = chtrObjectMgr::GetObject(index);
			pCharacter->ActivateObjectPart(i_PartName, i_CategoryName);
		}
	}

	//--------------------------------------------------------------------
	//	DeleteObjectPart from Placed, represents DELETE key or button
	//		when a part item is selected in the placed menu.
	//--------------------------------------------------------------------
	void DeleteObjectPart(const nameString& i_Name, 
						  const std::string i_PartName, 
						  const std::string i_CategoryName)
	{
		// we could do undo here
		chtrActualOperations::DeleteObjectPart(i_Name, i_PartName, i_CategoryName);
	}

	//--------------------------------------------------------------------
	//  Keep track of index of character being edited so that calls to
	//  ChangeCharacterData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const chtrScriptData& i_Data)
	{
		// we could do undo here
		chtrActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	//  Changes visible state of character with given index
	//--------------------------------------------------------------------
	void  SetEditorVisible(int i_Index, bool i_bVisible)
	{
		chtrObjectMgr::SetEditorVisible(i_Index, i_bVisible);
	}

	//--------------------------------------------------------------------
	//  Change geometry of character with given index
	//--------------------------------------------------------------------
	void  ChangeFilename(const nameString& i_Name, const itString& i_Filename)
	{
		int index = chtrObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			chtrActualOperations::SetFilename(index, i_Filename);
		}
	}

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			char displaytext[128];
			sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Name.c_str());
			l_LastOp = new chtrSetOperation(l_SelectedIndex, chtrObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current name, replace the name string, and then set the name
		nameString name = chtrObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetValue();
		name.SetString(i_Name);
		chtrActualOperations::SetName(l_SelectedIndex, name);
	}

	//--------------------------------------------------------------------
	// Set the weight for the expression with the given name
	//--------------------------------------------------------------------
	/*void SetExpressionWeight(const std::string& i_Name, float i_Weight)
	{
		chtrActualOperations::SetExpressionWeight(l_SelectedIndex, i_Name, i_Weight);
	}*/

}	// end of namespace
