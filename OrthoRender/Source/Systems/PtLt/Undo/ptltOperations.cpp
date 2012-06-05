/*****************************************************************************
**	ptltOperations.cpp
**
**	Utility for operations that are undoable in ptlt system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Systems/PtLt/Undo/ptltActualOperations.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"


//============================================================================
//============================================================================
namespace ptltOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Point Light";
		typedef cmmAddOperationTemplate< ptltScriptData, ptltActualOperations> ptltAddOperation;
		const char *c_DuplicateOperationDisplayName = "Duplicate Point Light";

		const char *c_DeleteOperationDisplayName = "Delete Point Light";
		typedef cmmDeleteOperationTemplate< ptltScriptData, ptltActualOperations> ptltDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		//const char *c_SetOperationDisplayName = "Point Light Settings";
		//typedef cmmSetOperationTemplate< ptltScriptData, ptltActualOperations, ptltObjectMgr> ptltSetOperation;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	Ptlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Add new point light to world
	//--------------------------------------------------------------------
	void  AddObject()
	{
		ptltScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const ptltScriptData& i_Data)
	{
		int index = ptltActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new ptltAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	// Add projected light at current camera position
	//--------------------------------------------------------------------
	void AddLightAtCameraPosition()
	{
		camCamera &current_cam = cam3dMgr::GetCamera();
		
		ptltScriptData new_data;
		new_data.m_BaseData.m_Position = current_cam.GetPosition();
		AddObject(new_data);
	}

	//--------------------------------------------------------------------
	//  Select point light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		ptltObjectMgr::SelectObject(i_Index);
		SetSelectedIndex(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = ptltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			ptltObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = ptltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			ptltObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, ptltObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new ptltDeleteOperation(i_Index, ptltObjectMgr::GetScriptData(i_Index), displaytext));
		ptltActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		ptltScriptData clone_data = ptltObjectMgr::GetScriptData(i_Index);
		nameString org_name = clone_data.m_BaseData.m_Name.GetValue(); // preserve original name
		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = ptltActualOperations::AddObject(clone_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DuplicateOperationDisplayName, org_name.GetString().c_str());
		undoUndoMgr::AddOperation(new ptltAddOperation(index, clone_data, displaytext));

		// Find which light set the light was in and add the new one
		// to the same set
		nameString set_name;
		if (ltstLightSetMgr::GetSetNameFromLight(org_name, set_name))
		{
			ptltPointLightObject *new_obj = ptltObjectMgr::GetObject(index)->GetPickObject();
			DBG_ASSERT0(new_obj, "DuplicateObject, do not have new light.");

			// Can't use the cloned data in order to get the new name, because it
			//	was probably changed when creating the object.
			ltstLightSetMgr::AddLightToSet(set_name, new_obj->GetName());
		}
	}

	//--------------------------------------------------------------------
	//  Keep track of index of light being edited so that calls to
	//  ChangeLightData affect the right light
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const ptltScriptData& i_Data)
	{
		// we could do undo here
		ptltActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

}	// end of namespace
