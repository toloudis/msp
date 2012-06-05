/*****************************************************************************
**	fogActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Undo/fogActualOperations.hpp"

#include "Systems/Fog/GUI/fogDialogDataUtil.hpp"
#include "Systems/Fog/GUI/fogDialogUtil.hpp"
#include "Systems/Fog/Data/fogDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new light set
//--------------------------------------------------------------------
int  fogActualOperations::AddObject(const fogScriptData& i_Data)
{
	int index = fogObjectMgr::AddObject(i_Data);
	fogDialogDataUtil::UpdateListDialog();
	fogDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in fogOperations
	fogObjectMgr::SelectObject();
	return 0;
}

//--------------------------------------------------------------------
//  Delete light set with given index
//--------------------------------------------------------------------
void  fogActualOperations::DeleteObject()
{
	fogObjectMgr::DeleteObject();
	fogDialogDataUtil::UpdateListDialog();
	fogDocumentChunk::ActiveDataChanged();
}


//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void fogActualOperations::ChangeDriverData(const fogScriptData& i_Data)
{
	// no need to notify aoObjectMgr if there is no undo, 
	// this message is coming from the object directly

	fogDocumentChunk::ActiveDataChanged();
}