/*****************************************************************************
**	aoActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/Undo/aoActualOperations.hpp"

#include "Systems/AmbientOcclusion/GUI/aoDialogDataUtil.hpp"
#include "Systems/AmbientOcclusion/GUI/aoDialogUtil.hpp"
#include "Systems/AmbientOcclusion/Data/aoDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new light set
//--------------------------------------------------------------------
int  aoActualOperations::AddObject(const aoScriptData& i_Data)
{
	int index = aoObjectMgr::AddObject(i_Data);
	aoDialogDataUtil::UpdateListDialog();
	aoDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in aoOperations
	aoObjectMgr::SelectObject();
	return 0;
}

//--------------------------------------------------------------------
//  Delete light set with given index
//--------------------------------------------------------------------
void  aoActualOperations::DeleteObject()
{
	aoObjectMgr::DeleteObject();
	aoDialogDataUtil::UpdateListDialog();
	aoDocumentChunk::ActiveDataChanged();
}
