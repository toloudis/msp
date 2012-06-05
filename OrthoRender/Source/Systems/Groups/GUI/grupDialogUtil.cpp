/*****************************************************************************
**	grupDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Groups/GUI/grupDialogUtil.hpp"

#include "Systems/Groups/GUI/grupDialogDataUtil.hpp"
#include "Systems/Groups/Data/grupDocumentChunk.hpp"
#include "Systems/Groups/GUI/grupGroupObjectsForm.h"
#include "Systems/Groups/GUI/wxGUI/grupGroupObjectsPage.hpp"
#include "Support/grps/grpsGroupInterest.hpp"

#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace SystemGroups;
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Callback when any data in grpsGroupMgr changes
	//--------------------------------------------------------------------
	//class MyDataChanged : public grpsGroupInterest
	//{
	//public:
	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectAdded()
	//	{
	//		update_groups_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRemoved(const nameString& i_Name)
	//	{
	//		grupOperations::RemoveByName(i_Name);
	//		update_groups_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectRenamed - a object has been renamed
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRenamed()
	//	{
	//		grupOperations::RefreshNames();
	//		update_groups_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	DataChanged - objects have been added/removed from 
	//	//		groups
	//	//--------------------------------------------------------------------
	//	virtual void DataChanged()
	//	{
	//		update_groups_dialog();
	//	}

	//private:
	//	//--------------------------------------------------------------------
	//	//--------------------------------------------------------------------
	//	void update_groups_dialog()
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
void  grupDialogUtil::Init()
{
//#ifdef _MANAGED
//	grpsGroupMgr::RegisterGroupInterest(&l_DataChangedObj);
//#endif

//	create_dialog();
#ifdef _MANAGED
	// Create tab page dialog
	if (!grupGroupObjectsForm::FormInstance)
	{
		grupGroupObjectsForm::FormInstance = gcnew grupGroupObjectsForm();
	}
#endif // _MANAGED
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  grupDialogUtil::CleanUp()
{
//#ifdef _MANAGED
//	grpsGroupMgr::UnRegisterGroupInterest(&l_DataChangedObj);
//#endif

	RemoveDataPage();

#ifdef _MANAGED
	grupGroupObjectsForm::FormInstance = nullptr;
#endif // _MANAGED
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void grupDialogUtil::UpdateListDialog()
{
	grupDialogDataUtil::UpdateListDialog();
}

//--------------------------------------------------------------------
// Update common data tab page
//--------------------------------------------------------------------
void grupDialogUtil::UpdateDialog(grupGroupObject *i_pObject)
{
#ifdef _MANAGED
	if (grupGroupObjectsForm::FormInstance && i_pObject)
	{
		grupGroupObjectsForm::FormInstance->Update(i_pObject->GetName());
	}
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (i_pObject)
	{
		if (grupGroupObjectsPage::Instance)
			grupGroupObjectsPage::Instance->Update(i_pObject->GetName());
	}
	else 
	{
		if (grupGroupObjectsPage::Instance)
			grupGroupObjectsPage::Instance->Clear();
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  grupDialogUtil::AddDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (!cmmObjectDialogUtil::HasTabPage(grupGroupObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::AddTabPage( grupGroupObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (cmmObjectDialog::FormInstance)
	{
		if (!grupGroupObjectsPage::Instance)
			grupGroupObjectsPage::Instance = new grupGroupObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(grupGroupObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( grupGroupObjectsPage::Instance, "Objects" );
		}
	}
#endif
}

void  grupDialogUtil::RemoveDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (cmmObjectDialogUtil::HasTabPage(grupGroupObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::RemoveTabPage( grupGroupObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (grupGroupObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(grupGroupObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( grupGroupObjectsPage::Instance );
			grupGroupObjectsPage::Instance->Show(false);
		}
	}
#endif
}
