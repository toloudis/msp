/*****************************************************************************
**	cmmObjectDialog.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Support/gsup/gsupTreeCtrlUtil.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/prty/prtyName.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default

namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void build_file_list(const tmlnDriverNameList& i_DriverNames,
						 fsysFileList &o_DriverList)
	{
		fsysFileList connect_driver_list;
		int num_items = i_DriverNames.m_DriverNames.size();
		for (int i=0; i<num_items; i++)
		{
			fsLocator category;
			category.Push("Category");

			//	check to see if this is a "connect" driver.  If so, add to the connect
			//	list otherwise add it to the driver list.
			if (strstr(i_DriverNames.m_DriverNames[i].m_Name.c_str(), "Connect Channel") == 0)
			{
				category.Push( i_DriverNames.m_DriverNames[i].m_Category.c_str() );
				o_DriverList.AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
			}
			else
			{
				category.Push( "Connect Channel" );
				connect_driver_list.AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
			}
		}

		// Add connect drivers to end of list, sorted internally
		if (connect_driver_list.Size() > 0)
		{
			connect_driver_list.Sort();
			o_DriverList.AppendFromFileList( connect_driver_list);
		}
	}

	void fill_treectrl_drivers( wxTreeCtrl* io_pTreeCtrl, const tmlnDriverNameList& i_DriverNames )
	{
		fsysFileList driver_list;
		build_file_list(i_DriverNames, driver_list);

		gsupTreeCtrlUtil::PopulateTreeView(io_pTreeCtrl, driver_list, true);
		//gsupTreeCtrlUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
	}

}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cmmObjectDialog* cmmObjectDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmObjectDialog::cmmObjectDialog( wxWindow* parent,
								  const std::string& i_Title)
: cmmObjectDialogBase( parent )
{
	// Select Properties pane to display
	m_notebook1->SetSelection(0);

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Right());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmObjectDialog::~cmmObjectDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing cmmObjectDialog()");
	if (cmmObjectDialog::FormInstance == this)
		cmmObjectDialog::FormInstance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void cmmObjectDialog::Clear()
{
	//	clear the data
	this->set_object_name("");

	//	clear the trees
	m_treeCtrl_Drivers->DeleteAllItems();

	//	clear out the controls 
	pwxFormControlBuilder::ClearForm(this->m_tabPage_Properties);
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void cmmObjectDialog::UpdateDialog()
{	
	// The tab page is sometimes disabled for read-only objects. Just make sure it is 
	// enabled by default before the update.
	this->m_tabPage_Properties->Enable(true);

	// Check for sinlge or multiple selection
	const std::list<pick3dPickObject*>& selected_list = sel3dMgr::GetSelectedList();
	if (selected_list.size() == 1)
	{
		//	if just one object fall to the "standard" update function
		//
		pick3dPickObject* pick_obj = sel3dMgr::GetSelected();
		prtyObject *pObject = dynamic_cast<prtyObject*>(pick_obj);
		if (pObject)
			update_dialog( pObject, pick_obj->GetPick3dName() );
		else
			this->Clear();
	}
	else if (selected_list.size() > 1)
	{
		//	set the object name label
		this->set_object_name("Multiple selection");

		// Clear out drivers tab when multiple selection
		m_treeCtrl_Drivers->DeleteAllItems();

		//	clear the form before we add items to it.
		//
		pwxFormControlBuilder::InitForm( this->m_tabPage_Properties, this->m_labelDescription );

		//	loop through the selected items and build a single form
		//
		std::list<pick3dPickObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			prtyObject *pObject = dynamic_cast<prtyObject*>(*it);
			if (pObject != 0)
			{
				pwxFormControlBuilder::AddToFormList(pObject->GetList());
			}
		}

		//	sort then build the form
		//
		pwxFormControlBuilder::SortFormList(1); // 1 == sort by category
		pwxFormControlBuilder::CreateControlsForForm(this->m_tabPage_Properties, true);

		// should we set the "Properties" tab to be the tab page selection?
	}

}

//--------------------------------------------------------------------
// Add and remove tab pages
//--------------------------------------------------------------------
void cmmObjectDialog::AddTabPage(wxPanel* i_pTabPage, const std::string i_Title, bool i_bSelectTab)
{
	m_notebook1->InsertPage(1, i_pTabPage, i_Title, i_bSelectTab);
}
void cmmObjectDialog::RemoveTabPage(wxPanel* i_pTabPage)
{
	int page_count = m_notebook1->GetPageCount();
	for (int i=0; i<page_count; i++)
	{
		if (m_notebook1->GetPage(i) == i_pTabPage)
		{
			bool bNeedsNewSelectedPage = (m_notebook1->GetSelection() == i);
			m_notebook1->RemovePage(i);
			if (bNeedsNewSelectedPage)
				m_notebook1->SetSelection(0);
			return;
		}
	}	
}

//--------------------------------------------------------------------
// Returns true if the given tab page is attached.
//--------------------------------------------------------------------
bool cmmObjectDialog::HasTabPage(wxPanel* i_pTabPage)
{
	int page_count = m_notebook1->GetPageCount();
	for (int i=0; i<page_count; i++)
	{
		if (m_notebook1->GetPage(i) == i_pTabPage)
		{
			return true;
		}
	}	
	return false;
}

//--------------------------------------------------------------------
// Update the dialog with data from a single object
//--------------------------------------------------------------------
void cmmObjectDialog::update_dialog(prtyObject* i_pObject, const std::string& i_Pick3dName)
{
	//	set the object name
	const prtyName* pName = dynamic_cast<const prtyName*>(i_pObject->GetProperty("Name"));
	this->set_object_name((pName) ? pName->GetString() : i_Pick3dName);

	//	add the properties
	i_pObject->SortListByCategory();
	pwxFormControlBuilder::BuildForm( this->m_tabPage_Properties, 
		(i_pObject->GetList()), true, false, this->m_labelDescription );

	// If property object is read-only (i.e. locked materials),
	// disable whole panel. 
	// Maybe could just disable the property controls themselves sometime later.
	this->m_tabPage_Properties->Enable(!i_pObject->IsReadOnly());

	buildtreeview_drivers();
}

//--------------------------------------------------------------------
// set name label
//--------------------------------------------------------------------
void cmmObjectDialog::set_object_name(const std::string &i_Name)
{
	this->m_label_Name->SetLabel( i_Name );
}

//--------------------------------------------------------------------
// build tree views of interface
//--------------------------------------------------------------------
void cmmObjectDialog::buildtreeview_drivers()
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
		fill_treectrl_drivers( m_treeCtrl_Drivers, driver_names );
	}
	else
	{
		// Clear out drivers tab
		m_treeCtrl_Drivers->DeleteAllItems();
	}
}
//--------------------------------------------------------------------
// Choose selected item in list and create driver
//--------------------------------------------------------------------
void cmmObjectDialog::create_driver()
{
	wxTreeItemId sel_node = m_treeCtrl_Drivers->GetSelection();
	if (sel_node.IsOk())
	{
		// Has to be a leaf node in order to create a driver
		if (!m_treeCtrl_Drivers->ItemHasChildren(sel_node))
		{
			std::string drivername = m_treeCtrl_Drivers->GetItemText(sel_node);
			cmmObjectDialogUtil::CreateDriver( drivername );
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmObjectDialog::treeCtrl_Drivers_DoubleClick(wxTreeEvent& i_Event)
{
	create_driver();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmObjectDialog::button_AddDriver_Click(wxCommandEvent& i_Event)
{
	create_driver();
}


#endif // USE_WXWIDGETS
