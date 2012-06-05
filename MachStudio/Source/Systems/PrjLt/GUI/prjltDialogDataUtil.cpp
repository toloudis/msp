/*****************************************************************************
**	prjltDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/PrjLt/GUI/prjltDialogDataUtil.hpp"

#include "Systems/PrjLt/Object/prjltObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/ltst/ltstIsolateMgr.hpp"


namespace
{
	const char* c_SystemName = "Projected Lights";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  prjltDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	prjltDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void prjltDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = prjltObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		prjltData objdata = prjltObjectMgr::GetBaseData(i);
		cmmDialogData &dialog_data = io_DataList[startindex+i];
		dialog_data.m_Name		= objdata.m_Name.GetValue();
		dialog_data.m_Filename	= itString( objdata.m_Name.GetString().c_str() );
		dialog_data.m_Desc		= "";
		dialog_data.m_SystemName	= c_SystemName;
		dialog_data.m_bVisible	= objdata.m_bEditorVisible.GetValue();
		dialog_data.m_bEnabled	= ltstIsolateMgr::IsLightEnabled(objdata.m_Name.GetValue());
		dialog_data.m_pPickObject	= prjltObjectMgr::GetPickObject(i);
		dialog_data.m_GroupState	= cmmDialogData::e_LeafNode;
	}
}

