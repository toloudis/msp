/*****************************************************************************
**	envtOperations.cpp
**
**	Utility for operations that are undoable in envt system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Systems/Environments/Undo/envtActualOperations.hpp"
#include "Systems/Environments/Undo/envtObjectOperation.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"
#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"

#include "Support/evmt/evmtEnvironmentMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
namespace envtOperations
{
	namespace
	{
		int l_SelectedIndex = 0;

		const char *c_AddOperationDisplayName = "Add Environment";
		typedef cmmAddOperationTemplate< envtScriptData, envtActualOperations> envtAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Environment";
		typedef cmmDeleteOperationTemplate< envtScriptData, envtActualOperations> envtDeleteOperation;
	}	// end of namespace



//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new environment
	//--------------------------------------------------------------------
	void  AddObject()
	{
		envtScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const envtScriptData& i_Data)
	{
		int index = envtActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		std::string displayText(c_AddOperationDisplayName);
		if(i_Data.m_BaseData.m_Name.GetString() != "")
			displayText += " - ";
		displayText += i_Data.m_BaseData.m_Name.GetString();
		// Need new name given to new object when making undo operation
		undoUndoMgr::AddOperation(new envtAddOperation(index, envtObjectMgr::GetScriptData(index), displayText.c_str()));
	}

	//--------------------------------------------------------------------
	//  Select environment with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		envtObjectMgr::SelectObject(i_Index);
		SetSelectedIndex(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = envtObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			SetSelectedIndex(index);
			envtObjectMgr::SelectObject(index, i_bAppend);
		}
		else if (i_Name.GetString() == envtObjectMgr::GetDefaultEnvironment()->GetDisplayName())
		{
			sel3dMgr::CreateUndoOperation();
			envtObjectMgr::SelectDefaultEnvironment(i_bAppend);
		}
		else if (i_Name.GetString() == envtObjectMgr::GetSwlEnvironment()->GetDisplayName())
		{
			sel3dMgr::CreateUndoOperation();
			envtObjectMgr::SelectSwlEnvironment(i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = envtObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			envtObjectMgr::DeselectObject(index);
		}
		else if (i_Name.GetString() == envtObjectMgr::GetDefaultEnvironment()->GetDisplayName())
		{
			sel3dMgr::CreateUndoOperation();
			envtObjectMgr::DeselectDefaultEnvironment();
		}
		else if (i_Name.GetString() == envtObjectMgr::GetSwlEnvironment()->GetDisplayName())
		{
			sel3dMgr::CreateUndoOperation();
			envtObjectMgr::DeselectSwlEnvironment();
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		std::string displayText(c_DeleteOperationDisplayName);
		if(envtObjectMgr::GetBaseData(i_Index).m_Name.GetString() != "")
			displayText += " - ";
		displayText += envtObjectMgr::GetBaseData(i_Index).m_Name.GetString();

		undoUndoMgr::AddOperation(new envtDeleteOperation(i_Index, envtObjectMgr::GetScriptData(i_Index), displayText.c_str()));
		envtActualOperations::DeleteObject(i_Index);
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
	void ChangeDriverData(const envtScriptData& i_Data)
	{
		// we could do undo here
		envtActualOperations::ChangeDriverData(l_SelectedIndex, i_Data);

		envtDocumentChunk::ActiveDataChanged();;
	}

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given environment
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(const nameString& i_EnvironmentName, 
							  const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new envtAddObjectOperation(i_ObjectName));
		envtActualOperations::AddObjectToEnvironment(i_EnvironmentName, i_ObjectName);
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given environment
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(const nameString& i_EnvironmentName, 
								   const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new envtRemoveObjectOperation(i_ObjectName));
		envtActualOperations::RemoveObjectFromEnvironment(i_EnvironmentName, i_ObjectName);
	}

}	// end of namespace
