/*****************************************************************************
**	dcutDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/DirectorsCut/GUI/dcutDialogDataUtil.hpp"

#include "Systems/DirectorsCut/Object/dcutDirectorsCutObject.hpp"
#include "Systems/DirectorsCut/Object/dcutObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


namespace
{
	const std::string c_SystemName("Director's Cuts");
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  dcutDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	dcutDialogDataUtil::RebuildListData(data_list);

	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName, data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void dcutDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = dcutObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		dcutCueData objdata = dcutObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( "" );
		// use filename as description
		io_DataList[startindex+i].m_Desc		= "";
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= true;
		io_DataList[startindex+i].m_pPickObject	= dcutObjectMgr::GetPickObject(i);
	}
}


