/*****************************************************************************
**	chtrDialogDataUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrDialogDataUtil.hpp"

#include "Systems/Character/Expressions/chtrExpressionObject.hpp"
#include "Systems/Character/GUI/chtrPartConstants.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/dyn/GUI/dynPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"


//============================================================================
//============================================================================
namespace
{
	const char* c_SystemName = "Characters";
}


//--------------------------------------------------------------------
//  Update dialog of the list
//--------------------------------------------------------------------
void  chtrDialogDataUtil::UpdateListDialog()
{
	cmmDialogDataList data_list;
	chtrDialogDataUtil::RebuildListData(data_list);
	
	cmmSystemDialogUtil::UpdatePlacedList(c_SystemName,data_list);
}

//--------------------------------------------------------------------
//	Rebuild the list data
//--------------------------------------------------------------------
void chtrDialogDataUtil::RebuildListData(cmmDialogDataList& io_DataList)
{
	//	Loop through all the data items and add the info to the data list
	//
	int count = chtrObjectMgr::GetNumObjects();
	int startindex = io_DataList.size();
	io_DataList.resize( startindex + count );
	for (int i=0; i < count; ++i)
	{
		chtrData objdata = chtrObjectMgr::GetBaseData(i);
		cmmDialogData &dialog_data = io_DataList[startindex+i];
		dialog_data.m_Name		= objdata.m_Name.GetValue();
		dialog_data.m_Filename	= objdata.m_Filename.GetValue();
		// use filename as description
		std::string desc = itStringUtil::GetStdString(objdata.m_Filename.GetValue());
		dialog_data.m_Desc			= desc;
		dialog_data.m_SystemName	= c_SystemName;
		dialog_data.m_bVisible		= objdata.m_bEditorVisible.GetValue();
		dialog_data.m_pPickObject	= chtrObjectMgr::GetPickObject(i);
		
		// Gather up part categories
		chtrScriptObject *pObject = chtrObjectMgr::GetObject(i);

		// Where should the define for the category name go?
		int num_surfaces = pObject->GetNumFragments();
		if (num_surfaces > 0)
		{
			std::vector<cmmDialogPartData> &surfaces = dialog_data.m_Parts[chtrPartConstants::c_SurfaceCategoryName];
			for (int i=0; i<num_surfaces; i++)
				surfaces.push_back(cmmDialogPartData(pObject->GetFragmentName(i),
													 pObject->GetFragmentUI(i)));
		}
		else
		{
			dialog_data.m_Parts[chtrPartConstants::c_SurfaceCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_SurfaceOverrideString));
		}
		
		int num_materials = pObject->GetNumMaterials();
		if (num_materials > 0)
		{
			std::vector<cmmDialogPartData> &materials = dialog_data.m_Parts[chtrPartConstants::c_MaterialCategoryName];
			for (int i=0; i<num_materials; i++)
				materials.push_back(cmmDialogPartData(pObject->GetMaterialName(i),
													 pObject->GetMaterialUI(i)));
		}
		else
		{
			dialog_data.m_Parts[chtrPartConstants::c_MaterialCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_MaterialOverrideString));
		}
	
		// Always option to make new control
		dialog_data.m_Parts[chtrPartConstants::c_ControlCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_NewControlString));
		int num_controls = pObject->GetNumControls();
		if (num_controls > 0)
		{
			std::vector<cmmDialogPartData> &controls = dialog_data.m_Parts[chtrPartConstants::c_ControlCategoryName];
			for (int i=0; i<num_controls; i++)
				controls.push_back(cmmDialogPartData(pObject->GetControlName(i),
													 pObject->GetControlUI(i)));
		}

		// Always option to make new expression
		const chtrExpressionObject* pExpObj = pObject->GetExpressionObject();
		dialog_data.m_Parts[chtrPartConstants::c_ExpressionCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_NewExpressionSingleString));
		dialog_data.m_Parts[chtrPartConstants::c_ExpressionCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_NewExpressionDualString));
		//dialog_data.m_Parts[chtrPartConstants::c_ExpressionCategoryName].push_back(cmmDialogPartData(chtrPartConstants::c_NewExpressionQuadString));
		int num_Expressions = pObject->GetNumExpressions();
		if (num_Expressions > 0)
		{
			std::vector<cmmDialogPartData> &expressions = dialog_data.m_Parts[chtrPartConstants::c_ExpressionCategoryName];
			for (int i=0; i<num_Expressions; i++)
			{
				chtrExpressionPropertyObject* pEPO = pExpObj->GetExpressionUI(i);
				expressions.push_back( cmmDialogPartData(pExpObj->GetExpressionName(i), pEPO) );
			}
		}
	}
}
