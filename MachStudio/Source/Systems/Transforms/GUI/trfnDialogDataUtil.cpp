/*****************************************************************************
**	trfnDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Transforms/GUI/trfnDialogDataUtil.hpp"

#include "Systems/Transforms/Object/trfnObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "Parents";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  trfnDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	trfnDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//  Update tree view of scene hierarchy
//--------------------------------------------------------------------
void  trfnDialogDataUtil::UpdateSceneHierarchy()
{
	cmmSystemDialogUtil::UpdateSceneHierarchy();
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void trfnDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = trfnObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count ); 
	for (int i=0; i < count; ++i)
	{
		trfnData objdata = trfnObjectMgr::GetBaseData(i);
		cmmDialogData &dialog_data = io_DataList[startindex+i];
		dialog_data.m_Name		= objdata.m_Name.GetValue();
		dialog_data.m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		dialog_data.m_Desc		= "";
		dialog_data.m_SystemName	= c_SystemName;
		dialog_data.m_bVisible	= true; // checked state set by child leaf nodes
		dialog_data.m_pPickObject	= trfnObjectMgr::GetPickObject(i);
		dialog_data.m_GroupState	= cmmDialogData::e_GroupingNode;
	}
}
