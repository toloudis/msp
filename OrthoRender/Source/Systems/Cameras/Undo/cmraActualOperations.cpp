/*****************************************************************************
**	cmraActualOperations.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/Undo/cmraActualOperations.hpp"

#include "Systems/Cameras/Object/cmraScriptObject.hpp"
//#include "Systems/Cameras/Cue/cmraCueDataUtil.hpp"
//#include "Systems/Cameras/Cue/cmraCueDialogUtil.hpp"
#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"
#include "Systems/Cameras/GUI/cmraDialogUtil.hpp"
#include "Systems/Cameras/Data/cmraDocumentChunk.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Support/cmps/cmpsCompassMgr.hpp"


//--------------------------------------------------------------------
//  Add new camera to world
//--------------------------------------------------------------------
int  cmraActualOperations::AddObject(const cmraScriptData& i_Data)
{
	int index = cmraObjectMgr::AddObject(i_Data);
	if (index < 0) return index;	// if there is a problem loading the object, return immediately

	cmraDialogUtil::UpdateListDialog();

	//cmraCueDialogUtil::UpdateCameraNames();

	cmraDocumentChunk::ActiveDataChanged();

	// not sure if this should be here or in cmraOperations
	cmraObjectMgr::SelectObject(index);
	return index;
}

//--------------------------------------------------------------------
//  Delete camera with given index
//--------------------------------------------------------------------
void  cmraActualOperations::DeleteObject(int i_Index)
{
	cmraObjectMgr::DeleteObject(i_Index);

	cmraDialogUtil::UpdateListDialog();

	//cmraCueDataUtil::DeleteCamera(i_Index); // notify indices have changed
	//cmraCueDialogUtil::UpdateCameraNames();

	cmraDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual base properties
//--------------------------------------------------------------------
void cmraActualOperations::SetBaseData(int i_Index, const cmraCameraData& i_Data)
{
	cmraObjectMgr::SetBaseData(i_Index, i_Data);
	cmraDialogUtil::UpdateListDialog();

	cmraDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Update individual camera properties
//--------------------------------------------------------------------
void cmraActualOperations::SetData(int i_Index, const cmraScriptData& i_Data)
{
	if ( i_Index >= 0 )
	{
		cmraObjectMgr::SetScriptData(i_Index, i_Data);

		cmraDialogUtil::UpdateListDialog();
		//cmraCueDialogUtil::UpdateCameraNames();

		cmraDocumentChunk::ActiveDataChanged();
	}	
	else
	{
		camCamera &camera = cam3dMgr::GetEditorCamera();
		camera.SetFOV( i_Data.m_BaseData.m_FOV.GetValue() );
		camera.SetClip( i_Data.m_BaseData.m_Near.GetValue(), i_Data.m_BaseData.m_Far.GetValue() );
		camHDRData hdrData;
		hdrData.m_MiddleGray = i_Data.m_BaseData.m_HDRMiddleGray.GetValue();
		hdrData.m_BloomScale = i_Data.m_BaseData.m_HDRBloomScale.GetValue();
		hdrData.m_StarType = i_Data.m_BaseData.m_HDRStarType.GetValue();
		hdrData.m_StarScale = i_Data.m_BaseData.m_HDRStarScale.GetValue();
		hdrData.m_BrightPassThresh = i_Data.m_BaseData.m_HDRBrightPassThresh.GetValue();
		hdrData.m_BrightPassOffset = i_Data.m_BaseData.m_HDRBrightPassOffset.GetValue();
		hdrData.m_WhiteCutoff = i_Data.m_BaseData.m_HDRWhiteCutoff.GetValue();
		hdrData.m_SceneLuminance = i_Data.m_BaseData.m_HDRSceneLuminance.GetValue();
		camera.SetHDRParams(hdrData);
		if (i_Data.m_BaseData.m_bEnableDOF.GetValue())
		{
			camera.SetDOFParams(i_Data.m_BaseData.m_MaxFarBlur.GetValue(), 
				i_Data.m_BaseData.m_NearBlurDistance.GetValue(),
				i_Data.m_BaseData.m_NearFocalDistance.GetValue(),
				i_Data.m_BaseData.m_FarFocalDistance.GetValue(),
				i_Data.m_BaseData.m_FarBlurDistance.GetValue());
		}
		else
		{
			camera.SetDOFParams(-1,-1,-1,-1,-1);
		}
	}

}

//--------------------------------------------------------------------
// Update individual camera driver properties
//--------------------------------------------------------------------
void cmraActualOperations::ChangeDriverData(int i_Index, const cmraScriptData& i_Data)
{
	// no need to notify cmraObjectMgr if there is no undo, 
	// this message is coming from the object directly

	cmraDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Set Description
//--------------------------------------------------------------------
void cmraActualOperations::SetDescription(int i_Index, const nameString& i_Description)
{
	// first get the latest data, then set the position
	//
	cmraCameraData data = cmraObjectMgr::GetBaseData(i_Index);
	data.m_Description = i_Description.GetString().c_str();

	cmraObjectMgr::SetBaseData(i_Index, data);

	// we don’t need to do the first call since the Description isn’t changing.
	//
	cmraDialogUtil::UpdateListDialog();

	cmraDocumentChunk::ActiveDataChanged();
}


