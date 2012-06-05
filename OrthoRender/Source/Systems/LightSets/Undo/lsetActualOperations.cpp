/*****************************************************************************
**	lsetActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/Undo/lsetActualOperations.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new light set
//--------------------------------------------------------------------
int  lsetActualOperations::AddObject(const lsetScriptData& i_Data)
{
	int index = lsetObjectMgr::AddObject(i_Data);
	lsetDialogDataUtil::UpdateListDialog();
	lsetDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in lsetOperations
	lsetObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete light set with given index
//--------------------------------------------------------------------
void  lsetActualOperations::DeleteObject(int i_Index)
{
	lsetObjectMgr::DeleteObject(i_Index);
	lsetDialogDataUtil::UpdateListDialog();
	lsetDocumentChunk::ActiveDataChanged();
}
