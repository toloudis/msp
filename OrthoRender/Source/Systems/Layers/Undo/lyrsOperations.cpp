/*****************************************************************************
**	lyrsOperations.cpp
**
**	Utility for operations that are undoable in lyrs system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Systems/Layers/Undo/lyrsActualOperations.hpp"
#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"

#include "Systems/Common/Templates/cmmAddOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmDeleteOperationTemplate.hpp"
#include "Systems/Common/Templates/cmmSetOperationTemplate.hpp"
#include "Core/undo/undoUndoMgr.hpp"


//============================================================================
namespace lyrsOperations
{
	namespace
	{
		const char *c_AddOperationDisplayName = "Add Layer";
		typedef cmmAddOperationTemplate< lyrsData, lyrsActualOperations> lyrsAddOperation;

		const char *c_DeleteOperationDisplayName = "Delete Layer";
		typedef cmmDeleteOperationTemplate< lyrsData, lyrsActualOperations> lyrsDeleteOperation;


		// TODO: [bga] - all operations need an undo operation

		// all operations need to set data changed
		void set_data_changed()
		{
			lyrsDocumentChunk::ActiveDataChanged();
		}

	}	// end of namespace


//============================================================================
//============================================================================

	//--------------------------------------------------------------------
	//  Add new layer
	//--------------------------------------------------------------------
	void  AddObject()
	{
		lyrsData default_data;
		AddObject(default_data);
	}
	void  AddObject(const lyrsData& i_Data)
	{
		int index = lyrsActualOperations::AddObject(i_Data);
		if (index < 0) return;	// if there is a problem loading the object, return immediately

		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new lyrsAddOperation(index, i_Data, displaytext));
	}

	//--------------------------------------------------------------------
	//  Select layer with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		lyrsObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = lyrsObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			lyrsObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = lyrsObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			lyrsObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		char displaytext[128];
		sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, lyrsObjectMgr::GetData(i_Index).m_Name.GetString().c_str());
		undoUndoMgr::AddOperation(new lyrsDeleteOperation(i_Index, lyrsObjectMgr::GetData(i_Index), displaytext));
		lyrsActualOperations::DeleteObject(i_Index);
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLayer(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		lyerLayerMgr::AddObjectToLayer(i_SetName, i_ObjectName);
		set_data_changed();
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLayer(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		lyerLayerMgr::RemoveObjectFromLayer(i_SetName, i_ObjectName);
		set_data_changed();
	}

}	// end of namespace
