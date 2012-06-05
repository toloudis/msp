/*****************************************************************************
**	prtclDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Particles/GUI/prtclDialogDataUtil.hpp"

#include "Systems/Particles/Object/prtclObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"



namespace
{
	const char* c_SystemName = "Particles";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  prtclDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	prtclDialogDataUtil::RebuildListData(data_list);
	
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void prtclDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = prtclObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		prtclData objdata = prtclObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= objdata.m_Filename.GetValue();
		// use filename as description
		std::string desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());
		io_DataList[startindex+i].m_Desc		= desc;
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= prtclObjectMgr::GetPickObject(i);
	}
}
