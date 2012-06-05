/*****************************************************************************
**	dcutActualOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/Undo/dcutActualOperations.hpp"

#include "Systems/DirectorsCut/Object/dcutScriptObject.hpp"
#include "Systems/DirectorsCut/Cue/dcutCueDataUtil.hpp"
#include "Systems/DirectorsCut/Cue/dcutCueDialogUtil.hpp"
#include "Systems/DirectorsCut/GUI/dcutDialogDataUtil.hpp"
#include "Systems/DirectorsCut/GUI/dcutDialogUtil.hpp"
#include "Systems/DirectorsCut/Data/dcutDocumentChunk.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"


//--------------------------------------------------------------------
//  Add new camera to world
//--------------------------------------------------------------------
int  dcutActualOperations::AddObject(const dcutScriptData& i_Data)
{
	int index = dcutObjectMgr::AddObject(i_Data);

	dcutDialogUtil::UpdateListDialog();

	dcutCueDialogUtil::UpdateCameraNames();

	dcutDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in dcutOperations
	dcutObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  dcutActualOperations::DeleteObject(int i_Index)
{
	dcutObjectMgr::DeleteObject(i_Index);

	dcutDialogUtil::UpdateListDialog();

	dcutCueDataUtil::DeleteCamera(i_Index); // notify indices have changed
	dcutCueDialogUtil::UpdateCameraNames();

	dcutDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual base properties
//--------------------------------------------------------------------
void dcutActualOperations::SetBaseData(int i_Index, const dcutCueData& i_Data)
{
	dcutObjectMgr::SetBaseData(i_Index, i_Data);
	dcutDialogUtil::UpdateListDialog();

	dcutDialogUtil::UpdateCameraData(i_Index, i_Data);

	dcutDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual camera properties
//--------------------------------------------------------------------
void dcutActualOperations::SetData(int i_Index, const dcutScriptData& i_Data)
{
	if ( i_Index >= 0 )
	{
		dcutObjectMgr::SetScriptData(i_Index, i_Data);

		dcutDialogUtil::UpdateListDialog();
		dcutCueDialogUtil::UpdateCameraNames();

		dcutDocumentChunk::ActiveDataChanged();
	}	

	dcutDialogUtil::UpdateCameraData(i_Index, i_Data);
}

//--------------------------------------------------------------------
//	Select the camera and set the datadata (no matter what)
//--------------------------------------------------------------------
void dcutActualOperations::UpdateCameraData( int i_Index )
{
	//	set the data on click (in case the editor camera was
	//	last loaded)
	//
	dcutScriptObject* pCO = dcutObjectMgr::GetObject( i_Index );
	if ( pCO )
	{
		dcutScriptData data = pCO->GetScriptData();
		SetData(i_Index, data);
	}
}

//--------------------------------------------------------------------
// Update individual camera driver properties
//--------------------------------------------------------------------
void dcutActualOperations::ChangeDriverData(int i_Index, const dcutScriptData& i_Data)
{
	// no need to notify dcutObjectMgr if there is no undo, 
	// this message is coming from the object directly

	dcutDialogUtil::UpdateCameraData(i_Index, i_Data);

	dcutDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set Description
//--------------------------------------------------------------------
//void dcutActualOperations::SetDescription(int i_Index, const nameString& i_Description)
//{
//	// first get the latest data, then set the position
//	//
//	dcutCueData data = dcutObjectMgr::GetBaseData(i_Index);
//	data.m_Description = i_Description.GetString().c_str();
//
//	dcutObjectMgr::SetBaseData(i_Index, data);
//
//	// we don’t need to do the first call since the Description isn’t changing.
//	//
//	dcutDialogUtil::UpdateListDialog();
//
//	dcutDialogUtil::UpdateCameraData(i_Index, data);
//
//	dcutDocumentChunk::ActiveDataChanged();
//}


