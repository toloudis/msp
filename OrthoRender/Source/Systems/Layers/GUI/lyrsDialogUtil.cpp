/*****************************************************************************
**	lyrsDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"

#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"
#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"
#include "Systems/Layers/GUI/lyrsLayerObjectsForm.h"
#include "Systems/Layers/GUI/wxGUI/lyrsLayerObjectsPage.hpp"
#include "Support/lyer/lyerLayerInterest.hpp"

#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace SystemLayers;
#endif

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	//--------------------------------------------------------------------
	// Callback when any data in lyerLayerMgr changes
	//--------------------------------------------------------------------
	//class MyDataChanged : public lyerLayerInterest
	//{
	//public:
	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectAdded()
	//	{
	//		update_layers_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectAdded - an object has been 
	//	//		added or removed to the system
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRemoved(const nameString& i_Name)
	//	{
	//		lyrsOperations::RemoveByName(i_Name);
	//		update_layers_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	ObjectRenamed - a object has been renamed
	//	//--------------------------------------------------------------------
	//	virtual void ObjectRenamed()
	//	{
	//		lyrsOperations::RefreshNames();
	//		update_layers_dialog();
	//	}

	//	//--------------------------------------------------------------------
	//	//	DataChanged - objects have been added/removed from 
	//	//		layers
	//	//--------------------------------------------------------------------
	//	virtual void DataChanged()
	//	{
	//		update_layers_dialog();
	//	}

	//private:
	//	//--------------------------------------------------------------------
	//	//--------------------------------------------------------------------
	//	void update_layers_dialog()
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
void  lyrsDialogUtil::Init()
{
//#ifdef _MANAGED
//	lyerLayerMgr::RegisterLayerInterest(&l_DataChangedObj);
//#endif

//	create_dialog();
#ifdef _MANAGED
	// Create tab page dialog
	if (!lyrsLayerObjectsForm::FormInstance)
	{
		lyrsLayerObjectsForm::FormInstance = gcnew lyrsLayerObjectsForm();
	}
#endif // _MANAGED
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  lyrsDialogUtil::CleanUp()
{
//#ifdef _MANAGED
//	lyerLayerMgr::UnRegisterLayerInterest(&l_DataChangedObj);
//#endif

	RemoveDataPage();

#ifdef _MANAGED
	lyrsLayerObjectsForm::FormInstance = nullptr;
#endif // _MANAGED
}

//--------------------------------------------------------------------
//	Update the dialog list
//--------------------------------------------------------------------
//static 
void lyrsDialogUtil::UpdateListDialog()
{
	lyrsDialogDataUtil::UpdateListDialog();
}

//--------------------------------------------------------------------
// Update common data tab page
//--------------------------------------------------------------------
void lyrsDialogUtil::UpdateDialog(lyrsLayerObject *i_pObject)
{
#ifdef _MANAGED
	if (lyrsLayerObjectsForm::FormInstance && i_pObject)
	{
		lyrsLayerObjectsForm::FormInstance->Update(i_pObject->GetName());
	}
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (i_pObject)
	{
		if (lyrsLayerObjectsPage::Instance)
			lyrsLayerObjectsPage::Instance->Update(i_pObject->GetName());
	}
	else 
	{
		if (lyrsLayerObjectsPage::Instance)
			lyrsLayerObjectsPage::Instance->Clear();
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  lyrsDialogUtil::AddDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (!cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::AddTabPage( lyrsLayerObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (cmmObjectDialog::FormInstance)
	{
//WXGUI
/*
		if (!lyrsLayerObjectsPage::Instance)
			lyrsLayerObjectsPage::Instance = new lyrsLayerObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( lyrsLayerObjectsPage::Instance, "Objects" );
		}
*/
	}
#endif
}

void  lyrsDialogUtil::RemoveDataPage()
{
#ifdef _MANAGED
	// Two tab pages, lights and objects
	if (cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsForm::FormInstance->GetTabPage(0)))
		cmmObjectDialogUtil::RemoveTabPage( lyrsLayerObjectsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
#ifdef USE_WXWIDGETS
	if (lyrsLayerObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lyrsLayerObjectsPage::Instance );
			lyrsLayerObjectsPage::Instance->Show(false);
		}
	}
#endif
}
