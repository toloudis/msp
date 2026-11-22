/*****************************************************************************
**	lsetDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/lsetDialogUtil.hpp"

#include "Systems/LightSets/GUI/lsetDialogDataUtil.hpp"
#include "Systems/LightSets/Data/lsetDocumentChunk.hpp"
#ifdef MSP_GUI_QT
#include "Systems/LightSets/GUI/qtGUI/lsetLightSetLightsPage.hpp"
#else
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetLightsPage.hpp"
#endif
#ifdef MSP_GUI_QT
#include "Systems/LightSets/GUI/qtGUI/lsetLightSetObjectsPage.hpp"
#else
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetObjectsPage.hpp"
#endif
#include "Support/ltst/ltstLightSetInterest.hpp"

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
//	create_dialog();
}

//--------------------------------------------------------------------
//  Clean up dialogs
//--------------------------------------------------------------------
void  lsetDialogUtil::CleanUp()
{
	RemoveDataPage();
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
void lsetDialogUtil::ClearDialog()
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lsetLightSetLightsPage::Instance)
		lsetLightSetLightsPage::Instance->Clear();
	if (lsetLightSetObjectsPage::Instance)
		lsetLightSetObjectsPage::Instance->Clear();
#endif // USE_WXWIDGETS
}
void lsetDialogUtil::UpdateDialog(const nameString &i_LightSetName)
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lsetLightSetLightsPage::Instance)
		lsetLightSetLightsPage::Instance->Update(i_LightSetName);
	if (lsetLightSetObjectsPage::Instance)
		lsetLightSetObjectsPage::Instance->Update(i_LightSetName);
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
// Update specific pages when membership changed from something
// outside GUI checkboxes
//--------------------------------------------------------------------
void lsetDialogUtil::UpdateObjectsPage()
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lsetLightSetObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsPage::Instance))
		{
			lsetLightSetObjectsPage::Instance->ReUpdate();
		}
	}
#endif // USE_WXWIDGETS
}
void lsetDialogUtil::UpdateLightsPage()
{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lsetLightSetLightsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetLightsPage::Instance))
		{
			lsetLightSetLightsPage::Instance->ReUpdate();
		}
	}
#endif // USE_WXWIDGETS
}

//--------------------------------------------------------------------
//  Add/Remove tab page from selected object dialog
//--------------------------------------------------------------------
void  lsetDialogUtil::AddDataPage()
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (cmmObjectDialog::FormInstance)
	{
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
	}
#endif
}

void  lsetDialogUtil::RemoveDataPage()
{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
	if (lsetLightSetLightsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetLightsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lsetLightSetLightsPage::Instance );
#ifdef USE_QT
			lsetLightSetLightsPage::Instance->hide();
#else
			lsetLightSetLightsPage::Instance->Show(false);
#endif
		}
	}
	if (lsetLightSetObjectsPage::Instance)
	{
		if (cmmObjectDialogUtil::HasTabPage(lsetLightSetObjectsPage::Instance))
		{
			cmmObjectDialogUtil::RemoveTabPage( lsetLightSetObjectsPage::Instance );
#ifdef USE_QT
			lsetLightSetObjectsPage::Instance->hide();
#else
			lsetLightSetObjectsPage::Instance->Show(false);
#endif
		}
	}
#endif
}
