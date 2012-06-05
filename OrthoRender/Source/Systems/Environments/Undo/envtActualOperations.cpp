/*****************************************************************************
**	envtActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/Undo/envtActualOperations.hpp"

#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"
#include "Systems/Environments/GUI/envtDialogUtil.hpp"
#include "Systems/Environments/Data/envtDocumentChunk.hpp"

//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new environment
//--------------------------------------------------------------------
int  envtActualOperations::AddObject(const envtScriptData& i_Data)
{
	int index = envtObjectMgr::AddObject(i_Data);
	envtDialogDataUtil::UpdateListDialog();
	envtDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in envtOperations
	envtObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete environment with given index
//--------------------------------------------------------------------
void  envtActualOperations::DeleteObject(int i_Index)
{
	envtObjectMgr::DeleteObject(i_Index);
	envtDialogDataUtil::UpdateListDialog();
	envtDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void envtActualOperations::ChangeDriverData(int i_Index, const envtScriptData& i_Data)
{
	// no need to notify envtObjectMgr if there is no undo, 
	// this message is coming from the object directly

	envtDialogUtil::UpdateEnvironmentData(i_Index, i_Data);
	envtDocumentChunk::ActiveDataChanged();;
}
