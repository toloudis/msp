/*****************************************************************************
**	cmmSystemDialogUtil.cpp
**
**		API for opening the Object dialog for systems
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/mGUI/cmmSystemForm.h"
#include "Systems/Common/GUI/wxGUI/cmmSceneDialog.hpp"
#include "Systems/Common/GUI/cmmSceneOperations.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace
{
	SceneSetupData l_SceneSetupData;

	std::vector<cmmSystemDialogInterest*>	l_cmmInterestList;
}


//============================================================================
//============================================================================
namespace cmmSystemDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{
#ifdef USE_WXWIDGETS
		DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
		cmmSceneDialog::FormInstance = new cmmSceneDialog(twxSystem::g_pMainForm);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp()
	{
#ifdef USE_WXWIDGETS
		// It looks like wxWidgets deletes the dialogs attached to the main form 
		// so we don'thave to do it.
		//if (cmmSceneDialog::FormInstance)
		//{
		//	delete cmmSceneDialog::FormInstance;
		//	cmmSceneDialog::FormInstance = NULL;
		//}
#endif
	}

	//--------------------------------------------------------------------
	// ShowInitial - checks is this dialog is supposed to be visible or not
	//--------------------------------------------------------------------
	void ShowInitial()
	{
	}

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void Show()
	{
		
#ifdef USE_WXWIDGETS
		if (!cmmSceneDialog::FormInstance)
		{
			Init();
		}
		cmmSceneDialog::FormInstance->Update();
		twxPaneMgr::Show(cmmSceneDialog::FormInstance);
#endif
	}

	//--------------------------------------------------------------------
	//  Hide
	//--------------------------------------------------------------------
	void Hide()
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			twxPaneMgr::Show(cmmSceneDialog::FormInstance, false);
		}
#endif
	}

	//--------------------------------------------------------------------
	// Update the dialog
	//--------------------------------------------------------------------
	void UpdateDialog(prtyObject* i_pObject)
	{
		//if (cmmSystemForm::FormInstance != nullptr)
		//{
		//	//	add the properties
		//	i_pObject->SortListByCategory();
		//	prtyFormControlBuilder::BuildForm( cmmSystemForm::FormInstance->GetPropertyTab(), (i_pObject->GetListContainer()), true );

		//	//	add the drivers
		//	//
		//	tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		//	if (script_obj)
		//	{
		//		// get the drivers and find if the driver exists
		//		//
		//		tmlnDriverNameList driver_names;
		//		tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);
		//		cmmSystemForm::FormInstance->BuildTreeView( driver_names );
		//	}
		//}
	}

	void UpdateDialog()
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->Clear();
			cmmSceneDialog::FormInstance->UpdateDialog();
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateDialog_SceneProperties()
	{
	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void UpdateSceneProperties()
	{
		cmmSystemDialogUtil::UpdateDialog_SceneProperties();
	}

	//------------------------------------------------------------------------
	//	Add a command to the system tab page command tree.
	//------------------------------------------------------------------------
	void AddSystemCommand(const std::string& i_System, const std::string& i_DisplayText, const cmaCommand* i_pCommand )
	{
	}

	//------------------------------------------------------------------------
	//	find the item on the placed tree an highlight it.
	//------------------------------------------------------------------------
	void SelectObjectOnPlacedList(sel3dObject* i_pPickObject)
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->SelectObjectOnPlacedList(i_pPickObject);
		}
#endif
	}

	//------------------------------------------------------------------------
	//	Update placed list multiple selection and highlight
	//------------------------------------------------------------------------
	void AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->AddToSelectedObjectsOnPlacedList(i_pPickObject);
		}
#endif
	}

	//------------------------------------------------------------------------
	//	Update placed list multiple selection and highlight
	//------------------------------------------------------------------------
	void RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->RemoveFromSelectedObjectsOnPlacedList(i_pPickObject);
		}
#endif
	}
	
	//------------------------------------------------------------------------
	// Update the placed items within the given system name
	//------------------------------------------------------------------------
	void UpdatePlacedList(const std::string& i_SystemName, cmmDialogDataList& i_DataList)
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdatePlacedList(i_SystemName, i_DataList);
		}
#endif
	}

	//------------------------------------------------------------------------
	// Update the scene hierarchy tree view
	//------------------------------------------------------------------------
	void UpdateSceneHierarchy()
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdateSceneHierarchy();
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Update tree view of light sets
	//--------------------------------------------------------------------
	void  UpdateSetRelationships()
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdateSetRelationships();
		}
#endif
	}

	//------------------------------------------------------------------------
	// Update the available items (todo: within the given system name)
	//------------------------------------------------------------------------
	void UpdateAvailableList()
	{

#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdateAvailableList();
		}
#endif
	}

	//------------------------------------------------------------------------
	// Operations on selected objects
	//------------------------------------------------------------------------
	void DuplicateSelected()
	{
		cmmSceneOperations::DuplicateSelected();
	}
	void ReloadSelected()
	{
		cmmSceneOperations::ReloadSelected();
	}
	void DeleteSelected()
	{
		cmmSceneOperations::DeleteSelected();
	}

	//
	//	Interest related functions
	//

	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( cmmSystemDialogInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		l_cmmInterestList.push_back( i_pInterest );
		
		//DBG_LOG("Scene Dialog Interest Added - " << l_cmmInterestList.size() << " entries" );
	}

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( cmmSystemDialogInterest* i_pInterest )
	{
		DBG_ASSERT( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_cmmInterestList, i_pInterest );

		//DBG_LOG("Scene Dialog Interest Removed - " << l_cmmInterestList.size() << " entries left" );
	}

	//--------------------------------------------------------------------
	//	Clear() - clear the interest list
	//--------------------------------------------------------------------
	void ClearInterests()
	{
		l_cmmInterestList.clear();
	}

	//--------------------------------------------------------------------
	//	SceneDialogOpen - perform tasks (like adding tabs) relating
	//	to the scene/system dialog opening.  These tasks happen each time
	//	the scene dialog is launched.
	//--------------------------------------------------------------------
	void SceneDialogOpen()
	{
		//DBG_LOG("Scene Dialog Opening - " << l_cmmInterestList.size() << " entries" );

		for (int i = 0; i < l_cmmInterestList.size(); ++i)
		{
			l_cmmInterestList[i]->SceneDialogOpen();
		}
	}

	//--------------------------------------------------------------------
	//	SceneDialogClose - perform tasks (like adding tabs) relating
	//	to the scene/system dialog closing.  These tasks happen each time
	//	the scene dialog is closed.
	//--------------------------------------------------------------------
	void SceneDialogClose()
	{
		//DBG_LOG("Scene Dialog Closing - " << l_cmmInterestList.size() << " entries" );

		for (int i = 0; i < l_cmmInterestList.size(); ++i)
		{
			l_cmmInterestList[i]->SceneDialogClose();
		}
	}

}	// end of namespace

