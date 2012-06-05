/*****************************************************************************
**	cmraDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Cameras/GUI/cmraDialogDataUtil.hpp"

#include "Systems/Cameras/Object/cmraCameraObject.hpp"
#include "Systems/Cameras/Object/cmraObjectMgr.hpp"

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
	cmraDialogDataUtil::RebuildListData(data_list);

	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
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
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		// use filename as description
		io_DataList[startindex+i].m_Desc		= objdata.m_Description.GetValue();
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= cmraObjectMgr::GetPickObject(i);
	}
}

