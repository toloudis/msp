/*****************************************************************************
**	trfnActualOperations.cpp
**
**	Everything outside of the "undo" folder should call a function
**	in the "Operations" namespace to make any change to the data.
**	This namespace is for internal use of the classes in the undo folder.
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/Undo/trfnActualOperations.hpp"

#include "Systems/Transforms/GUI/trfnDialogDataUtil.hpp"
#include "Systems/Transforms/Data/trfnDocumentChunk.hpp"

#include "Support/xfrm/xfrmTransformMgr.hpp"


//============================================================================
//============================================================================
//--------------------------------------------------------------------
//  Add new transform
//--------------------------------------------------------------------
int  trfnActualOperations::AddObject(const trfnScriptData& i_Data)
{
	int index = trfnObjectMgr::AddObject(i_Data);
	trfnDialogDataUtil::UpdateListDialog();
	trfnDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in trfnOperations
	trfnObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete transform with given index
//--------------------------------------------------------------------
void  trfnActualOperations::DeleteObject(int i_Index)
{
	trfnObjectMgr::DeleteObject(i_Index);
	trfnDialogDataUtil::UpdateListDialog();
	trfnDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
//	Add object to things that are lit by the given light set
//--------------------------------------------------------------------
void  trfnActualOperations::AddNodeToTransform(const nameString& i_SetName, 
						  const nameString& i_ObjectName)
{
	const bool c_bPreserveTransform = true; // Preserve Transform is the default
	xfrmTransformMgr::AddNodeToTransform(i_SetName, i_ObjectName, c_bPreserveTransform);
	trfnDocumentChunk::ActiveDataChanged();
	trfnDialogDataUtil::UpdateSceneHierarchy();
}

//--------------------------------------------------------------------
//	Remove object from things that are lit by the given light set
//--------------------------------------------------------------------
void  trfnActualOperations::RemoveNodeFromParent(const nameString& i_ObjectName)
{
	xfrmTransformMgr::RemoveNodeFromParent(i_ObjectName);
	trfnDocumentChunk::ActiveDataChanged();
	trfnDialogDataUtil::UpdateSceneHierarchy();
}
