/*****************************************************************************
**	cmmObjectDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmObjectDialog.hpp"

#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"
//#include "Support/swl/swlScriptObject.hpp"
//#include "Support/swl/swlPropertyObject.hpp"
//#include "Support/swl/swlSelectionUtil.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/prty/prtyName.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default


//============================================================================
//============================================================================
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

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void fill_treectrl_drivers( wxTreeCtrl* io_pTreeCtrl, const tmlnDriverNameList& i_DriverNames )
	{
		fsysFileList driver_list;
		build_file_list(i_DriverNames, driver_list);

		gsupTreeCtrlUtil::PopulateTreeView(io_pTreeCtrl, driver_list, true);
		//gsupTreeCtrlUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
	}

	//--------------------------------------------------------------------
	// predicate for finding a UIInfo by category
	//--------------------------------------------------------------------
	struct category_match
	{
		std::string m_Category;

		category_match(const std::string &i_Category)
			: m_Category(i_Category) {}

		inline bool operator()(shared_ptr<prtyPropertyUIInfo>& info) 
		{ 
			return (info->GetCategory() == m_Category); 
		}
	};

	//--------------------------------------------------------------------
	// Sort peroperties by category, but do not put those categories
	// in sorted order, just keep them in the order they are found.
	//--------------------------------------------------------------------
	void sort_categories(const PropertyUIIList &i_OrginalList, 
						 PropertyUIIList& o_SortedList)
	{
		PropertyUIIList::const_iterator it;
		for (it = i_OrginalList.begin(); it != i_OrginalList.end(); ++it)
		{
			shared_ptr<prtyPropertyUIInfo> ui_info(*it);

			// Use reverse iterators to find the category because
			// we will likely find the category in the last UIInfo,
			// and because when we want to insert, we want to insert
			// after the last property that had this category name.
			//
			PropertyUIIList::reverse_iterator rit;
			rit = std::find_if(o_SortedList.rbegin(), o_SortedList.rend(), 
								category_match(ui_info->GetCategory()));

			if (rit == o_SortedList.rend())
			{
				// Category not found, add the UIInfo at the end
				o_SortedList.push_back(ui_info);
			}
			else
			{
				// A property with the same category was found, convert the
				// reverse iterator to a forward iterator and use that as the
				// insertion point
				PropertyUIIList::iterator fwd_it(rit.base());
				o_SortedList.insert(fwd_it, ui_info);
			}
		}
	}

	//--------------------------------------------------------------------
	// Sort peroperties by category, but do not put those categories
	// in sorted order, just keep them in the order they are found.
	//--------------------------------------------------------------------
	void sort_categories(prtyObject *i_pObject, prtyPropertyUIInfoContainer& o_SortedList)
	{
		const prtyPropertyUIInfoContainer org_list_ctr = i_pObject->GetListContainer();
		//const PropertyUIIList &org_list = i_pObject->GetList();
		sort_categories(org_list_ctr.GetList(), o_SortedList.GetPropertyUIInfoList());

		//bga - eventually we will have nested sub categories and this 
		// will have to do deeper recursion
		const int num_sub_categories = org_list_ctr.GetNumSubCategories();
		for (int i=0; i<num_sub_categories; ++i)
		{	
			shared_ptr<prtyPropertyUIInfoContainer> new_property_list(new prtyPropertyUIInfoContainer);
			o_SortedList.AddSubCategory(org_list_ctr.GetSubCategoryDisplayName(i),
										new_property_list);
			sort_categories(org_list_ctr.GetSubCategory(i).GetList(), 
							new_property_list->GetPropertyUIInfoList());
		}
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
								  const wxString& i_Title,
								  const wxString& i_Caption)
: cmmObjectDialogBase( parent )
{
	// Select Properties pane to display
	m_notebook1->SetSelection(0);

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Right());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmObjectDialog::~cmmObjectDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing cmmObjectDialog()");
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

	// Check for single or multiple selection
	const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
	if (selected_list.size() == 1)
	{
		//	if just one object fall to the "standard" update function
		//
		sel3dObject* pick_obj = sel3dMgr::GetSelected();
		
		std::string object_category("Object");
		//bga - not crazy about how this uses the timeline manager to 
		// get the object category, but seemed okay for now.
		if (tmlnScriptObject* pScriptObject = sel3dCastUtil::CastPickObject<tmlnScriptObject>(pick_obj))
			object_category = tmlnTimelineMgr::GetObjectCategory( pScriptObject );

		prtyObject *pObject = dynamic_cast<prtyObject*>(pick_obj);
		if (pObject)
			update_dialog( pObject, pick_obj->GetDisplayName(), object_category );
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
		pwxFormControlBuilder::InitForm( this->m_tabPage_Properties );

		//	loop through the selected items and build a single form
		//
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
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
		pwxFormControlBuilder::CreateControlsForForm(this->m_tabPage_Properties, true, false,
			(cmmObjectDialogUtil::GetViewPropertyKeyButtons()) ? &keyfKeyframeUtil::KeyProperty : NULL );

		// should we set the "Properties" tab to be the tab page selection?
	}

}

//--------------------------------------------------------------------
// Add and remove tab pages
//--------------------------------------------------------------------
void cmmObjectDialog::AddTabPage(wxPanel* i_pTabPage, const std::string i_Title, bool i_bSelectTab)
{
	m_notebook1->InsertPage(1, i_pTabPage, wxString(i_Title.c_str(), wxConvUTF8), i_bSelectTab);
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
void cmmObjectDialog::update_dialog(prtyObject* i_pObject, 
									const std::string& i_Pick3dName,
									const std::string& i_ObjectCategory)
{
	//	set the object name
	const prtyName* pName = dynamic_cast<const prtyName*>(i_pObject->GetProperty("Name"));
	this->set_object_name((pName) ? pName->GetString() : i_Pick3dName);

	//	add the properties
	//bga - This next lines control whether the object properties's categories
	// are in sorted order or not. When the shader parameters are created in order,
	// then we can use the second method (change the arguments to BuildForm also)
	// Sorts categories by name
	//i_pObject->SortListByCategory();
	// Sorts by category, but categories remain in order
	prtyPropertyUIInfoContainer sorted_list;
	sort_categories(i_pObject, sorted_list);

	pwxFormControlBuilder::BuildForm( this->m_tabPage_Properties, i_ObjectCategory,
		sorted_list, true, false, 
		//(i_pObject->GetListContainer()), true, false, 
		(cmmObjectDialogUtil::GetViewPropertyKeyButtons()) ? &keyfKeyframeUtil::KeyProperty : NULL );

	// If property object is read-only (i.e. locked materials),
	// disable whole panel. 
	// Maybe could just disable the property controls themselves sometime later.
	this->m_tabPage_Properties->Enable(!i_pObject->IsReadOnly());
	
	buildtreeview_drivers();
	//buildsoftware_lighting();
}

//--------------------------------------------------------------------
// set name label
//--------------------------------------------------------------------
void cmmObjectDialog::set_object_name(const std::string &i_Name)
{
	this->m_label_Name->SetLabel( wxString(i_Name.c_str(), wxConvUTF8) );
}

//--------------------------------------------------------------------
// build tree views of interface
//--------------------------------------------------------------------
void cmmObjectDialog::buildtreeview_drivers()
{
	//	add the drivers
	//
	tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
	if (script_obj != NULL)
	{
		// get the drivers and find if the driver exists
		//
		tmlnDriverNameList driver_names;
		tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);
		if (driver_names.Empty())
		{
			ShowTab(wxString(L"Drivers"), false);
			m_treeCtrl_Drivers->DeleteAllItems();
		}
		else
		{
			ShowTab(wxString(L"Drivers"), true);
			fill_treectrl_drivers( m_treeCtrl_Drivers, driver_names );
		}
	}
	else
	{
		// Clear out drivers tab
		m_treeCtrl_Drivers->DeleteAllItems();
	}
}

//--------------------------------------------------------------------
// show notebook tab
//
//	Note: this function doesn't work!
//	I'm not sure why.  I can't hide/disable the tab.  
//--------------------------------------------------------------------
void cmmObjectDialog::ShowTab(wxString& i_Name, bool i_bShow)
{
	return;

	for (int i=0; i < m_notebook1->GetPageCount(); ++i)
	{
		wxWindow* pWin = m_notebook1->GetPage(i);
		wxString Title = m_notebook1->GetPageText(i);

		if ((pWin != NULL) && (Title.IsSameAs(i_Name)))
		{
			pWin->Freeze();
			pWin->Enable(i_bShow);
			pWin->Show(i_bShow);
			pWin->Thaw();
		}
	}
}

////--------------------------------------------------------------------
//// build software lighting
////--------------------------------------------------------------------
//void cmmObjectDialog::buildsoftware_lighting()
//{
//	swlScriptObject* script_obj = swlSelectionUtil::GetSelectedScriptObject();
//	if (script_obj)
//	{
//		prtyPropertyUIInfoContainer sorted_list;
//		sort_categories(script_obj->GetPropertyUI(), sorted_list);
//
//		pwxFormControlBuilder::BuildForm( this->m_tabPage_SoftwareLighting, "",
//			sorted_list, true, false, NULL );
//
//		// If property object is read-only (i.e. locked materials),
//		// disable whole panel. 
//		// Maybe could just disable the property controls themselves sometime later.
//		this->m_tabPage_SoftwareLighting->Enable(!script_obj->GetPropertyUI()->IsReadOnly());
//	}
//	else
//	{
//		pwxFormControlBuilder::ClearForm(this->m_tabPage_SoftwareLighting);
//	}
//}

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
			std::string drivername = m_treeCtrl_Drivers->GetItemText(sel_node).utf8_str();
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
