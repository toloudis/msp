/*****************************************************************************
**	billOperations.cpp
**
**	Utility for doing operations that are undoable in SystemBillboards
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Systems/Billboard/Undo/billOperations.hpp"

#include "Systems/Billboard/Undo/billActualOperations.hpp"
#include "Systems/Billboard/Object/billBillboardObject.hpp"
#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
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
		int l_ObjectCounter = 0;

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
	void  AddObject()
	{
		billScriptData default_data;
		nameString objName;

		CreateNewObjectName( nameString(), objName);
		default_data.m_BaseData.m_Name = objName;
		billOperations::AddObject( default_data );
	}
	void  AddObject(const billScriptData &i_Item)
	{
		int index = billActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_AddOperationDisplayName <<" - " << i_Item.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation( new billAddOperation( index, billObjectMgr::GetScriptData(index), displaytext.c_str() ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Billboard
	//--------------------------------------------------------------------
	void  RemoveBillboard(int i_Index)
	{
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, billObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_DeleteOperationDisplayName <<" - " <<  billObjectMgr::GetBaseData(i_Index).m_Name.GetString();
		std::string displaytext(oss.str());
		undoUndoMgr::AddOperation( new billDeleteOperation( i_Index, billObjectMgr::GetScriptData(i_Index), displaytext.c_str() ) );
		billActualOperations::DeleteObject(i_Index);
	}

	//------------------------------------------------------------------------
	//  Duplicates an item from the bill
	//------------------------------------------------------------------------
	nameString  DuplicateBillboard(int i_Index)
	{
		billScriptData clone_data = billObjectMgr::GetScriptData(i_Index);
		// clear out name, let object mgr make new one
		//clone_data.m_BaseData.m_Name = nameString(); 
		// Create new name for the duplicated object
		nameString basename, newname;
		if (PrefsMgr::Data().m_bDuplicateObjectsName.GetValue())
		{
			basename.SetString(clone_data.m_BaseData.m_Name.GetString());
		}
		CreateNewObjectName(basename, newname);
		clone_data.m_BaseData.m_Name = newname;
		int index = billActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();

		l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss <<c_AddOperationDisplayName <<" - " <<  clone_data.m_BaseData.m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new billAddOperation(index, clone_data, displaytext.c_str()));

		billBillboardObject *new_obj = billObjectMgr::GetObject(index)->GetPickObject();
		return new_obj->GetName();
	}

	//--------------------------------------------------------------------
	//  Select bill with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		billObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = billObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			billObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = billObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			billObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Keep track of index of bill being edited so that calls to
	//  ChangeBillboardData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString)
	{
		if (i_Filename.IsEmpty())
		{
			billObjectMgr::create_default_name(itString("Billboard"), o_NameString, l_ObjectCounter);
		}
		else
		{
			billObjectMgr::create_duplicate_name(itString(i_Filename.GetString().c_str()), o_NameString);
		}
	}
}	// end of namespace
