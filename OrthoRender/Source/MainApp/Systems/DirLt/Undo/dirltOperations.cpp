/*****************************************************************************
**	dirltOperations.cpp
**
**	Utility for operations that are undoable in dirlt system
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltOperations.hpp"

#include "dirltActualOperations.hpp"
#include "dirltObjectMgr.hpp"

#include "cmmAddOperationTemplate.hpp"
#include "cmmDeleteOperationTemplate.hpp"
#include "cmmSetOperationTemplate.hpp"
#include "undoUndoMgr.hpp"



namespace dirltOperations
{
	namespace
	{
		undoUndoOperation *l_LastOp = NULL;
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Dir Light";
		typedef cmmAddOperationTemplate< dirltScriptData, dirltActualOperations> dirltAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Dir Light";
		typedef cmmDeleteOperationTemplate< dirltScriptData, dirltActualOperations> dirltDeleteOperation;

		// Maybe this should be different for what changed (i.e. Position, etc.)
		const char *c_SetOperationDisplayName = "Dir Light Settings";
		typedef cmmSetOperationTemplate< dirltScriptData, dirltActualOperations, dirltObjectMgr> dirltSetOperation;

	}	// end of namespace


	//--------------------------------------------------------------------
	//	dirlt operations are joined together if the same type of operation.
	//	Call this to force a new operation
	//--------------------------------------------------------------------
	void StartNewOp()
	{
		l_LastOp = NULL;
	}

	//--------------------------------------------------------------------
	//  Add new dir light to world
	//--------------------------------------------------------------------
	void  AddObject()
	{
		dirltScriptData default_data;
		int index = dirltActualOperations::AddObject(default_data);

		l_LastOp = NULL;
		undoUndoMgr::AddOperation(new dirltAddOperation(index, default_data, c_AddOperationDisplayName));
	}

	//--------------------------------------------------------------------
	//  Select dir light with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		dirltObjectMgr::SelectObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Delete dir light with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		l_LastOp = NULL;
		undoUndoMgr::AddOperation(new dirltDeleteOperation(i_Index, dirltObjectMgr::GetScriptData(i_Index), c_DeleteOperationDisplayName));
		dirltActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//  Make a clone of the dir light with given index
	//--------------------------------------------------------------------
	void  DuplicateDirLight(int i_Index)
	{
		dirltScriptData clone_data = dirltObjectMgr::GetScriptData(i_Index);
		// clear out name, let object mgr make new one
		clone_data.m_BaseData.m_Name = nameString(); 
		int index = dirltActualOperations::AddObject(clone_data);

		l_LastOp = NULL;
		undoUndoMgr::AddOperation(new dirltAddOperation(index, clone_data, c_AddOperationDisplayName));
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
	// Update individual basic properties
	//--------------------------------------------------------------------
	void ChangeBaseData(const dirltData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetBaseData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Update individual dir light properties
	//--------------------------------------------------------------------
	void ChangeLightData(const dirltScriptData& i_Data)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetData(l_SelectedIndex, i_Data);
	}


	//--------------------------------------------------------------------
	// Update individual driver properties
	//--------------------------------------------------------------------
	void ChangeDriverData(const dirltScriptData& i_Data)
	{
		// we could do undo here
		dirltActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);
	}

	//--------------------------------------------------------------------
	// Change the Name
	//--------------------------------------------------------------------
	void ChangeName(const std::string& i_Name)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}

		//	get the current name, replace the name string, and then set the name
		nameString name = dirltObjectMgr::GetBaseData(l_SelectedIndex).m_Name.GetValue();
		name.SetString(i_Name);
		dirltActualOperations::SetName(l_SelectedIndex, name);
	}

	//--------------------------------------------------------------------
	// Change the Position
	//--------------------------------------------------------------------
	void ChangePosition(const maPoint3d& i_Position)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetPosition(l_SelectedIndex, i_Position);
	}

	//--------------------------------------------------------------------
	// Change the Orientation
	//--------------------------------------------------------------------
	void ChangeOrientation(const maRotation& i_Orientation)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetOrientation(l_SelectedIndex, i_Orientation);
	}

	//--------------------------------------------------------------------
	// ChangeColor
	//--------------------------------------------------------------------
	void ChangeColor(int i_Index, const maFloatRGBA& i_Color)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetColor(l_SelectedIndex, i_Color);
	}

	//--------------------------------------------------------------------
	// ChangeEnabled
	//--------------------------------------------------------------------
	void ChangeEnabled(int i_Index, const bool i_Enabled)
	{
		if (!l_LastOp || (undoUndoMgr::PeekLastOperation() != l_LastOp))
		{
			l_LastOp = new dirltSetOperation(l_SelectedIndex, dirltObjectMgr::GetScriptData(l_SelectedIndex), c_SetOperationDisplayName);
			undoUndoMgr::AddOperation(l_LastOp);
		}
		dirltActualOperations::SetEnabled(l_SelectedIndex, i_Enabled);
	}

}	// end of namespace
