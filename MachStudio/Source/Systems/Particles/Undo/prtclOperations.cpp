/*****************************************************************************
**	prtclOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/Undo/prtclOperations.hpp"

#include "Systems/Particles/Undo/prtclActualOperations.hpp"
#include "Systems/Particles/Object/prtclObject.hpp"
#include "Systems/Particles/Object/prtclObjectMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
//============================================================================
namespace prtclOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Particle Item";
		typedef cmmAddOperationTemplate< prtclScriptData, prtclActualOperations> prtclAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Particle Item";
		typedef cmmDeleteOperationTemplate< prtclScriptData, prtclActualOperations> prtclDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "Particle Settings";
		typedef cmmSetOperationTemplate< prtclScriptData, prtclActualOperations, prtclObjectMgr> prtclSetOperation;

	}

	//--------------------------------------------------------------------
	//  Add new item to Particle
	//--------------------------------------------------------------------
	void  AddObject(const prtclScriptData &i_Item)
	{
		int index = prtclActualOperations::AddObject(i_Item);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Item.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new prtclAddOperation( index, i_Item, displaytext ) );
	}

	//--------------------------------------------------------------------
	//  Removes an item from the Particle
	//--------------------------------------------------------------------
	void  RemoveParticle(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, prtclObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation( new prtclDeleteOperation( i_Index, prtclObjectMgr::GetScriptData(i_Index), displaytext ) );
		prtclActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the Object with given index
	//--------------------------------------------------------------------
	void  DuplicateObject(int i_Index)
	{
		prtclScriptData clone_data = prtclObjectMgr::GetScriptData(i_Index);

		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = prtclActualOperations::AddObject(clone_data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		l_LastOp = NULL;
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, clone_data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new prtclAddOperation(index, clone_data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select prtcl with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		SetSelectedIndex(i_Index);
		prtclObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = prtclObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			prtclObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = prtclObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			prtclObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Keep track of index of prtcl being edited so that calls to
	//  ChangeParticleData affect the right one
	//--------------------------------------------------------------------
	void SetSelectedIndex(int i_Index)
	{
		l_SelectedIndex = i_Index;
	}

	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const prtclScriptData& i_Data)
	{
		// we could do undo here
		prtclActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
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
			l_LastOp = new prtclSetOperation(l_SelectedIndex, prtclObjectMgr::GetScriptData(l_SelectedIndex), displaytext);
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current name, replace the name string, and then set the name
		nameString name = prtclObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetValue();
		name.SetString(i_Name);
		prtclActualOperations::SetName(l_SelectedIndex, name);
	}

	//--------------------------------------------------------------------
	// ResetGenerators - clear out particles from generators to
	//	to restart generation
	//--------------------------------------------------------------------
	void ResetGenerators()
	{
		// no undo needed, just editor operation
		prtclObjectMgr::ResetGenerators();
	}

}	// end of namespace
