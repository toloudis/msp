/*****************************************************************************
**	grupActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/Undo/grupActualOperations.hpp"

#include "Systems/Groups/GUI/grupDialogDataUtil.hpp"
#include "Systems/Groups/GUI/grupDialogUtil.hpp"
#include "Systems/Groups/Data/grupDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new group
//--------------------------------------------------------------------
int  grupActualOperations::AddObject(const grupData& i_Data)
{
	int index = grupObjectMgr::AddObject(i_Data);
	grupDialogDataUtil::UpdateListDialog();
	grupDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in grupOperations
	grupObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete group with given index
//--------------------------------------------------------------------
void  grupActualOperations::DeleteObject(int i_Index)
{
	grupObjectMgr::DeleteObject(i_Index);
	grupDialogDataUtil::UpdateListDialog();
	grupDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Add object to things that are lit by the given light set
//--------------------------------------------------------------------
void  grupActualOperations::AddObjectToGroup(const nameString& i_SetName, 
						  const nameString& i_ObjectName)
{
	grpsGroupMgr::AddObjectToGroup(i_SetName, i_ObjectName);
	grupDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Remove object from things that are lit by the given light set
//--------------------------------------------------------------------
void  grupActualOperations::RemoveObjectFromGroup(const nameString& i_SetName, 
							   const nameString& i_ObjectName)
{
	grpsGroupMgr::RemoveObjectFromGroup(i_SetName, i_ObjectName);
	grupDocumentChunk::ActiveDataChanged();
}
