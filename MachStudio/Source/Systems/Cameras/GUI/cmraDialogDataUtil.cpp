/*****************************************************************************
**	cmraDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

#include "Systems/Billboard/Object/billObjectMgr.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


namespace
{
	const char* c_SystemName = "Cameras";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  cmraDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	std::vector<shared_ptr<camCamera>> camera_list;
	std::vector<std::string> name_list;

	cmraDialogDataUtil::RebuildListData(data_list);
	cmraDialogDataUtil::RebuildCameraList(camera_list, name_list);

	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
	billObjectMgr::UpdateCameraList(camera_list, name_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void cmraDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = cmraObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count + 1 ); // plus one for editor camera

	// Create item for editor camera 
	io_DataList[startindex].m_Name			= nameString(cmraObjectMgr::GetEditorCameraObjectName());
	io_DataList[startindex].m_Filename		= itString("");
	io_DataList[startindex].m_Desc			= "";
	io_DataList[startindex].m_SystemName	= c_SystemName;
	io_DataList[startindex].m_bVisible		= true;
	io_DataList[startindex].m_pPickObject	= cmraObjectMgr::GetEditorCameraObject();
	startindex++;

	for (int i=0; i < count; ++i)
	{
		cmraCameraData objdata = cmraObjectMgr::GetBaseData(i);
		cmmDialogData &dialog_data = io_DataList[startindex+i];
		dialog_data.m_Name		= objdata.m_Name.GetValue();
		dialog_data.m_Filename	= itString( objdata.m_Name.GetString().c_str() );
		// use filename as description
		dialog_data.m_Desc		= objdata.m_Description.GetValue();
		dialog_data.m_SystemName	= c_SystemName;
		dialog_data.m_bVisible	= objdata.m_bEditorVisible.GetValue();
		dialog_data.m_pPickObject	= cmraObjectMgr::GetPickObject(i);
		dialog_data.m_GroupState	= cmmDialogData::e_LeafNode;
	}
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
//static 
void cmraDialogDataUtil::RebuildCameraList(std::vector<shared_ptr<camCamera>>& io_CameraList,
										std::vector<std::string>& io_NameList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = cmraObjectMgr::GetNumObjects();
	int startindex = io_CameraList.size();
	io_CameraList.resize( startindex + count + 1 ); // plus one for editor camera
	io_NameList.resize( startindex + count + 1 );

	// Create item for editor camera 
	io_CameraList[startindex] = cmraObjectMgr::GetEditorCameraObject()->GetCameraPtr();
	io_NameList[startindex] = cmraObjectMgr::GetEditorCameraObjectName();
	startindex++;
	for (int i=0; i < count; ++i)
	{
		io_CameraList[startindex + i] = cmraObjectMgr::GetPickObject(i)->GetCameraPtr();
		io_NameList[startindex + i] = cmraObjectMgr::GetBaseData(i).m_Name.GetValue().GetString();
	}
}

//--------------------------------------------------------------------
//	Delete
//--------------------------------------------------------------------
//static 
void cmraDialogDataUtil::DeleteObject(int i_Index)
{
	billObjectMgr::DeleteCameraIndex(i_Index);
}