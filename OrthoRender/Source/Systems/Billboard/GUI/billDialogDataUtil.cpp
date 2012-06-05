/*****************************************************************************
**	billDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Billboard/GUI/billDialogDataUtil.hpp"

#include "Systems/Billboard/Object/billObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


namespace
{
	const char* c_SystemName = "Billboards";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  billDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	billDialogDataUtil::RebuildListData(data_list);
	
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void billDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = billObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		billData objdata = billObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= objdata.m_Filename.GetValue();
		// use filename as description
		std::string desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());
		io_DataList[startindex+i].m_Desc		= desc;
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= billObjectMgr::GetPickObject(i);
	}
}

