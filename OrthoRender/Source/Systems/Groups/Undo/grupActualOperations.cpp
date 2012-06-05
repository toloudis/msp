/*****************************************************************************
**	grupActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
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
