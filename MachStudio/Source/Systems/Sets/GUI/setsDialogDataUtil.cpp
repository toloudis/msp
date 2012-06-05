/*****************************************************************************
**	setsDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/GUI/setsDialogDataUtil.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/Data/setsScriptData.hpp"
#include "Systems/Sets/Data/setsObject.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Core/dbg/dbgMsg.hpp"


namespace
{
	const char* c_SystemName = "Sets";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  setsDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	setsDialogDataUtil::RebuildListData(data_list);
	//DBG_LOG("Update Sets List data size=" << data_list.size());
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName, data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void setsDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = setsDataMgr::GetNumSetItems();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		setsData objdata = setsDataMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= objdata.m_Filename.GetValue();
		// use filename as description
		std::string desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());
		io_DataList[startindex+i].m_Desc		= desc;
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= setsDataMgr::GetObject(i);
	}
}
