/*****************************************************************************
**	lyrsOperations.cpp
**
**	Utility for operations that are undoable in lyrs system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Undo/lyrsOperations.hpp"

#include "Systems/Layers/Undo/lyrsActualOperations.hpp"
#include "Systems/Layers/Undo/lyrsObjectOperation.hpp"
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

		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_AddOperationDisplayName, i_Data.m_Name.GetString().c_str());
		// Need new name given to new object when making udo operation
		std::ostringstream oss;
		oss << c_AddOperationDisplayName <<" - "<<i_Data.m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new lyrsAddOperation(index, lyrsObjectMgr::GetData(index), displaytext.c_str()));
	}

	//--------------------------------------------------------------------
	//  Select layer with given index
	//--------------------------------------------------------------------
	void  SelectObject(int i_Index)
	{
		sel3dMgr::CreateUndoOperation();
		lyrsObjectMgr::SelectObject(i_Index);
	}
	void  SelectObject(const nameString& i_Name, bool i_bAppend)
	{
		int index = lyrsObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			lyrsObjectMgr::SelectObject(index, i_bAppend);
		}
	}
	void  DeselectObject(const nameString& i_Name)
	{
		int index = lyrsObjectMgr::GetIndexForObject(i_Name);
		if (index >= 0)
		{
			sel3dMgr::CreateUndoOperation();
			lyrsObjectMgr::DeselectObject(index);
		}
	}

	//--------------------------------------------------------------------
	//  Delete object with given index
	//--------------------------------------------------------------------
	void  DeleteObject(int i_Index)
	{
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_DeleteOperationDisplayName, lyrsObjectMgr::GetData(i_Index).m_Name.GetString().c_str());
		std::ostringstream oss;
		oss << c_DeleteOperationDisplayName <<" - "<<lyrsObjectMgr::GetData(i_Index).m_Name.GetString();
		std::string displaytext(oss.str());
		
		undoUndoMgr::AddOperation(new lyrsDeleteOperation(i_Index, lyrsObjectMgr::GetData(i_Index), displaytext.c_str()));
		lyrsActualOperations::DeleteObject(i_Index);
	}


	//--------------------------------------------------------------------
	//	Add object to things that are lit by the given light set
	//--------------------------------------------------------------------
	void  AddObjectToLayer(const nameString& i_SetName, 
							  const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new lyrsAddObjectOperation(i_ObjectName));
		lyrsActualOperations::AddObjectToLayer(i_SetName, i_ObjectName);
	}

	//--------------------------------------------------------------------
	//	Remove object from things that are lit by the given light set
	//--------------------------------------------------------------------
	void  RemoveObjectFromLayer(const nameString& i_SetName, 
								   const nameString& i_ObjectName)
	{
		undoUndoMgr::AddOperation(new lyrsRemoveObjectOperation(i_ObjectName));
		lyrsActualOperations::RemoveObjectFromLayer(i_SetName, i_ObjectName);
	}

}	// end of namespace
