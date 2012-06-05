/*****************************************************************************
**	envtOperations.cpp
**
**	Utility for operations that are undoable in envt system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Undo/envtOperations.hpp"

#include "Systems/Environments/Undo/envtActualOperations.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"
#include "Systems/Environments/GUI/envtTextureList.hpp"

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

	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToEnvironment(envtScriptObject* i_Environment, 
							  const nameString& i_ObjectName)
	{
		// remove object from existing envt
		envtScriptObject* curEnv = FindEnvironmentOfObject(i_ObjectName);
		if (curEnv != NULL)
			curEnv->Remove(i_ObjectName);
		if (i_Environment != NULL)
			i_Environment->Add(i_ObjectName);

		envtDocumentChunk::ActiveDataChanged();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromEnvironment(envtScriptObject* i_Environment, 
								   const nameString& i_ObjectName)
	{
		if (i_Environment != NULL)
			i_Environment->Remove(i_ObjectName);
		envtDocumentChunk::ActiveDataChanged();
	}


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

		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new envtAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select environment with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		envtObjectMgr::SelectObject(i_Index);
		SetSelectedIndex(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = envtObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			SetSelectedIndex(index);
			envtObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = envtObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
			envtObjectMgr::DeselectObject(index);
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, envtObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new envtDeleteOperation(i_Index, envtObjectMgr::GetScriptData(i_Index), displaytext));
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
	// Find the envt that contains the given object
	//--------------------------------------------------------------------
	envtScriptObject* FindEnvironmentOfObject(const nameString& i_ObjectName)
	{
		int n = envtObjectMgr::GetNumObjects();
		for (int i = 0; i < n; i++)
		{
			envtScriptObject* ob = envtObjectMgr::GetObject(i);
			if (ob->HasObject(i_ObjectName))
				return ob;
		}
		// name not in any known environment.
		return NULL;
	}

	//--------------------------------------------------------------------
	//	Remove object from manager (removing from all environments)
	//--------------------------------------------------------------------
	void RemoveObject(nameObject* i_pNameObj, api3dObjectSingle* i_pObject)
	{
		envtScriptObject* env = FindEnvironmentOfObject(i_pNameObj->GetName());
		if (env != NULL)
			RemoveObjectFromEnvironment(env, i_pNameObj->GetName());
		evmtEnvironmentMgr::RemoveObject(i_pNameObj, i_pObject);

		envtDocumentChunk::ActiveDataChanged();
	}

	void RefreshNames()
	{
		int n = envtObjectMgr::GetNumObjects();
		for (int i = 0; i < n; i++)
		{
			envtScriptObject* ob = envtObjectMgr::GetObject(i);
			ob->RefreshNames();
		}
	}

	void RemoveByName(const nameString& i_ObjectName)
	{
		envtScriptObject* env = FindEnvironmentOfObject(i_ObjectName);
		if (env != NULL)
			RemoveObjectFromEnvironment(env, i_ObjectName);
	}

}	// end of namespace
