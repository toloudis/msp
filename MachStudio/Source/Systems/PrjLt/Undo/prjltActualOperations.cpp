/*****************************************************************************
**	prjltActualOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/Undo/prjltActualOperations.hpp"

#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"
#include "Systems/PrjLt/GUI/prjltDialogUtil.hpp"
#include "Systems/PrjLt/Data/prjltDocumentChunk.hpp"

#include "Support/cmps/cmpsCompassMgr.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//--------------------------------------------------------------------
//  Add new projected light to world
//--------------------------------------------------------------------
int  prjltActualOperations::AddObject(const prjltScriptData& i_Data)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	int index = prjltObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	prjltDialogDataUtil::UpdateListDialog();
	prjltDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in prjltOperations
	prjltObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete projected light with given index
//--------------------------------------------------------------------
void  prjltActualOperations::DeleteObject(int i_Index)
{
	// stop any render threads
	gpxRenderControl::ConfirmSingleThread();

	prjltObjectMgr::DeleteObject(i_Index);
	prjltDialogDataUtil::UpdateListDialog();
	prjltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual base properties
//--------------------------------------------------------------------
void prjltActualOperations::SetBaseData(int i_Index, const prjltData& i_Data)
{
	prjltObjectMgr::SetBaseData(i_Index, i_Data);
	prjltDialogDataUtil::UpdateListDialog();
	prjltDialogUtil::UpdateData(i_Index, i_Data);

	prjltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual projected light properties
//--------------------------------------------------------------------
void prjltActualOperations::SetData(int i_Index, const prjltScriptData& i_Data)
{
	prjltObjectMgr::SetScriptData(i_Index, i_Data);
	prjltDialogDataUtil::UpdateListDialog();
	prjltDialogUtil::UpdateData(i_Index, i_Data.m_BaseData);

	prjltDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual driver properties
//--------------------------------------------------------------------
void prjltActualOperations::ChangeDriverData(int i_Index, const prjltScriptData& i_Data)
{
	// no need to notify prjltObjectMgr if there is no undo, 
	// this message is coming from the object directly

	prjltDialogUtil::UpdateLightData(i_Index, i_Data);
	prjltDocumentChunk::ActiveDataChanged();
}


