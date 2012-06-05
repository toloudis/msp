/*****************************************************************************
**	ptltDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/PtLt/GUI/ptltDialogDataUtil.hpp"

#include "Systems/PtLt/Object/ptltObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

//
//#include "Core/it/itStringUtil.hpp"

namespace
{
	const char* c_SystemName = "Point Lights";
}



//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  ptltDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	ptltDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void ptltDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = ptltObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		ptltData objdata = ptltObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		io_DataList[startindex+i].m_Desc		= "";
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= ptltObjectMgr::GetPickObject(i);
	}
}
