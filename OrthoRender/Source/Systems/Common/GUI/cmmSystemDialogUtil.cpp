/*****************************************************************************
**	cmmSystemDialogUtil.cpp
**
**		API for opening the Object dialog for systems
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"

#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneDialog.hpp"
#include "Systems/Common/GUI/cmmSystemDialogInterest.hpp"
#include "Systems/Common/GUI/mGUI/cmmSystemForm.h"
#include "ToolUIManaged/tma/tmaSystem.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
#endif


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
#ifdef _MANAGED
		// Create tab page dialog
		if (!cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance = gcnew cmmSystemForm();

			// add form to main form
			tmaSystem::g_pMainForm->AddOwnedForm(cmmSystemForm::FormInstance);
		}
#endif
#ifdef USE_WXWIDGETS
		DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
		cmmSceneDialog::FormInstance = new cmmSceneDialog(twxSystem::g_pMainForm);
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp()
	{
#ifdef _MANAGED
		cmmSystemForm::FormInstance = nullptr;
#endif
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
#ifdef _MANAGED
		//	if not intialized then do it.  Initialization should only
		//	happens at the beginning of the application launch.
		//
		if (!cmmSystemForm::FormInstance)
		{
			Init();

			if (!cmmSystemForm::FormInstance->GetVisibleFlag())
			{
				return;
			}
		}

		SceneDialogOpen();
		cmmSystemForm::FormInstance->Show();
#endif
	}

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void Show()
	{
#ifdef _MANAGED
		//	if not intialized then do it.  Initialization should only
		//	happens at the beginning of the application launch.
		//
		if (!cmmSystemForm::FormInstance)
		{
			Init();

			SceneDialogOpen();
		}

		if (cmmSystemForm::FormInstance->WindowState == FormWindowState::Minimized)
			cmmSystemForm::FormInstance->WindowState = FormWindowState::Normal;
		cmmSystemForm::FormInstance->Show();
#endif
		
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
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->Hide();
//			SceneDialogClose();
		}
#endif
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
		//	prtyFormControlBuilder::BuildForm( cmmSystemForm::FormInstance->GetPropertyTab(), (i_pObject->GetList()), true );

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
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->Clear();
			cmmSystemForm::FormInstance->Update();
		}
#endif
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
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance != nullptr)
		{
			SceneSetupData& data = SceneSetupDialogUtil::Data();
			cmmSystemForm::FormInstance->Update(data.m_PropertiesData);
		}
#endif
	}

#ifdef _MANAGED
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	TabPage^	GetTabPage(String^ i_TabName)
	{
		if (cmmSystemForm::FormInstance)
		{
			return cmmSystemForm::FormInstance->GetTabPage(i_TabName);
		}
		return nullptr;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddTabPage(TabPage^ i_pTabPage)
	{
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->AddTabPage(i_pTabPage);
		}
	}

	void RemoveTabPage(TabPage^ i_pTabPage)
	{
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->RemoveTabPage(i_pTabPage);
		}
	}
#endif

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
#ifdef _MANAGED
		//TabPage^ pTP = cmmSystemForm::FormInstance->GetTabPage("System");
		//if (pTP != 0)
		{
			cmmSystemForm::FormInstance->AddSystemCommand(	i_System, 
															i_DisplayText, 
															i_pCommand->GetTag(), 
															i_pCommand->GetObjectID() );
		}
#endif
	}

	//------------------------------------------------------------------------
	//	find the item on the placed tree an highlight it.
	//------------------------------------------------------------------------
	void SelectObjectOnPlacedList(pick3dPickObject* i_pPickObject)
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->SelectObjectOnPlacedList(i_pPickObject);
		}
#endif
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
	void AddToSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->AddToSelectedObjectsOnPlacedList(i_pPickObject);
		}
#endif
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
	void RemoveFromSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance)
		{
			cmmSystemForm::FormInstance->RemoveFromSelectedObjectsOnPlacedList(i_pPickObject);
		}
#endif
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
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance != nullptr)
		{
			cmmSystemForm::FormInstance->UpdatePlacedList(i_SystemName, i_DataList);
		}
#endif
#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdatePlacedList(i_SystemName, i_DataList);
		}
#endif
	}

	//------------------------------------------------------------------------
	// Update the available items (todo: within the given system name)
	//------------------------------------------------------------------------
	void UpdateAvailableList()
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance != nullptr)
		{
			cmmSystemForm::FormInstance->UpdateAvailableList();
		}
#endif
#ifdef USE_WXWIDGETS
		if (cmmSceneDialog::FormInstance)
		{
			cmmSceneDialog::FormInstance->UpdateAvailableList();
		}
#endif
	}

	//------------------------------------------------------------------------
	// Update display of list of picked objects
	//------------------------------------------------------------------------
	void UpdatePickList(const pick3dPickList& i_PickList, 
						const pick3dPickObject* i_pSelObj)
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance != nullptr)
		{
			cmmSystemForm::FormInstance->UpdatePickList(i_PickList, i_pSelObj);
		}
#endif
	}

	//------------------------------------------------------------------------
	// Clear display of picked objects
	//------------------------------------------------------------------------
	void ClearPickList()
	{
#ifdef _MANAGED
		if (cmmSystemForm::FormInstance != nullptr)
		{
			cmmSystemForm::FormInstance->ClearPickList();
		}
#endif
	}

	//
	//	Interest related functions
	//

	//--------------------------------------------------------------------
	//	RegisterInterest() - add an interest
	//--------------------------------------------------------------------
	void RegisterInterest( cmmSystemDialogInterest* i_pInterest )
	{
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

		l_cmmInterestList.push_back( i_pInterest );
		
		//DBG_LOG1("Scene Dialog Interest Added - %d entries", l_cmmInterestList.size());
	}

	//--------------------------------------------------------------------
	//	UnRegisterInterest() - remove an interest
	//
	//	Note: this will NOT delete the  interest.  It is up to the
	//	registerer.
	//--------------------------------------------------------------------
	void UnRegisterInterest( cmmSystemDialogInterest* i_pInterest )
	{
		DBG_ASSERT0( i_pInterest != 0, "NULL interest" );

		envSTLHelpers::RemoveOneValue( l_cmmInterestList, i_pInterest );

		//DBG_LOG1("Scene Dialog Interest Removed - %d entries left", l_cmmInterestList.size());
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
		//DBG_LOG1("Scene Dialog Opening - %d entries", l_cmmInterestList.size());

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
		//DBG_LOG1("Scene Dialog Closing - %d entries", l_cmmInterestList.size());

		for (int i = 0; i < l_cmmInterestList.size(); ++i)
		{
			l_cmmInterestList[i]->SceneDialogClose();
		}
	}

}	// end of namespace

