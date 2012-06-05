/*****************************************************************************
**	propActualOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/Undo/propActualOperations.hpp"

#include "Systems/Props/GUI/propDialogDataUtil.hpp"
#include "Systems/Props/Data/propDocumentChunk.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Core/fs/fsResourceTracker.hpp"



//--------------------------------------------------------------------
//  Add new Prop to world
//--------------------------------------------------------------------
int  propActualOperations::AddObject(const propScriptData& i_Data)
{
	int index = propObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	propDialogDataUtil::UpdateListDialog();

	propDocumentChunk::ActiveDataChanged();

	propObjectMgr::SelectObject(index); // not sure if this should be here or in propOperations
	return index;
}

//--------------------------------------------------------------------
//  Delete Prop with given index
//--------------------------------------------------------------------
void   propActualOperations::DeleteObject(int i_Index)
{
	//	remove the file from the resource tracker
	propScriptObject* pSO = propObjectMgr::GetObject(i_Index);
	if (pSO != 0)
	{
		fsResourceTracker::Remove(pSO->GetBaseData().m_Filename.GetValue());
		//fsResourceTracker::Debug_OutputList();
	}

	//	do the actual deletion
	propObjectMgr::DeleteObject(i_Index);
	propDialogDataUtil::UpdateListDialog();
	propDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void  propActualOperations::ChangeDriverData(int i_Index, const propScriptData& i_Data)
{
	// no need to notify propObjectMgr if there is no undo, 
	// this message is coming from the object directly

	propDialogUtil::UpdatePropData(i_Index, i_Data);

	//propDocumentChunk::ActiveDataChanged();
}

