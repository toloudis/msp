/*****************************************************************************
**	grupOperations.cpp
**
**	Utility for operations that are undoable in grup system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Undo/grupOperations.hpp"

#include "Systems/Groups/Undo/grupActualOperations.hpp"
#include "Systems/Groups/Data/grupDocumentChunk.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
namespace grupOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add Group";
		typedef cmmAddOperationTemplate< grupData, grupActualOperations> grupAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Group";
		typedef cmmDeleteOperationTemplate< grupData, grupActualOperations> grupDeleteOperation;


		// TODO: [bga] - all operations need an undo operation

		// all operations need to set data changed
		void set_data_changed()
		{
			grupDocumentChunk::ActiveDataChanged();
		}

	}	// end of namespace


//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new group
	//--------------------------------------------------------------------
	void  AddObject()
	{
		grupData default_data;
		AddObject(default_data);
	}
	void  CreateGroupFromSelection()
	{
		grupData selected_data;
		grpsGroupMgr::GetSelectedGroupObjects(selected_data.m_Objects);
		AddObject(selected_data);
	}
	void  AddObject(const grupData& i_Data)
	{
		int index = grupActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new grupAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select group with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		grupObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = grupObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			grupObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = grupObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			grupObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, grupObjectMgr::GetData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new grupDeleteOperation(i_Index, grupObjectMgr::GetData(i_Index), displaytext));
		grupActualOperations::DeleteObject(i_Index);
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToGroup(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		grpsGroupMgr::AddObjectToGroup(i_SetName, i_ObjectName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromGroup(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		grpsGroupMgr::RemoveObjectFromGroup(i_SetName, i_ObjectName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Select objects in a group
	//--------------------------------------------------------------------
	void  SelectGroupObjects(const nameString& i_GroupName)
	{	
		grpsGroupMgr::SelectGroupObjects( i_GroupName );
	}

}	// end of namespace
