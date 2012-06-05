/*****************************************************************************
**	sbrdDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Storyboards/GUI/sbrdDialogDataUtil.hpp"

#include "Systems/Storyboards/Data/sbrdListData.hpp"
#include "Systems/Storyboards/Object/sbrdObjectMgr.hpp"
#include "Systems/Storyboards/GUI/sbrdStoryboardsForm.h"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_SystemName = "Storyboards";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  sbrdDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	sbrdDialogDataUtil::RebuildListData(data_list);
	
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName, data_list);

#ifdef _MANAGED
	if (SystemStoryboards::sbrdStoryboardsForm::FormInstance != nullptr)
	{
		sbrdListData& list_data = sbrdObjectMgr::GetListData();
		SystemStoryboards::sbrdStoryboardsForm::FormInstance->UpdateForm( list_data );
	}
#endif
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void sbrdDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = sbrdObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		sbrdObjectData objdata = sbrdObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= objdata.m_Filename.GetValue();

		// use filename as description
		DBG_LOG2("storyboard %d of %d", i, count);
		std::string desc;
		if (objdata.m_Filename.GetValue().GetLength() > 0)
			desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());

		io_DataList[startindex+i].m_Desc		= desc;
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= sbrdObjectMgr::GetPickObject(i);
	}
}

