/*****************************************************************************
**	billActualOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/Undo/billActualOperations.hpp"

#include "Systems/Billboard/GUI/billDialogDataUtil.hpp"
#include "Systems/Billboard/Data/billDocumentChunk.hpp"
#include "Systems/Billboard/Timeline/billDriverCreator.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//--------------------------------------------------------------------
//  Add new Billboard to world
//--------------------------------------------------------------------
int  billActualOperations::AddObject(const billScriptData& i_Data)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	int index = billObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	billDialogDataUtil::UpdateListDialog();
	billDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in billOperations
	billObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete Billboard with given index
//--------------------------------------------------------------------
void  billActualOperations::DeleteObject(int i_Index)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	//	remove the file from the resource tracker
	billScriptObject* pSO = billObjectMgr::GetObject(i_Index);
	if (pSO != 0)
	{
		fsResourceTracker::Remove(pSO->GetBaseData().m_Filename.GetValue());
		//fsResourceTracker::Debug_OutputList();
	}

	//	do the actual deletion
	billObjectMgr::DeleteObject(i_Index);
	billDialogDataUtil::UpdateListDialog();
	billDocumentChunk::ActiveDataChanged();
}

