/*****************************************************************************
**	cmmObjectDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Features/Channels/chnlDialogUtil.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/tmln/tmlnDriver.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Systems/Common/GUI/cmmAddDriverOperation.hpp"
#include "Systems/Common/GUI/mGUI/cmmObjectForm.h"
#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"

#include "Core/prty/prtyName.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace
{
	bool l_bShowPropertyKeys = false;
}


//============================================================================
//============================================================================
namespace cmmObjectDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{

#ifdef USE_WXWIDGETS
		if (cmmObjectDialog::FormInstance == NULL)
		{
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			cmmObjectDialog::FormInstance = new cmmObjectDialog(twxSystem::g_pMainForm);
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void CleanUp()
	{

#ifdef USE_WXWIDGETS
		// wxWidgets deletes the dialogs attached to the main form 
		// so we don't have to do it.
#endif
	}

	//--------------------------------------------------------------------
	// Display key buttons in front of property names
	//--------------------------------------------------------------------
	void SetViewPropertyKeyButtons(bool i_bVal)
	{
		l_bShowPropertyKeys = i_bVal;
		UpdateDialog();
		PrefsMgr::GetDataSimple().m_bPropKeyButtons = i_bVal;
	}
	bool GetViewPropertyKeyButtons()
	{
		return l_bShowPropertyKeys;
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
	}

	//--------------------------------------------------------------------
	//  Hide
	//--------------------------------------------------------------------
	void  Hide()
	{

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
					undoUndoMgr::AddOperation( new cmmAddDriverOperation(script_obj->CreateReferenceToSelf(), pDriver) );

					chnlDialogUtil::ObjectSelected(script_obj);

					//
					if (pDriver->IsAutoPopUpEditProperties())
						pDriver->DoEditProperties();
				}
			}
		}
	}


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

