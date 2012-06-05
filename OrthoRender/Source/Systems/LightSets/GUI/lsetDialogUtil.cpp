/*****************************************************************************
**	lsetDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"
#include "Systems/LightSets/GUI/lsetLightSetObjectsForm.h"
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetLightsPage.hpp"
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetObjectsPage.hpp"
#include "Support/ltst/ltstLightSetInterest.hpp"

#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace SystemLightSets;
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Callback when any data in ltstLightSetMgr changes
	//--------------------------------------------------------------------
	//class MyDataChanged : public ltstLightSetInterest
	//{
	//public:
	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectAdded()
	//	{
	//		update_light_sets_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRemoved(const nameString& i_Name)
	//	{
	//		lsetOperations::RemoveByName(i_Name);
	//		update_light_sets_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectRenamed - a object has been renamed
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRenamed()
	//	{
	//		lsetOperations::RefreshNames();
	//		update_light_sets_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	DataChanged - objects have been added/removed from 
	//	//		light sets
	//	//--------------------------------------------------------------------
	//	virtual void DataChanged()
	//	{
	//		update_light sets_dialog();
	//	}

	//private:
	//	//--------------------------------------------------------------------
	//	//--------------------------------------------------------------------
	//	void update_light_sets_dialog()
	//	{
	//		int i  = 0;
	//	}
	//};
	//
	//MyDataChanged l_DataChangedObj;
}	// end of namespace

//============================================================================
//============================================================================



//--------------------------------------------------------------------
// Init
//--------------------------------------------------------------------
void  lsetDialogUtil::Init()
{
//#ifdef _MANAGED
//	ltstLightSetMgr::RegisterLightSetInterest(&l_DataChangedObj);
//#endif

//	create_dialog();
#ifdef _MANAGED
	// Create tab page dialog
	if (!lsetLightSetObjectsForm::FormInstance)
	{
		lsetLightSetObjectsForm::FormInstance = gcnew lsetLightSetObjectsForm();
	}
#endif // _MANAGED
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  lsetDialogUtil::CleanUp()
{
//#ifdef _MANAGED
//	ltstLightSetMgr::UnRegisterLightSetInterest(&l_DataChangedObj);
//#endif

	RemoveDataPage();

#ifdef _MANAGED
	lsetLightSetObjectsForm::FormInstance = nullptr;
#endif // _MANAGED
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void lsetDialogUtil::UpdateListDialog()
{
	lsetDialogDataUtil::UpdateListDialog();
}

//--------------------------------------------------------------------
// Update common data tab page
//--------------------------------------------------------------------
void lsetDialogUtil::UpdateDialog(lsetScriptObject *i_pObject)
{
#ifdef _MANAGED
	if (lsetLightSetObjectsForm::FormInstance && i_pObject)
	{
		lsetLightSetObjectsForm::FormInstance->Update(i_pObject->GetName());
	}
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (i_pObject)
	{
		if (lsetLightSetLightsPage::Instance)
			lsetLightSetLightsPage::Instance->Update(i_pObject->GetName());
		if (lsetLightSetObjectsPage::Instance)
			lsetLightSetObjectsPage::Instance->Update(i_pObject->GetName());
	}
	else 
	{
		if (lsetLightSetLightsPage::Instance)
			lsetLightSetLightsPage::Instance->Clear();
		if (lsetLightSetObjectsPage::Instance)
			lsetLightSetObjectsPage::Instance->Clear();
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  lsetDialogUtil::AddDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (!cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::AddTabPage( lsetLightSetObjectsForm::FormInstance->GetTabPage(0) );
	if (!cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsForm::FormInstance->GetTabPage(1)))
		cmmObjectDialogUtil::AddTabPage( lsetLightSetObjectsForm::FormInstance->GetTabPage(1) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (cmmObjectDialog::FormInstance)
	{
//WXGUI
/*
		if (!lsetLightSetObjectsPage::Instance)
			lsetLightSetObjectsPage::Instance = new lsetLightSetObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( lsetLightSetObjectsPage::Instance, "Objects" );
		}
		if (!lsetLightSetLightsPage::Instance)
			lsetLightSetLightsPage::Instance = new lsetLightSetLightsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(lsetLightSetLightsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( lsetLightSetLightsPage::Instance, "Lights" );
		}
*/
	}
#endif
}

void  lsetDialogUtil::RemoveDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::RemoveTabPage( lsetLightSetObjectsForm::FormInstance->GetTabPage(0) );
	if (cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsForm::FormInstance->GetTabPage(1)))
		cmmObjectDialogUtil::RemoveTabPage( lsetLightSetObjectsForm::FormInstance->GetTabPage(1) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (lsetLightSetLightsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetLightsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lsetLightSetLightsPage::Instance );
			lsetLightSetLightsPage::Instance->Show(false);
		}
	}
	if (lsetLightSetObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lsetLightSetObjectsPage::Instance );
			lsetLightSetObjectsPage::Instance->Show(false);
		}
	}
#endif
}
