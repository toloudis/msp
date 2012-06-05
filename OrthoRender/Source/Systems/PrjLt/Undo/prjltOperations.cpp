/*****************************************************************************
**	prjltOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Undo/prjltOperations.hpp"

#include "Systems/PrjLt/Undo/prjltActualOperations.hpp"
#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"


//============================================================================
//============================================================================
namespace prjltOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Projected Light";
		typedef cmmAddOperationTemplate< prjltScriptData, prjltActualOperations> prjltAddOperation;
		const char *c_DuplicateOperationDisplayName = "Duplicate Projected Light";

		const char *c_DeleteOperationDisplayName = "Delete Projected Light";
		typedef cmmDeleteOperationTemplate< prjltScriptData, prjltActualOperations> prjltDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "Projected Light Settings";
		typedef cmmSetOperationTemplate< prjltScriptData, prjltActualOperations, prjltObjectMgr> prjltSetOperation;

	}	// end of namespace

	//--------------------------------------------------------------------
	//	PrjLt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	void  AddObject()
	{
		prjltScriptData default_data;
		AddObject(default_data);
	}

	//--------------------------------------------------------------------
	//  Add new projected light to world
	//--------------------------------------------------------------------
	void  AddObject(const prjltScriptData& i_Data)
	{
		int index = prjltActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new prjltAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	// Add projected light at current camera position
	//--------------------------------------------------------------------
	void AddLightAtCameraPosition()
	{
		camCamera &current_cam = cam3dMgr::GetCamera();
		
		prjltScriptData new_data;
		new_data.m_BaseData.m_Position = current_cam.GetPosition();
		new_data.m_BaseData.m_Target = current_cam.GetTarget();
		AddObject(new_data);
	}

	//--------------------------------------------------------------------
	//  Select projected light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		SetSelectedIndex(i_Index);
		prjltObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = prjltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			prjltObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = prjltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			prjltObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//  Delete projected light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, prjltObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new prjltDeleteOperation(i_Index, prjltObjectMgr::GetScriptData(i_Index), displaytext));
		prjltActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		prjltScriptData clone_data = prjltObjectMgr::GetScriptData(i_Index);
		nameString org_name = clone_data.m_BaseData.m_Name.GetValue(); // preserve original name
		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = prjltActualOperations::AddObject(clone_data);

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DuplicateOperationDisplayName, org_name.GetString().c_str());
		undoUndoMgr::AddOperation(new prjltAddOperation(index, clone_data, displaytext));
		
		// Find which light set the light was in and add the new one
		// to the same set
		nameString set_name;
		if (ltstLightSetMgr::GetSetNameFromLight(org_name, set_name))
		{
			prjltProjectedLightObject *new_obj = prjltObjectMgr::GetObject(index)->GetPickObject();
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
	// Update individual projected light properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const prjltData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			char displaytext[128];
			sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_Name.GetString().c_str());
			l_LastOp = new prjltSetOperation(l_SelectedIndex, prjltObjectMgr::GetScriptData(l_SelectedIndex), displaytext );
			undoUndoMgr::AddOperation(l_LastOp);
		}
		prjltActualOperations::SetBaseData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual projected light properties
	//--------------------------------------------------------------------
	void ChangeLightData(const prjltScriptData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			char displaytext[128];
			sprintf(displaytext, "%s - %s", c_SetOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
			l_LastOp = new prjltSetOperation(l_SelectedIndex, prjltObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		prjltActualOperations::SetData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const prjltScriptData& i_Data)
	{
		// we could do undo here
		prjltActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}



}	// end of namespace
