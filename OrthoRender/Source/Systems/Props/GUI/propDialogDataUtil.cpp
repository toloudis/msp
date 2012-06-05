/*****************************************************************************
**	propDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Props/GUI/propDialogDataUtil.hpp"

#include "Systems/Props/Object/propObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"

namespace
{
	const char* c_SystemName = "Props";
}

//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  propDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	propDialogDataUtil::RebuildListData(data_list);
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void propDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = propObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		propData objdata = propObjectMgr::GetBaseData(i);
		cmmDialogData &dialog_data = io_DataList[startindex+i];
		dialog_data.m_Name		= objdata.m_Name.GetValue();
		dialog_data.m_Filename	= objdata.m_Filename.GetValue();
		// use filename as description
		std::string desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());
		dialog_data.m_Desc		= desc;
		dialog_data.m_SystemName	= c_SystemName;
		dialog_data.m_bVisible	= objdata.m_bEditorVisible.GetValue();
		dialog_data.m_pPickObject	= propObjectMgr::GetPickObject(i);

		// Gather up part categories
		propScriptObject *pObject = propObjectMgr::GetObject(i);

		// Where should the define for the category name go?
		int num_surfaces = pObject->GetNumFragments();
		if (num_surfaces > 0)
		{
			std::vector<cmmDialogPartData> &surfaces = dialog_data.m_Parts["Surfaces"];
			for (int i=0; i<num_surfaces; i++)
				surfaces.push_back(cmmDialogPartData(pObject->GetFragmentName(i),
													 pObject->GetFragmentUI(i)));
		}
		else
			dialog_data.m_Parts["Surfaces"].push_back(cmmDialogPartData("Override Surface Flags"));
		
		int num_materials = pObject->GetNumMaterials();
		if (num_materials > 0)
		{
			std::vector<cmmDialogPartData> &materials = dialog_data.m_Parts["Materials"];
			for (int i=0; i<num_materials; i++)
				materials.push_back(cmmDialogPartData(pObject->GetMaterialName(i),
													 pObject->GetMaterialUI(i)));
		}
		else
			dialog_data.m_Parts["Materials"].push_back(cmmDialogPartData("Override Materials"));
	}
}

