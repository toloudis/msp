/*****************************************************************************
**	lyrsDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"

#include "Systems/Layers/Object/lyrsObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "Layers";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  lyrsDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	lyrsDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void lyrsDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = lyrsObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count ); 
	for (int i=0; i < count; ++i)
	{
		lyrsData objdata = lyrsObjectMgr::GetData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		io_DataList[startindex+i].m_Desc		= "";
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= true;//false;//objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= lyrsObjectMgr::GetPickObject(i);
	}
}
