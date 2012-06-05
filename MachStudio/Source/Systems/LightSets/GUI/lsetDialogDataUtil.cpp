/*****************************************************************************
**	lsetDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"

#include "Systems/LightSets/Object/lsetObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "LightSets";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  lsetDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	lsetDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//  Update tree view of light sets
//--------------------------------------------------------------------
void  lsetDialogDataUtil::UpdateSetRelationships()
{
	cmmSystemDialogUtil::UpdateSetRelationships();
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void lsetDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = lsetObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );

	for (int i=0; i < count; ++i)
	{
		lsetData objdata = lsetObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		io_DataList[startindex+i].m_Desc		= "";
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= true;//false;//objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= lsetObjectMgr::GetPickObject(i);
	}
}
