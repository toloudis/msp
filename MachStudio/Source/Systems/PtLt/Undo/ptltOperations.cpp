/*****************************************************************************
**	ptltOperations.cpp
**
**	Utility for operations that are undoable in ptlt system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/Undo/ptltOperations.hpp"

#include "Systems/PtLt/Undo/ptltActualOperations.hpp"
#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Features/Prefs/PrefsMgr.hpp"
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
		int l_ObjectCounter = 0;

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
		nameString objName;

		CreateNewObjectName( nameString(), objName);
		default_data.m_BaseData.m_Name = objName;
		AddObject(default_data);
	}
	void  AddObject(const ptltScriptData& i_Data)
	{
		int index = ptltActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		std::string add_operation_name(c_AddOperationDisplayName);
		std::string data_name = i_Data.m_BaseData.m_Name.GetString();
		if(data_name != "")
			add_operation_name += " - ";
		
		std::string display_text = add_operation_name + data_name;
		// Need new name given to new object when making udo operation
		undoUndoMgr::AddOperation(new ptltAddOperation(index, ptltObjectMgr::GetScriptData(index), display_text.c_str()));
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
		sel3dMgr::CreateUndoOperation();
		ptltObjectMgr::SelectObject(i_Index);
		SetSelectedIndex(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = ptltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			ptltObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = ptltObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			ptltObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		std::string displayText(c_DeleteOperationDisplayName);
		std::string base_name = ptltObjectMgr::GetBaseData(i_Index).m_Name.GetString();
		if(base_name != "")
			displayText += " - ";
		displayText += base_name;
		undoUndoMgr::AddOperation(new ptltDeleteOperation(i_Index, ptltObjectMgr::GetScriptData(i_Index), displayText.c_str()));
		ptltActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the object with given index
	//--------------------------------------------------------------------
	nameString  DuplicateObject(int i_Index)
	{
		ptltScriptData clone_data = ptltObjectMgr::GetScriptData(i_Index);
		nameString org_name = clone_data.m_BaseData.m_Name.GetValue(); // preserve original name
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

		int index = ptltActualOperations::AddObject(clone_data);
		if (index < 0) return nameString();	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;

		std::string displayText(c_DuplicateOperationDisplayName);
		if(org_name.GetString() != "")
			displayText += " - ";
		displayText += org_name.GetString();

		undoUndoMgr::AddOperation(new ptltAddOperation(index, clone_data, displayText.c_str()));

		ptltPointLightObject *new_obj = ptltObjectMgr::GetObject(index)->GetPickObject();

		// Find which light set the light was in and add the new one
		// to the same set
		nameString set_name;
		if (ltstLightSetMgr::GetSetNameFromLight(org_name, set_name))
		{
			DBG_ASSERT(new_obj != NULL, "DuplicateObject, does not have new light.");

			// Can't use the cloned data in order to get the new name, because it
			//	was probably changed when creating the object.
			ltstLightSetMgr::AddLightToSet(set_name, new_obj->GetName());
		}

		return new_obj->GetName();
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
	//	Create names for new object
	//--------------------------------------------------------------------
	void CreateNewObjectName(const nameString& i_Filename, nameString& o_NameString)
	{
		if (i_Filename.IsEmpty())
		{
			ptltObjectMgr::create_default_name(itString("PointLight"), o_NameString, l_ObjectCounter);
		}
		else
		{
			ptltObjectMgr::create_duplicate_name(itString(i_Filename.GetString().c_str()), o_NameString);
		}
	}

}	// end of namespace
