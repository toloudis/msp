/*****************************************************************************
**	envtDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Environments/GUI/envtDialogDataUtil.hpp"

#include "Systems/Environments/Object/envtDefaultEnvironment.hpp"
#include "Systems/Environments/Object/envtObjectMgr.hpp"
#include "Systems/Environments/Object/envtSwlEnvironment.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

namespace
{
	const char* c_SystemName = "Environment Lights";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  envtDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	envtDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void envtDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = envtObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count + 2); // +1 for default env. +1 for swl env

	// add default env first.
	envtDefaultEnvironment* defaultEnv = envtObjectMgr::GetDefaultEnvironment();
	io_DataList[startindex].m_Name		= nameString(defaultEnv->GetDisplayName());
	io_DataList[startindex].m_Filename	= itString("");
	io_DataList[startindex].m_Desc		= "";
	io_DataList[startindex].m_SystemName	= c_SystemName;
	io_DataList[startindex].m_bVisible	= true;//false;//objdata.m_bEditorVisible.GetValue();
	io_DataList[startindex].m_pPickObject	= defaultEnv;
	startindex++;

	// add swl env first.
	envtSwlEnvironment* swlEnv = envtObjectMgr::GetSwlEnvironment();
	io_DataList[startindex].m_Name		= nameString(swlEnv->GetDisplayName());
	io_DataList[startindex].m_Filename	= itString("");
	io_DataList[startindex].m_Desc		= "";
	io_DataList[startindex].m_SystemName	= c_SystemName;
	io_DataList[startindex].m_bVisible	= true;//false;//objdata.m_bEditorVisible.GetValue();
	io_DataList[startindex].m_pPickObject	= swlEnv;
	startindex++;

	for (int i=0; i < count; ++i)
	{
		envtData objdata = envtObjectMgr::GetBaseData(i);
		io_DataList[startindex+i].m_Name		= objdata.m_Name.GetValue();
		io_DataList[startindex+i].m_Filename	= itString( io_DataList[startindex+i].m_Name.GetString().c_str() );
		io_DataList[startindex+i].m_Desc		= "";
		io_DataList[startindex+i].m_SystemName	= c_SystemName;
		io_DataList[startindex+i].m_bVisible	= true;//false;//objdata.m_bEditorVisible.GetValue();
		io_DataList[startindex+i].m_pPickObject	= envtObjectMgr::GetPickObject(i);
	}
}
