/*****************************************************************************
**	lyrsDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/Layers/GUI/lyrsDialogUtil.hpp"

#include "Systems/Layers/GUI/lyrsDialogDataUtil.hpp"
#include "Systems/Layers/Data/lyrsDocumentChunk.hpp"
#ifdef MSP_GUI_QT
#include "Systems/Layers/GUI/qtGUI/lyrsLayerObjectsPage.hpp"
#else
#include "Systems/Layers/GUI/wxGUI/lyrsLayerObjectsPage.hpp"
#endif
#include "Support/lyer/lyerLayerInterest.hpp"

#ifdef MSP_GUI_QT
#include "Systems/Common/GUI/qtGUI/cmmObjectDialog.hpp"
#else
#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#endif
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"


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
//	create_dialog();
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  lyrsDialogUtil::CleanUp()
{
	RemoveDataPage();
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

#if defined(USE_WXWIDGETS) || defined(USE_QT)
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
// Update specific pages when membership changed from something
// outside GUI checkboxes
//--------------------------------------------------------------------
void lyrsDialogUtil::UpdateObjectsPage()
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lyrsLayerObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsPage::Instance))
		{
			lyrsLayerObjectsPage::Instance->ReUpdate();
		}
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  lyrsDialogUtil::AddDataPage()
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (cmmObjectDialog::FormInstance)
	{
		if (!lyrsLayerObjectsPage::Instance)
			lyrsLayerObjectsPage::Instance = new lyrsLayerObjectsPage(cmmObjectDialog::FormInstance->GetNotebook());
		if (!cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsPage::Instance))
		{
			cmmObjectDialogUtil::AddTabPage( lyrsLayerObjectsPage::Instance, "Objects" );
		}
	}
#endif
}

void  lyrsDialogUtil::RemoveDataPage()
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lyrsLayerObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lyrsLayerObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lyrsLayerObjectsPage::Instance );
#ifdef USE_QT
			lyrsLayerObjectsPage::Instance->hide();
#else
			lyrsLayerObjectsPage::Instance->Show(false);
#endif
		}
	}
#endif
}
