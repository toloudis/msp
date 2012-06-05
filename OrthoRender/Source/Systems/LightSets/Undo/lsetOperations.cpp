/*****************************************************************************
**	lsetOperations.cpp
**
**	Utility for operations that are undoable in lset system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetOperations.hpp"

#include "Systems/LightSets/Undo/lsetActualOperations.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
namespace lsetOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add LightSet";
		typedef cmmAddOperationTemplate< lsetScriptData, lsetActualOperations> lsetAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete LightSet";
		typedef cmmDeleteOperationTemplate< lsetScriptData, lsetActualOperations> lsetDeleteOperation;


		// TODO: [bga] - all operations need an undo operation

		// all operations need to set data changed
		void set_data_changed()
		{
			lsetDocumentChunk::ActiveDataChanged();
		}

	}	// end of namespace


//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new light set
	//--------------------------------------------------------------------
	void  AddObject()
	{
		lsetScriptData default_data;
		AddObject(default_data);
	}
	void  AddObject(const lsetScriptData& i_Data)
	{
		int index = lsetActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_BaseData.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new lsetAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select light set with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		lsetObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = lsetObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			lsetObjectMgr::SelectObject(index, i_bAppend);
		}
		else if (i_Name.GetString() == lsetObjectMgr::GetGlobalObjectName())
		{
			lsetObjectMgr::SelectGlobalObject(i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = lsetObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			lsetObjectMgr::DeselectObject(index);
		}
		else if (i_Name.GetString() == lsetObjectMgr::GetGlobalObjectName())
		{
			lsetObjectMgr::DeselectGlobalObject();
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, lsetObjectMgr::GetBaseData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new lsetDeleteOperation(i_Index, lsetObjectMgr::GetScriptData(i_Index), displaytext));
		lsetActualOperations::DeleteObject(i_Index);
	}

	//--------------------------------------------------------------------
	//	Add named light to named light set.
	//--------------------------------------------------------------------
	void  AddLightToSet(const nameString& i_SetName, 
						const nameString& i_LightName)
	{
		ltstLightSetMgr::AddLightToSet(i_SetName, i_LightName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Remove named light from named light set.
	//--------------------------------------------------------------------
	void  RemoveLightFromSet(const nameString& i_SetName, 
							 const nameString& i_LightName)
	{
		ltstLightSetMgr::RemoveLightFromSet(i_SetName, i_LightName);
		set_data_changed();
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLightSet(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		ltstLightSetMgr::AddObjectToLightSet(i_SetName, i_ObjectName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLightSet(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		ltstLightSetMgr::RemoveObjectFromLightSet(i_SetName, i_ObjectName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Add node to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddNodeToLightSet(const nameString& i_SetName, 
							const nameString& i_ObjectName,
							int i_NodeIndex)
	{
		ltstLightSetMgr::AddNodeToLightSet(i_SetName, i_ObjectName, i_NodeIndex);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Remove node from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveNodeFromLightSet(const nameString& i_SetName, 
								 const nameString& i_ObjectName,
								 int i_NodeIndex)
	{
		ltstLightSetMgr::RemoveNodeFromLightSet(i_SetName, i_ObjectName, i_NodeIndex);
		set_data_changed();
	}


}	// end of namespace
