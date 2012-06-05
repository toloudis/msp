/*****************************************************************************
**	lyrsActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/Undo/lyrsActualOperations.hpp"

#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"
#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"


//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new layer
//--------------------------------------------------------------------
int  lyrsActualOperations::AddObject(const lyrsData& i_Data)
{
	int index = lyrsObjectMgr::AddObject(i_Data);
	lyrsDialogDataUtil::UpdateListDialog();
	lyrsDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in lyrsOperations
	lyrsObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete layer with given index
//--------------------------------------------------------------------
void  lyrsActualOperations::DeleteObject(int i_Index)
{
	lyrsObjectMgr::DeleteObject(i_Index);
	lyrsDialogDataUtil::UpdateListDialog();
	lyrsDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Add object to things that are lit by the given light set
//--------------------------------------------------------------------
void  lyrsActualOperations::AddObjectToLayer(const nameString& i_SetName, 
						  const nameString& i_ObjectName)
{
	lyerLayerMgr::AddObjectToLayer(i_SetName, i_ObjectName);
	lyrsDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Remove object from things that are lit by the given light set
//--------------------------------------------------------------------
void  lyrsActualOperations::RemoveObjectFromLayer(const nameString& i_SetName, 
							   const nameString& i_ObjectName)
{
	lyerLayerMgr::RemoveObjectFromLayer(i_SetName, i_ObjectName);
	lyrsDocumentChunk::ActiveDataChanged();
}
