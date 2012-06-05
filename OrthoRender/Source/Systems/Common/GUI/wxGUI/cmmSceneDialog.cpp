/*****************************************************************************
**	cmmSceneDialog.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmSceneDialog.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/App/appTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/pick3d/pick3dPickObject.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default

namespace
{
	const bool c_bExpandObjectParts = false; // Should object parts be expanded by default?

	//typedef std::set<wxTreeItemId> TreeItemIdSet;
	typedef std::list<wxTreeItemIdValue> TreeItemIdSet;

	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum PlacedImageState
	{
		e_Unchecked = 0,
		e_Checked = 1,
		e_MixChecked = 2,
		e_ControlPart,
		e_FirstPartIndex = e_ControlPart,
		e_ExpressionPart,
		e_MaterialPart,
		e_SurfacePart,
		e_NumPlacedImageStates
	};

	//--------------------------------------------------------------------
	// Tree item data contains index into m_DataListPlaced
	//--------------------------------------------------------------------
	class PlacedTreeItemData : public wxTreeItemData
	{
	public:
		PlacedTreeItemData(int i_Index, bool i_bIsPart)
			: m_Index(i_Index), m_bIsPart(i_bIsPart) {}
		int m_Index;
		bool m_bIsPart;
	};


	//--------------------------------------------------------------------
	// Assuming that i_Parent is a system node in the placed tree,
	// find the child node that has the given index.
	//--------------------------------------------------------------------
	wxTreeItemId find_node_with_index(wxTreeCtrl* io_pTreeCtrl,
										wxTreeItemId i_Parent,  
										int i_Index)
	{

		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			//if (!io_pTreeCtrl->ItemHasChildren(id))
			if (io_pTreeCtrl->GetItemData(id))
			{
				PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(io_pTreeCtrl->GetItemData(id));
				//DBG_ASSERT0(pData, "All placed tree item leaves should have item data with index.");

				if (i_Index == pData->m_Index)
					break;
			}
		}
		return id; // id.IsOk() == false if not found
	}

	//--------------------------------------------------------------------
	// Look at all nodes two levels deep (the objects in placed tree)
	//	and find the node with the given index, based on our 
	//	tree data type.
	//--------------------------------------------------------------------
	wxTreeItemId find_node_with_index(wxTreeCtrl* io_pTreeCtrl, 
										int i_Index)
	{
		wxTreeItemId root = io_pTreeCtrl->GetRootItem();
		DBG_ASSERT0(root.IsOk(), "Assuming that there is a root node already in tree control.");

		// Iterate through children
		wxTreeItemId id, found_node;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(root, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(root, cookie))
		{
			found_node = find_node_with_index(io_pTreeCtrl, id, i_Index);
			if (found_node.IsOk())
				return found_node;
		}

		return found_node; // will be IsOk() == false if not found
	}

	//------------------------------------------------------------------------
	// See if a node with given text exists under the given parent node.
	// If not, create it.
	//------------------------------------------------------------------------
	wxTreeItemId find_or_create_node(wxTreeCtrl* io_pTreeCtrl,
									 TreeItemIdSet& io_ItemsToDelete,
									 wxTreeItemId i_Parent,
									 const std::string& i_Text,
									 int i_ImageIndex = -1,
									 int i_ObjectIndex = -1,
									 bool i_bIsObjectPart = false,
									 bool i_bCreateAlways = false)
	{
		wxTreeItemId child_node;

		// In some cases, we know we will always need to create a new node
		// and can skip the "find" step.
		if (!i_bCreateAlways)
			child_node = gsupTreeCtrlUtil::FindNode(io_pTreeCtrl, i_Parent, i_Text);

		if (!child_node.IsOk())
		{
			// Need to create the node 
			child_node = io_pTreeCtrl->AppendItem(i_Parent, i_Text, i_ImageIndex);
			if (i_ObjectIndex >= 0)
				io_pTreeCtrl->SetItemData(child_node, new PlacedTreeItemData(i_ObjectIndex, i_bIsObjectPart));
			
			// Sort only if not a part. Parts are sorted later
			if (!i_bIsObjectPart)
				io_pTreeCtrl->SortChildren(i_Parent);
		}
		else 
		{
			envSTLHelpers::RemoveOneValue(io_ItemsToDelete, child_node);
			if (wxTreeItemData *pItemData = io_pTreeCtrl->GetItemData(child_node))
			{
				PlacedTreeItemData *pData = static_cast<PlacedTreeItemData*>(pItemData);
				//DBG_ASSERT0(pData, "All item data should have index.");
				pData->m_Index = i_ObjectIndex;
			}
		}
		return child_node;
	}
	
	//------------------------------------------------------------------------
	// Get the ids of all nodes under the given parent node
	//------------------------------------------------------------------------
	void get_all_tree_items(wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Parent, 
							TreeItemIdSet& o_TreeIds)
	{
		wxTreeItemIdValue cookie;
		for (wxTreeItemId id = i_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = i_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			// recurse on children first because we will remove them in that order
			if (i_pTreeCtrl->ItemHasChildren(id))
				get_all_tree_items(i_pTreeCtrl, id, o_TreeIds);

			// then add the parent
			o_TreeIds.push_back(id.m_pItem);
			//DBG_LOG2("Added tree id %x, num ids: %d", id, o_TreeIds.size());

		}
	}

	//------------------------------------------------------------------------
	// Get the ids of all nodes in the tree (not the invisible root).
	//------------------------------------------------------------------------
	void get_all_tree_items(wxTreeCtrl* i_pTreeCtrl, 
							TreeItemIdSet& o_TreeIds)
	{
		wxTreeItemId root = i_pTreeCtrl->GetRootItem();
		DBG_ASSERT0(root.IsOk(), "Assuming that there is a root node already in tree control.");
		get_all_tree_items(i_pTreeCtrl, root, o_TreeIds);
	}

	//--------------------------------------------------------------------
	// Return depth of the node (number of parent nodes). Notice
	//	that there is an invisible root node in the available and
	//	placed tree controls.
	//--------------------------------------------------------------------
	int get_depth_for_node(wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node)
	{
		if (!i_Node.IsOk())
			return 0;

		return (1 + get_depth_for_node(i_pTreeCtrl, i_pTreeCtrl->GetItemParent(i_Node)));
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void get_path_to_node(wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node,
						   fsLocator &o_Path)
	{
		wxTreeItemId parent_node = i_pTreeCtrl->GetItemParent(i_Node);
		if (parent_node.IsOk())
		{
			// Recurse on parent, push our node name after the
			// parent nodes have already contributed to path
			get_path_to_node(i_pTreeCtrl, parent_node, o_Path);

			// We don't want to add the root, so only add this node name
			// if we have a parent node.
			std::string node_name = i_pTreeCtrl->GetItemText(i_Node);
			DBG_LOG1("Adding nodename, %s, to path", node_name.c_str());
			o_Path.Push(node_name.c_str());
		}
	}

		
	//------------------------------------------------------------------------
	// Set color of item in order to indicate multiple selection in a 
	//	single selection tree control.
	//------------------------------------------------------------------------
	void highlight_tree_item(wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node,
						   bool i_bHighlight)
	{
		if (i_bHighlight)
		{	
			i_pTreeCtrl->SetItemTextColour(i_Node, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
			i_pTreeCtrl->SetItemBackgroundColour(i_Node,  wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );
			//i_pTreeCtrl->SetItemBackgroundColour(i_Node,  wxSystemSettings::GetColour( wxSYS_COLOUR_INACTIVECAPTION ) );
		}
		else
		{
			i_pTreeCtrl->SetItemTextColour(i_Node, i_pTreeCtrl->GetForegroundColour() );
			i_pTreeCtrl->SetItemBackgroundColour(i_Node,  i_pTreeCtrl->GetBackgroundColour() );
		}
	}

	//------------------------------------------------------------------------
	// Return current checked state of tree control item.
	// Since there is no support for checked items in wxWidgets, look at
	// the image state we are using.
	//------------------------------------------------------------------------
	bool get_item_checked( wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node)
	{
		return (i_pTreeCtrl->GetItemImage(i_Node) != e_Unchecked);
	}
	void set_item_checked( wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node,
						   bool i_bChecked)
	{
		return (i_pTreeCtrl->SetItemImage(i_Node, (i_bChecked) ? e_Checked : e_Unchecked));
	}

	//------------------------------------------------------------------------
	// Set checked state of tree node based on checked state of children
	//------------------------------------------------------------------------
	void update_parent_checked_state(wxTreeCtrl* i_pTreeCtrl,
									 wxTreeItemId i_Parent)
	{
		bool bAllChecked = true;
		bool bAllUnchecked = true;

		wxTreeItemIdValue cookie;
		for (wxTreeItemId id = i_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = i_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			if (get_item_checked(i_pTreeCtrl, id))
				bAllUnchecked = false;
			else
				bAllChecked = false;
		}

		if (bAllChecked)
			i_pTreeCtrl->SetItemImage(i_Parent, e_Checked);
		else if (bAllUnchecked)
			i_pTreeCtrl->SetItemImage(i_Parent, e_Unchecked);
		else
			i_pTreeCtrl->SetItemImage(i_Parent, e_MixChecked);
	}

	//------------------------------------------------------------------------
	// Used for finding part by pick object
	//------------------------------------------------------------------------
	class pick_object_search
	{
	public:
		pick_object_search(const pick3dPickObject* i_Object) : m_Object(i_Object) {};
		bool operator () ( const cmmDialogPartData& i_PartData )
		{
			return (m_Object == i_PartData.m_pPickObject);
		}
		const pick3dPickObject* m_Object;
	};

	//------------------------------------------------------------------------
	// Find tree node in the placed list that represents the given 
	//	pick object
	//------------------------------------------------------------------------
	wxTreeItemId find_node_for_pick_object(const cmmDialogDataList& i_DataListPlaced,
											wxTreeCtrl* i_pTreeCtrl,
											pick3dPickObject *i_pPickObject)
	{
		wxTreeItemId found_node;
		cmmDialogDataList::const_iterator it;
		int index = 0;
		for(it = i_DataListPlaced.begin(); it != i_DataListPlaced.end(); it++, index++)
		{
			if (it->m_pPickObject == i_pPickObject)
			{
				// Look for the tree node with this index
				found_node = find_node_with_index(i_pTreeCtrl, index);
				return found_node;
			}
			else if (!it->m_Parts.empty())
			{
				std::string part_name;
				std::map<std::string, std::vector<cmmDialogPartData> >::const_iterator category_it;
				for (category_it = it->m_Parts.begin(); category_it != it->m_Parts.end(); ++category_it)
				{
					std::vector<cmmDialogPartData>::const_iterator part_it =
						std::find_if(category_it->second.begin(), category_it->second.end(), pick_object_search(i_pPickObject));
					if (part_it != category_it->second.end())
					{
						// got the part, find the node for the part using
						// category and part name
						wxTreeItemId object_node = find_node_with_index(i_pTreeCtrl, index);
						if (object_node.IsOk())
						{
							wxTreeItemId ctgry_node = gsupTreeCtrlUtil::FindNode(i_pTreeCtrl, object_node, category_it->first);
							if (ctgry_node.IsOk())
							{
								found_node = gsupTreeCtrlUtil::FindNode(i_pTreeCtrl, ctgry_node, part_it->m_Name);
								return found_node;
							}
						}
					}
				}
			}
		}
		return found_node;
	}
}	// end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cmmSceneDialog* cmmSceneDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneDialog::cmmSceneDialog( wxWindow* parent, 
								const std::string& i_Title ) 
: cmmSceneDialogBase( parent ),
	m_bOurChange(false),
	m_bCtrlPressed(false), 
	m_bShiftPressed(false)
{
	// Select Available pane to display
	m_Notebook1->SetSelection(0);

	// Create state images to represent checked state
	wxImageList *pCheckImages = new wxImageList(13, 13, false, e_NumPlacedImageStates);
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-unchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-checked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-mixchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-controlpart.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-expressionpart.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-materialpart.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("./Data/Icons/tree-surfacepart.png", wxBITMAP_TYPE_PNG));
	m_treeCtrl_Placed->AssignImageList(pCheckImages); // tree ctrl takes ownership

	// Add this panel to the AUI manager
	twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(i_Title).Caption(i_Title).Show().Layer(1).Left());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmSceneDialog::~cmmSceneDialog()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing cmmSceneDialog()");
	if (cmmSceneDialog::FormInstance == this)
		cmmSceneDialog::FormInstance = NULL;
}

	
//--------------------------------------------------------------------
// Clear scene data from form
//--------------------------------------------------------------------	
void cmmSceneDialog::Clear()
{
	//	clear the data
	//m_pDataListPlaced->clear();

	//	clear the trees
	m_treeCtrl_Available->DeleteAllItems();
	m_treeCtrl_Placed->DeleteAllItems();

	//	change the button states
	enable_placed_buttons();
	//enable_available_buttons();
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdateDialog()
{
	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		buildtreeview_system_available();
		buildtreeview_system_placed();
					
		enable_placed_buttons();

//		SceneSetupData& data = SceneSetupDialogUtil::Data();
//		Update(data.m_PropertiesData);
	}
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdatePlacedList(const std::string& i_SystemName, 
									  cmmDialogDataList& i_DataList)
{
	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		cmmDialogInterestMgr::UpdateList(i_SystemName, i_DataList, m_DataListPlaced);

		//Note: could handle this better by searching for only the needed changes
		updatetreeview_system_placed();

	}
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmSceneDialog::UpdateAvailableList()
{
	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		//Note: could handle this better by searching for only the needed changes
		buildtreeview_system_available();
	}
}

//------------------------------------------------------------------------
//	Find the item on the placed tree an highlight it. 
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//------------------------------------------------------------------------
void cmmSceneDialog::SelectObjectOnPlacedList(pick3dPickObject* i_pPickObject)
{
	wxTreeItemId found_node = find_node_for_pick_object(m_DataListPlaced, m_treeCtrl_Placed, i_pPickObject);
	if (found_node.IsOk())
	{
		m_bOurChange = true;
		m_treeCtrl_Placed->UnselectAll();
		m_treeCtrl_Placed->SelectItem(found_node);
		enable_placed_buttons();
		m_bOurChange = false;
	}
	//else
	//{
	//	DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetPick3dName().c_str());
	//}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmSceneDialog::AddToSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
{
	wxTreeItemId found_node = find_node_for_pick_object(m_DataListPlaced, m_treeCtrl_Placed, i_pPickObject);
	if (found_node.IsOk())
	{
		highlight_tree_item(m_treeCtrl_Placed, found_node, true);
	}
	//else
	//{
	//	DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetPick3dName().c_str());
	//}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmSceneDialog::RemoveFromSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
{
	wxTreeItemId found_node = find_node_for_pick_object(m_DataListPlaced, m_treeCtrl_Placed, i_pPickObject);
	if (found_node.IsOk())
	{
		highlight_tree_item(m_treeCtrl_Placed, found_node, false);
	}
	//else
	//{
	//	DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetPick3dName().c_str());
	//}
}

//--------------------------------------------------------------------
// set enabled state of buttons based on selection
//--------------------------------------------------------------------
void cmmSceneDialog::enable_placed_buttons()
 {
	// make sure the user selected an object node.
	wxTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
	{
		PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(sel_node));
		DBG_ASSERT0(pData, "All item data should have index.");
		
		// do enable/disable of buttons
		//
		fsLocator path;
		path.Push(m_DataListPlaced[pData->m_Index].m_SystemName.c_str());
		path.Push(m_DataListPlaced[pData->m_Index].m_Name.GetString().c_str());

		if (pData->m_bIsPart)
		{
			// Add category and part name to path
			wxTreeItemId cat_node = m_treeCtrl_Placed->GetItemParent(sel_node);
			path.Push(m_treeCtrl_Placed->GetItemText(cat_node).c_str());
			path.Push(m_treeCtrl_Placed->GetItemText(sel_node).c_str());
		}

		int interactions = cmmDialogInterestMgr::GetAllowedInteractions( path );
		m_bpButton_Edit->Enable((interactions & e_DIAllow_Edit) != 0);
		m_bpButton_Delete->Enable((interactions & e_DIAllow_Del) != 0);
		m_bpButton_Duplicate->Enable((interactions & e_DIAllow_Dupe) != 0);
		m_bpButton_Reload->Enable( (interactions & e_DIAllow_Reload) != 0);
	}
	else
	{
		m_bpButton_Edit->Enable( false );
		m_bpButton_Delete->Enable( false );
		m_bpButton_Duplicate->Enable( false );
		m_bpButton_Reload->Enable( false );
	}
	
 }

//--------------------------------------------------------------------
// build tree view of available objects
//--------------------------------------------------------------------
void cmmSceneDialog::buildtreeview_system_available()
{
	cmmDialogInterestMgr::GetAvailableObjects( m_FileListAvailable );

	//m_bDisableNotify = true;
	//treeView_available->BeginUpdate();

	gsupTreeCtrlUtil::PopulateTreeView(m_treeCtrl_Available, m_FileListAvailable, false );

	//treeView_available->EndUpdate();
	//m_bDisableNotify = false;
}

//--------------------------------------------------------------------
// build tree view of placed objects
//--------------------------------------------------------------------
void cmmSceneDialog::buildtreeview_system_placed()
{
	//	Call the mgr to get all placed objects in the world first
	//
	m_DataListPlaced.clear();
	cmmDialogInterestMgr::GetPlacedObjects(m_DataListPlaced);
	
	// Clear out the old tree, need a single invisible root node to anchor hierarchy
	m_treeCtrl_Placed->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl_Placed->AddRoot("Root");

	updatetreeview_system_placed();

	// When first building the placed list, expand all systems
	gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts);

}

//--------------------------------------------------------------------
// clear and refill tree view with items
//--------------------------------------------------------------------
void cmmSceneDialog::updatetreeview_system_placed()
{
	// If root has not been created yet, then we are in process of starting program,
	// ignore these calls to update.
	wxTreeItemId root = m_treeCtrl_Placed->GetRootItem();
	if (!root.IsOk())
		return;
	//DBG_ASSERT0(root.IsOk(), "Assuming that there is a root node already in tree control.");

	cmmDialogInterestMgr::SortList(m_DataListPlaced);

	// Whatever tree nodes we don't confirm should be deleted.
	// Start with all tree nodes and remove them as we confirm them.
	TreeItemIdSet items_to_delete;
	get_all_tree_items(m_treeCtrl_Placed, items_to_delete);
	envSTLHelpers::RemoveOneValue(items_to_delete, root); // don't delete the root node
	//DBG_LOG1("Before update, have %d items", items_to_delete.size());

	m_treeCtrl_Placed->Freeze();
	m_treeCtrl_Placed->UnselectAll();
		
	//	go through the tree and add new nodes
	//
	cmmDialogDataList::const_iterator it;
	int index = 0;
	for(it = m_DataListPlaced.begin(); it != m_DataListPlaced.end(); it++, index++)
	{
		// See if system name exists already
		wxTreeItemId sys_node = find_or_create_node(m_treeCtrl_Placed, items_to_delete, root, it->m_SystemName);

		//	build the string the user sees
		wxString title;
		if (it->m_Desc.empty()) 
			title = it->m_Name.GetString();
		else
			title = wxString::Format("%s - %s", it->m_Name.GetString().c_str(), it->m_Desc.c_str());
		//DBG_LOG3("System: %s object: %s index: %d", it->m_SystemName.c_str(), title.c_str(), index);

		wxTreeItemId child_node = find_or_create_node(m_treeCtrl_Placed, items_to_delete, sys_node, title.c_str(), -1, index, false);
		set_item_checked(m_treeCtrl_Placed, child_node, it->m_bVisible);
		update_parent_checked_state(m_treeCtrl_Placed, sys_node);

		// Display categories of object parts
		int cat_index = 0;
		std::map<std::string, std::vector<cmmDialogPartData> >::const_iterator cat_it;
		for (cat_it = it->m_Parts.begin(); cat_it != it->m_Parts.end(); ++cat_it, ++cat_index)
		{
			// load an image icon for the part to make categories easier to see
			int image_index = e_FirstPartIndex + cat_index;
			if (image_index >= e_NumPlacedImageStates)
				image_index = -1;

			wxTreeItemId cat_node = find_or_create_node(m_treeCtrl_Placed, items_to_delete, child_node, cat_it->first, image_index);
		
			// Common cases are either all parts being created new or
			// all parts exist already. Try to detect and handle these cases quicker.
			const int num_parts = cat_it->second.size();
			const bool bRecursive = false;
			const int num_nodes = m_treeCtrl_Placed->GetChildrenCount(cat_node, bRecursive);

			// Temporary test here, see how it affects the timing if we never update
			// existing part lists in the tree control.
			if (num_nodes == num_parts)
			{
				// Reuse all nodes, just set the string and ids again
				wxTreeItemIdValue cookie;
				int part_ind = 0;
				for (wxTreeItemId id = m_treeCtrl_Placed->GetFirstChild(cat_node, cookie);
					 id.IsOk(); 
					 ++part_ind, id = m_treeCtrl_Placed->GetNextChild(cat_node, cookie))
				{
					const cmmDialogPartData &part_data = cat_it->second[part_ind];
					m_treeCtrl_Placed->SetItemText(id, part_data.m_Name);
					PlacedTreeItemData *pData = static_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(id));
					DBG_ASSERT0(pData, "All item data should have index.");
					pData->m_Index = index; // index of object, not of part
					envSTLHelpers::RemoveOneValue(items_to_delete, id);
				}
			}
			else
			{
				// If we don't have any nodes, then we can just create them without
				// looking for nodes with the same name
				const bool c_bCreateAlways = (num_nodes == 0);

				std::vector<cmmDialogPartData>::const_iterator part_it;
				for (part_it = cat_it->second.begin(); part_it != cat_it->second.end(); ++part_it)
				{
					const bool c_bIsPart = true;
					wxTreeItemId part_node = find_or_create_node(m_treeCtrl_Placed, items_to_delete, 
						cat_node, part_it->m_Name, image_index, index, c_bIsPart, c_bCreateAlways);
				}
			}

			// Don't sort until all the parts are created.
			m_treeCtrl_Placed->SortChildren(cat_node);
		}
	}

	// Now remove all the old nodes that we did not confirm
	//DBG_LOG1("After update, have %d items", items_to_delete.size());
	TreeItemIdSet::iterator item_it;
	for (item_it = items_to_delete.begin(); item_it != items_to_delete.end(); ++item_it)
	{
		m_treeCtrl_Placed->Delete(*item_it);
	}


	m_treeCtrl_Placed->Thaw();
}

//--------------------------------------------------------------------
// Add objects to scene based on what is selected
//--------------------------------------------------------------------
void cmmSceneDialog::add_objects()
 {
	 wxArrayTreeItemIds selection;
	int num_selected = m_treeCtrl_Available->GetSelections(selection);

	if (num_selected > 1)
		undoUndoMgr::BeginMultipleOperationBlock();

    for (int i=0; i<num_selected; ++i)
	{
		wxTreeItemId sel_node = selection[i];
		if (sel_node.IsOk())
		{
			// Has to be a leaf node in order to create an object for it
			if (!m_treeCtrl_Available->ItemHasChildren(sel_node))
			{
				itString fname(m_treeCtrl_Available->GetItemText(sel_node));
				int index = m_FileListAvailable.GetIndex(fname);
				if (index >= 0)
				{
					itString fname = m_FileListAvailable.GetFilename(index);
					fsLocator path;
					m_FileListAvailable.GetFilePath(index, path);
					cmmDialogInterestMgr::AddObject(fname, path);
				}
			}
		}
	}

	 if (num_selected > 1)
		undoUndoMgr::EndMultipleOperationBlock();
 }

//--------------------------------------------------------------------
// Handle selection of a single node in the placed tree
//--------------------------------------------------------------------
void cmmSceneDialog::select_item(wxTreeItemId i_SelNode, bool i_bAppend)
{		
	PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(i_SelNode));
	DBG_ASSERT0(pData, "All calls to select_item should have item data with index.");

	int index = pData->m_Index;
	//DBG_LOG2("Selection id is: %d, index: %d", i_SelNode, index);

	if (pData->m_bIsPart)
	{
		wxTreeItemId cat_node = m_treeCtrl_Placed->GetItemParent(i_SelNode);

		cmmDialogInterestMgr::SelectObjectPart(m_DataListPlaced[index].m_Name, 
			m_DataListPlaced[index].m_SystemName,
			m_treeCtrl_Placed->GetItemText(i_SelNode).c_str(),
			m_treeCtrl_Placed->GetItemText(cat_node).c_str(),
			false); // i_bAppend); //temp [bga] - force object part selection to be single selection
	}
	else
	{
		cmmDialogInterestMgr::SelectObject(m_DataListPlaced[index].m_Name, 
			m_DataListPlaced[index].m_SystemName,
			i_bAppend);
	}
}

//--------------------------------------------------------------------
// Delete selected objects from the placed menu
//--------------------------------------------------------------------
void cmmSceneDialog::delete_selection()
{
	// Make a copy of the selected list, because this will change.
	std::list<pick3dPickObject*> selected_objects = sel3dMgr::GetSelectedList();

	// Create a single undo operation for the multiple delete
	undoUndoMgr::BeginMultipleOperationBlock();

	// Clear the selection before deleting?
	//sel3dMgr::CreateUndoOperation();
	sel3dMgr::ClearSelection();

	// Iterate through the old selected list, finding and deleting each object
	std::list<pick3dPickObject*>::iterator it;
	for (it = selected_objects.begin(); it != selected_objects.end(); ++it)
	{
		wxTreeItemId found_node = find_node_for_pick_object(m_DataListPlaced, m_treeCtrl_Placed, *it);
		if (found_node.IsOk() && m_treeCtrl_Placed->GetItemData(found_node))
		{
			PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(found_node));
			DBG_ASSERT0(pData, "All item data should have index.");
			
			if (!pData->m_bIsPart)
				cmmDialogInterestMgr::DeleteObject(m_DataListPlaced[pData->m_Index].m_Name, m_DataListPlaced[pData->m_Index].m_SystemName);
			else
			{
			//	guiMessageBox::Show("Cannot delete an object part", "Delete", guiMessageBox::e_OKOnly);
				
				wxTreeItemId cat_node = m_treeCtrl_Placed->GetItemParent(found_node);

				cmmDialogInterestMgr::DeleteObjectPart(m_DataListPlaced[pData->m_Index].m_Name, 
					m_DataListPlaced[pData->m_Index].m_SystemName,
					m_treeCtrl_Placed->GetItemText(found_node).c_str(),
					m_treeCtrl_Placed->GetItemText(cat_node).c_str());
			}
		}
	}

	undoUndoMgr::EndMultipleOperationBlock();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::button_Available_OpenAll_Click(wxCommandEvent& i_Event)
{
	gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Available);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::button_Available_CollapseAll_Click(wxCommandEvent& i_Event)
{
	gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Available);
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::button_Available_OpenPartial_Click(wxCommandEvent& i_Event)
{
	 wxArrayTreeItemIds selection;
	int num_selected = m_treeCtrl_Available->GetSelections(selection);

    for (int i=0; i<num_selected; ++i)
	{
		wxTreeItemId sel_node = selection[i];
		if (sel_node.IsOk())
		{
			if (m_treeCtrl_Available->IsExpanded(sel_node))
				m_treeCtrl_Available->CollapseAllChildren(sel_node);
			else
				m_treeCtrl_Available->ExpandAllChildren(sel_node);
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Available_DoubleClick(wxTreeEvent& i_Event)
{
	wxTreeItemId sel_node = i_Event.GetItem();
	
	// Has to be a leaf node in order to create an object for it
	if (m_treeCtrl_Available->ItemHasChildren(sel_node))
	{
		//m_treeCtrl_Available->ExpandAllChildren(sel_node);
		i_Event.Skip();
	}
	else
	{
		// If leaf node, add it to the scene
		this->add_objects();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::button_Add_Click(wxCommandEvent& i_Event)
{
	this->add_objects();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::button_Refresh_Click(wxCommandEvent& i_Event)
{
	this->buildtreeview_system_available();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_LeftMouseDown( wxMouseEvent& i_Event )
{
	bool bSkip = true;

	// Store states of modifier keys to get them later
	// during a selection event
	m_bCtrlPressed = i_Event.m_controlDown;
	m_bShiftPressed = i_Event.m_shiftDown;

	int flags = 0;
	wxTreeItemId pick_item = m_treeCtrl_Placed->HitTest(i_Event.GetPosition(), flags);
	if (pick_item.IsOk())
	{
		// Check for click on the icon
		if (flags & wxTREE_HITTEST_ONITEMICON)
		{
			int pick_depth = get_depth_for_node(m_treeCtrl_Placed, pick_item);
			//DBG_LOG2("Icon click: %s, depth %d", m_treeCtrl_Placed->GetItemText(pick_item).c_str(), pick_depth);

			if (pick_depth == 2)
			{
				// Clicked on system name 
				bool bNewCheckState = (!get_item_checked(m_treeCtrl_Placed, pick_item));
				set_item_checked(m_treeCtrl_Placed, pick_item, bNewCheckState);

				//set checked state of child nodes
				wxTreeItemIdValue cookie;
				for (wxTreeItemId id = m_treeCtrl_Placed->GetFirstChild(pick_item, cookie);
					 id.IsOk(); 
					 id = m_treeCtrl_Placed->GetNextChild(pick_item, cookie))
				{
					PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(id));
					DBG_ASSERT0(pData, "All item data should have index.");
				
					set_item_checked(m_treeCtrl_Placed, id, bNewCheckState);
					visMgr::SetVisibleInEditor(m_DataListPlaced[pData->m_Index].m_Name.GetString(), bNewCheckState);
				}
				bSkip = false;
			}
			else if (m_treeCtrl_Placed->GetItemData(pick_item))
			{
				PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(pick_item));
				DBG_ASSERT0(pData, "All item data should have index.");
				
				// No check boxes for object parts
				if (!pData->m_bIsPart)
				{
					// A single node's check has changed
					bool bNewCheckState = (!get_item_checked(m_treeCtrl_Placed, pick_item));
					set_item_checked(m_treeCtrl_Placed, pick_item, bNewCheckState);
					update_parent_checked_state(m_treeCtrl_Placed, m_treeCtrl_Placed->GetItemParent(pick_item));
					visMgr::SetVisibleInEditor(m_DataListPlaced[pData->m_Index].m_Name.GetString(), bNewCheckState);
					bSkip = false;
				}
			}

			//std::string name;
			//bool bCheckState = pSN->Checked;
			//if (pSN->GetNodeCount(false) > 0)
			//{
			//	// This is a label node, so make all the children's visibility checks match this one
			//	//
			//	int childnodes = pSN->GetNodeCount(false);
			//	for (int i=0; i< childnodes; ++i)
			//	{
			//		bool b_childcheck = pSN->Nodes[i]->Checked;
			//		if ( b_childcheck != bCheckState)
			//		{
			//			pSN->Nodes[i]->Checked = bCheckState;

			//			std::string name;
			//			extract_valid_name(pSN->Nodes[i]->Text, 0, name);
			//			visMgr::SetVisibleInEditor(name, bCheckState);
			//		}
			//	}
			//}
			//else
			//{
			//	std::string name;
			//	extract_valid_name(pSN->Text, 0, name);

			//	// A single node's check has changed
			//	visMgr::SetVisibleInEditor(name, bCheckState);
			//}
		}
	}
	i_Event.Skip(bSkip);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_LeftMouseUp( wxMouseEvent& i_Event )
{
	m_bCtrlPressed = false;
	m_bShiftPressed = false;
	i_Event.Skip();
}

//--------------------------------------------------------------------
// Delete key pressed in tree control
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_CharPressed( wxKeyEvent& i_Event )
{
	if (i_Event.GetKeyCode() == WXK_DELETE)
	{
		delete_selection();
	}
	else
		i_Event.Skip();
}

//--------------------------------------------------------------------
// Selection is changing, this event handler can veto it
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_Selecting(wxTreeEvent& i_Event)
{
	//if (i_Event.GetItem().IsOk())
	//	DBG_LOG1("S-ing Event node: %s", m_treeCtrl_Placed->GetItemText(i_Event.GetItem()).c_str());
	//else
	//	DBG_LOG0("S-ing Event node: null");

	//if (i_Event.GetOldItem().IsOk())
	//	DBG_LOG1("S-ing Event old node: %s", m_treeCtrl_Placed->GetItemText(i_Event.GetOldItem()).c_str());
	//else
	//	DBG_LOG0("S-ing Event old node: null");

	//// Our requirement is that all selected children need
	//// to have the same depth in the tree. You can select
	//// multiple objects and multiple object parts, but not
	//// a mix of objects and parts.

	//wxArrayTreeItemIds selection;
	//int num_selected = m_treeCtrl_Placed->GetSelections(selection);
	//DBG_LOG2("S-ing Num selected: %d %d", num_selected, selection.size());
	//if (num_selected > 1)
	//{
	//	int first_depth = get_depth_for_node(m_treeCtrl_Placed, selection[0]);
	//	for (int i=1; i<num_selected; ++i)
	//	{
	//		int depth = get_depth_for_node(m_treeCtrl_Placed, selection[i]);
	//		if (depth != first_depth)
	//		{
	//			i_Event.Veto();
	//			//m_treeCtrl_Placed->SelectItem(selection[i], false);

	//			// Just vetoing the event won't clear out the incorrect
	//			// selection, so we need to re-notify in order to
	//			// correctly display the selection as we understand it.
	//			mnmThinkMgr::CallFunctionDelayed(&sel3dMgr::Renotify);
	//			break;
	//		}
	//	}
	//}
}

//--------------------------------------------------------------------
// Selection has changed, notify the systems.
// This is the single selection version
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_Selection(wxTreeEvent& i_Event)
{
	if (m_bOurChange)
		return;
	
	wxTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
	{
		PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(sel_node));
		DBG_ASSERT0(pData, "All sel_nodes should have item data with index.");

			// can't get the modifier keys from here in wxWidgets,
		// so we get the state of the buttons in mouse events elsewhere
		if (this->m_bCtrlPressed)
		{
			int index = pData->m_Index;
			//DBG_LOG2("Selection id is: %d, index: %d", i_SelNode, index);

			if (!pData->m_bIsPart)
			{
				cmmDialogInterestMgr::DeselectObject(m_DataListPlaced[index].m_Name, 
					m_DataListPlaced[index].m_SystemName);
				m_treeCtrl_Placed->Unselect();
			}
		}
		else if (this->m_bShiftPressed)
		{
			// Append selections
			const bool bAppend = true;

			if (!pData->m_bIsPart)
			{
				// Test if old node is valid
				wxTreeItemId old_node = i_Event.GetOldItem();
				wxTreeItemId parent_node = m_treeCtrl_Placed->GetItemParent(sel_node);
				if (old_node.IsOk()
					&& (parent_node == m_treeCtrl_Placed->GetItemParent(old_node)))
				{
					//DBG_LOG1("Have old node: %s", m_treeCtrl_Placed->GetItemText(old_node).c_str());

					// The old selected node and the current picked node have the same parent, so
					// we should append select all nodes between them also.
					bool bBetweenNodes = false;
					wxTreeItemIdValue cookie;
					for (wxTreeItemId id = m_treeCtrl_Placed->GetFirstChild(parent_node, cookie);
						 id.IsOk(); 
						 id = m_treeCtrl_Placed->GetNextChild(parent_node, cookie))
					{
						if (id == sel_node || id == old_node)
						{
							if (bBetweenNodes)
								break;
							else
								bBetweenNodes = true;
						}
						else if (bBetweenNodes)
						{
							// Append select the items between the old and new selection
							select_item(id, bAppend);
						}
					}
				}
			}

			// Select new item last
			select_item(sel_node, bAppend);
		}
		else
		{
			// Select item alone
			const bool bAppend = false;
			select_item(sel_node, bAppend);
		}
	}
	enable_placed_buttons();
}

//--------------------------------------------------------------------
// Selection has changed, notify the systems
// This is the multiple selection version, not being used right now.
//--------------------------------------------------------------------
void cmmSceneDialog::treeCtrl_Placed_Selection_MultipleVersion(wxTreeEvent& i_Event)
{
	wxArrayTreeItemIds selection;
	int num_selected = m_treeCtrl_Placed->GetSelections(selection);
	//DBG_LOG2("Num selected: %d %d", num_selected, selection.size());

	//if (i_Event.GetItem().IsOk())
	//	DBG_LOG1("Event node: %s", m_treeCtrl_Placed->GetItemText(i_Event.GetItem()).c_str());
	//else
	//	DBG_LOG0("Event node: null");

	//if (i_Event.GetOldItem().IsOk())
	//	DBG_LOG1("Event old node: %s", m_treeCtrl_Placed->GetItemText(i_Event.GetOldItem()).c_str());
	//else
	//	DBG_LOG0("Event old node: null");

	if (num_selected == 1)
	{
		wxTreeItemId sel_node = selection[0];
		if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
		{
			const bool bAppend = false;
			select_item(sel_node, bAppend);
		}
	}
	else
	{
		// If more than one item selected, clear selection and then
		// add in each item in the selection. Make this whole change
		// a single selection operation by using a undo block.
		undoUndoMgr::BeginMultipleOperationBlock();

		// do we need to ask sel3dMgr to create an undo operation also?
		sel3dMgr::ClearSelection();

		for (int i=0; i<num_selected; ++i)
		{
			wxTreeItemId sel_node = selection[i];
			if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
			{
				DBG_LOG2("Selection %d item: %s", i, m_treeCtrl_Placed->GetItemText(sel_node).c_str());
				const bool bAppend = true;
				select_item(sel_node, bAppend);
			}
		}

		undoUndoMgr::EndMultipleOperationBlock();
	}
}

void cmmSceneDialog::treeCtrl_Placed_Activated( wxTreeEvent& i_Event )
{
	if (m_treeCtrl_Placed->GetItemData(i_Event.GetItem()))
	{
		PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(i_Event.GetItem()));
		DBG_ASSERT0(pData, "All calls to select_item should have item data with index.");

		int index = pData->m_Index;
		//DBG_LOG2("Activation id is: %d, index: %d", i_SelNode, index);

		// Activate objects and parts differently
		if (!pData->m_bIsPart)
		{
			cmmDialogInterestMgr::ActivateObject(m_DataListPlaced[index].m_Name, 
				m_DataListPlaced[index].m_SystemName);
		}
		else
		{
			wxTreeItemId cat_node = m_treeCtrl_Placed->GetItemParent(i_Event.GetItem());

			cmmDialogInterestMgr::ActivateObjectPart(m_DataListPlaced[index].m_Name, 
				m_DataListPlaced[index].m_SystemName,
				m_treeCtrl_Placed->GetItemText(i_Event.GetItem()).c_str(),
				m_treeCtrl_Placed->GetItemText(cat_node).c_str());
		}
	}
}

void cmmSceneDialog::treeCtrl_Placed_StateImageClick( wxTreeEvent& i_Event )
{
	//bga - doesn't work. Can't get the state icons to display.
	i_Event.Skip(); 
}

void cmmSceneDialog::button_Placed_OpenAll_Click( wxCommandEvent& i_Event )
{
	gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts);
}

void cmmSceneDialog::button_Placed_CollapseAll_Click( wxCommandEvent& i_Event )
{
	gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed);
}

void cmmSceneDialog::button_Placed_OpenPartial_Click( wxCommandEvent& i_Event )
{
	 wxArrayTreeItemIds selection;
	int num_selected = m_treeCtrl_Placed->GetSelections(selection);

    for (int i=0; i<num_selected; ++i)
	{
		wxTreeItemId sel_node = selection[i];
		if (sel_node.IsOk())
		{
			if (m_treeCtrl_Placed->IsExpanded(sel_node))
				m_treeCtrl_Placed->CollapseAllChildren(sel_node);
			else
				m_treeCtrl_Placed->ExpandAllChildren(sel_node);
		}
	}
}

void cmmSceneDialog::button_Edit_Click( wxCommandEvent& i_Event )
{
	cmmObjectDialogUtil::Show();
}

void cmmSceneDialog::button_Duplicate_Click( wxCommandEvent& i_Event )
{
	wxTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
	{
		PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(sel_node));
		DBG_ASSERT0(pData, "All item data should have index.");
		
		if (!pData->m_bIsPart)
			cmmDialogInterestMgr::DuplicateObject(m_DataListPlaced[pData->m_Index].m_Name, m_DataListPlaced[pData->m_Index].m_SystemName);
		else
			guiMessageBox::Show("Cannot duplicate an object part", "Duplicate", guiMessageBox::e_OKOnly);
	}

}

void cmmSceneDialog::button_Reload_Click( wxCommandEvent& i_Event )
{
	wxTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	if (sel_node.IsOk() && m_treeCtrl_Placed->GetItemData(sel_node))
	{
		PlacedTreeItemData *pData = dynamic_cast<PlacedTreeItemData*>(m_treeCtrl_Placed->GetItemData(sel_node));
		DBG_ASSERT0(pData, "All item data should have index.");
		
		if (!pData->m_bIsPart)
			cmmDialogInterestMgr::ReloadObject(m_DataListPlaced[pData->m_Index].m_Name, m_DataListPlaced[pData->m_Index].m_SystemName);
		else
			guiMessageBox::Show("Cannot reload an object part", "Reload", guiMessageBox::e_OKOnly);
	}

}

void cmmSceneDialog::button_Delete_Click( wxCommandEvent& i_Event )
{
	delete_selection();
}



#endif // USE_WXWIDGETS