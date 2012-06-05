/*****************************************************************************
**	giDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Systems/GlobalIllumination/GUI/giDialogDataUtil.hpp"

#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "GI";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  giDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	giDialogDataUtil::RebuildListData(data_list);

	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);

	// update the available list to make gi disappear if it has been added,
	// and reappear if deleted.
	cmmSystemDialogUtil::UpdateAvailableList();
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void giDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//

	int startindex = io_DataList.size();
	io_DataList.resize( startindex + 1 );
	io_DataList[startindex].m_Name			= nameString("GI");
	io_DataList[startindex].m_Filename		= itString("");
	io_DataList[startindex].m_Desc			= "";
	io_DataList[startindex].m_SystemName	= c_SystemName;
	io_DataList[startindex].m_bVisible		= true;
	io_DataList[startindex].m_pPickObject	= giObjectMgr::GetPickObject();
}

