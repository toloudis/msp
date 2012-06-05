/*****************************************************************************
**	aoDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/AmbientOcclusion/GUI/aoDialogDataUtil.hpp"

#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "AO";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  aoDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	aoDialogDataUtil::RebuildListData(data_list);

	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);

	// update the available list to make ao disappear if it has been added,
	// and reappear if deleted.
	cmmSystemDialogUtil::UpdateAvailableList();
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void aoDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//

	int startindex = io_DataList.size();
	io_DataList.resize( startindex + 1 );
	io_DataList[startindex].m_Name			= nameString("AO");
	io_DataList[startindex].m_Filename		= itString("");
	io_DataList[startindex].m_Desc			= "";
	io_DataList[startindex].m_SystemName	= c_SystemName;
	io_DataList[startindex].m_bVisible		= true;
	io_DataList[startindex].m_pPickObject	= aoObjectMgr::GetPickObject();
}

