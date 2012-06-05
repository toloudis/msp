/*****************************************************************************
**	cmmObjectDialogUtil.cpp
**
**		API for opening the Object dialog for systems
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/mGUI/cmmObjectForm.h"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"

#include "Core/prty/prtyName.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIManaged/tma/tmaSystem.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"



//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
using namespace System::Windows::Forms;
#endif

//============================================================================
//============================================================================
namespace
{

#ifdef _MANAGED
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void add_drivers()
	{
		//	add the drivers
		//
		tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		if (script_obj)
		{
			// get the drivers and find if the driver exists
			//
			tmlnDriverNameList driver_names;
			tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);
			cmmObjectForm::FormInstance->Update( driver_names );
		}
	}

	//--------------------------------------------------------------------
	// Update the dialog
	//--------------------------------------------------------------------
	void update_dialog(prtyObject* i_pObject)
	{
		if (cmmObjectForm::FormInstance == nullptr)
			return;

		// Store current tab name in order to set it again later
		String^ currentTab = cmmObjectForm::FormInstance->GetCurrentTabName();

		//	set the object name
		const prtyName* pName = dynamic_cast<const prtyName*>(i_pObject->GetProperty("Name"));
		if (pName != 0)
		{
			cmmObjectForm::FormInstance->SetObjectName(pName->GetString());
		}
		else
		{
			cmmObjectForm::FormInstance->SetObjectName("");
		}

		//	add the properties
		i_pObject->SortListByCategory();
		//prtyFormControlBuilder::SetCategoryLabelWidth(1000);
		prtyFormControlBuilder::BuildForm( cmmObjectForm::FormInstance->GetPropertyTab(), 
			(i_pObject->GetList()), true, false, cmmObjectDialogUtil::GetDescriptionLabel() );

		add_drivers();

		// select the tab we had before, or properties if not
		if (currentTab != nullptr)
			cmmObjectForm::FormInstance->SelectTab(currentTab);
		else
			cmmObjectForm::FormInstance->SelectTab("Properties");
	}
#endif

}

//============================================================================
//============================================================================
namespace cmmObjectDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{
#ifdef _MANAGED
		// Create tab page dialog
		if (cmmObjectForm::FormInstance == nullptr)
		{
			cmmObjectForm::FormInstance = gcnew cmmObjectForm();

			// add form to main form
			tmaSystem::g_pMainForm->AddOwnedForm(cmmObjectForm::FormInstance);

			//	show the dialog if visible is set
			if (cmmObjectForm::FormInstance->GetVisibleFlag())
			{
				cmmObjectForm::FormInstance->Show();
			}
		}
#endif
#ifdef USE_WXWIDGETS
		if (cmmObjectDialog::FormInstance == NULL)
		{
			DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			cmmObjectDialog::FormInstance = new cmmObjectDialog(twxSystem::g_pMainForm);
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp()
	{
#ifdef _MANAGED
		cmmObjectForm::FormInstance = nullptr;
#endif
#ifdef USE_WXWIDGETS
		// wxWidgets deletes the dialogs attached to the main form 
		// so we don't have to do it.
#endif
	}

	//--------------------------------------------------------------------
	// Show
	//--------------------------------------------------------------------
	void  Show()
	{
		Init();

		UpdateDialog();

		// Notify the selection changed interests again, without changing the
		// selection. This causes the specialized tab pages to be added.
		sel3dMgr::Renotify();

#ifdef _MANAGED
		cmmObjectForm::FormInstance->Show();
		if (cmmObjectForm::FormInstance->WindowState == FormWindowState::Minimized)
			cmmObjectForm::FormInstance->WindowState = FormWindowState::Normal;
		cmmObjectForm::FormInstance->SelectTab("Properties");
#endif

#ifdef USE_WXWIDGETS
		//cmmObjectDialog::FormInstance->Show();
		twxPaneMgr::Show(cmmObjectDialog::FormInstance);
#endif
	}

	//--------------------------------------------------------------------
	//	ShowDrivers - like show but puts Drivers tab to the front
	//--------------------------------------------------------------------
	void  ShowDrivers()
	{
		Show();

#ifdef _MANAGED
		cmmObjectForm::FormInstance->SelectTab("Drivers");
#endif
	}

	//--------------------------------------------------------------------
	//  Hide
	//--------------------------------------------------------------------
	void  Hide()
	{
#ifdef _MANAGED
		if (cmmObjectForm::FormInstance)
			cmmObjectForm::FormInstance->Hide();
#endif
#ifdef USE_WXWIDGETS
		if (cmmObjectDialog::FormInstance)
		{
			cmmObjectDialog::FormInstance->Hide();
		}
#endif
	}

	//--------------------------------------------------------------------
	// Update the dialog
	//--------------------------------------------------------------------
	void UpdateDialog()
	{
#ifdef _MANAGED
		if (cmmObjectForm::FormInstance == nullptr)
			return;

		const std::list<pick3dPickObject*>& selected_list = sel3dMgr::GetSelectedList();
		if (selected_list.size() == 1)
		{
			//	if just one object fall to the "standard" update function
			//
			prtyObject *pObject = dynamic_cast<prtyObject*>(sel3dMgr::GetSelected());
			if (pObject == 0)
				return;
			update_dialog( pObject );
		}
		else if (selected_list.size() > 1)
		{
			//	set the object name
			std::string object_name("multiple objects");
			cmmObjectForm::FormInstance->SetObjectName(object_name);

			//	clear the form before we add items to it.
			//
			prtyFormControlBuilder::InitForm( cmmObjectForm::FormInstance->GetPropertyTab(), GetDescriptionLabel() );

			//	loop through the selected items and build a single form
			//
			std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
			for (it = selected_list.begin(); it != end; ++it)
			{
				prtyObject *pObject = dynamic_cast<prtyObject*>(*it);
				if (pObject != 0)
				{
					prtyFormControlBuilder::AddToFormList(pObject->GetList());
				}
			}

			//	sort then build the form
			//
			prtyFormControlBuilder::SortFormList(1);
			prtyFormControlBuilder::CreateControlsForForm(cmmObjectForm::FormInstance->GetPropertyTab(), true);

			// select the tab
			cmmObjectForm::FormInstance->SelectTab("Properties");
		}
#endif
#ifdef USE_WXWIDGETS
		if (cmmObjectDialog::FormInstance)
		{
			cmmObjectDialog::FormInstance->UpdateDialog();
		}
#endif
	}


	//--------------------------------------------------------------------
	// clear the dialog
	//--------------------------------------------------------------------
	void ClearDialog()
	{
#ifdef _MANAGED
		if (cmmObjectForm::FormInstance != nullptr)
		{
			cmmObjectForm::FormInstance->ClearTabs();
			cmmObjectForm::FormInstance->SetObjectName("");
		}
#endif
#ifdef USE_WXWIDGETS
		if (cmmObjectDialog::FormInstance)
		{
			cmmObjectDialog::FormInstance->Clear();
		}
#endif
	}

	//--------------------------------------------------------------------
	//	Create the driver
	//--------------------------------------------------------------------
	void CreateDriver(std::string& i_DriverName)
	{
		//
		tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
		if (script_obj)
		{
			// get the drivers and find if the driver exists
			//
			tmlnDriverNameList driver_names;
			tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);
			if (!driver_names.Empty())
			{
				//	find the index
				//
				int i;
				for (i=0; i< driver_names.m_DriverNames.size(); i++)
				{
					if (_stricmp(driver_names.m_DriverNames[i].m_Name.c_str(),i_DriverName.c_str()) == 0)
					{
						break;
					}
				}

				if ( i >= driver_names.m_DriverNames.size() )
				{
					return;
				}

				tmlnDriverNameList::DriverName driver_info = driver_names.m_DriverNames[i];

				// Create Driver
				//
				tmlnDriver* pDriver = driver_info.m_Creator->CreateDriverByName( i_DriverName.c_str(), script_obj );

				// Give driver to script object to own
				if (pDriver)
				{
					script_obj->AddDriver( pDriver );
					script_obj->NotifyDriverChanged();

					// Create an undo operation for this driver
					undoUndoMgr::AddOperation( new cmmAddDriverOperation(script_obj, pDriver) );

					chnlDialogUtil::ObjectSelected(script_obj);

					//
					if (pDriver->IsAutoPopUpEditProperties())
						pDriver->DoEditProperties();
				}
			}
		}
	}

#ifdef _MANAGED
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddTabPage(TabPage^ i_pTabPage, bool i_bSelectTab)
	{
		if (cmmObjectForm::FormInstance)
		{
			cmmObjectForm::FormInstance->AddTabPage(i_pTabPage);

			if (i_bSelectTab)
			{
				cmmObjectForm::FormInstance->SelectTab(i_pTabPage->Text);
			}
		}
	}

	void RemoveTabPage(TabPage^ i_pTabPage)
	{
		if (cmmObjectForm::FormInstance)
		{
			cmmObjectForm::FormInstance->RemoveTabPage(i_pTabPage);
		}
	}

	//--------------------------------------------------------------------
	// Returns true if the given tab page is attached.
	//--------------------------------------------------------------------
	bool HasTabPage(TabPage^ i_pTabPage)
	{
		if (cmmObjectForm::FormInstance)
		{
			return cmmObjectForm::FormInstance->HasTabPage(i_pTabPage);
		}
		return false;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	System::Windows::Forms::Label ^ GetDescriptionLabel()
	{
		if (cmmObjectForm::FormInstance)
		{
			return cmmObjectForm::FormInstance->GetDescriptionLabel();
		}
		return nullptr;
	}
#endif

#ifdef USE_WXWIDGETS
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void AddTabPage(wxPanel* i_pTabPage, const std::string i_Title, bool i_bSelectTab)
	{
		if (cmmObjectDialog::FormInstance)
		{
			cmmObjectDialog::FormInstance->AddTabPage(i_pTabPage, i_Title, i_bSelectTab);
		}
	}
	void RemoveTabPage(wxPanel* i_pTabPage)
	{
		if (cmmObjectDialog::FormInstance)
		{
			cmmObjectDialog::FormInstance->RemoveTabPage(i_pTabPage);
		}
	}

	//--------------------------------------------------------------------
	// Returns true if the given tab page is attached.
	//--------------------------------------------------------------------
	bool HasTabPage(wxPanel* i_pTabPage)
	{
		if (cmmObjectDialog::FormInstance)
		{
			return cmmObjectDialog::FormInstance->HasTabPage(i_pTabPage);
		}
		return false;
	}

#endif

}	// end of namespace

