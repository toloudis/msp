/*****************************************************************************
**	twcTreeView.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/twc/twcTreeView.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itString.hpp"


#ifdef USE_WXWIDGETS

namespace
{
	// There is a problem with wxWidgets where the wxTR_MULTIPLE flag causes the
	// Left Mouse Down events to not be sent to our handler. This means that
	// we don't notice when the user clicks on the check box icon when the item
	// is not selected. So, this has forced us in the past to do a single selection
	// tree control and have special functions that highlight the additional
	// selected objects.
	const long c_TreeWindowStyle = wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT;
	//const long c_TreeWindowStyle = wxTR_DEFAULT_STYLE|wxTR_HIDE_ROOT|wxTR_MULTIPLE;

	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum CheckboxImageState
	{
		e_NoCheckbox = 0,
		e_Unchecked = 1,
		e_Checked = 2,
		e_MixChecked = 3,
		e_NumCheckboxImageStates
	};

	//--------------------------------------------------------------------
	// Tree item data contains reference to the node in the tree
	//	data given to us by the user.
	//--------------------------------------------------------------------
	class TreeNodeItemData : public wxTreeItemData
	{
	public:
		TreeNodeItemData(shared_ptr<twcTreeNode> i_Node)
			: m_Node(i_Node) {}
		shared_ptr<twcTreeNode> m_Node;
	};


	//--------------------------------------------------------------------
	// Find node with given text as child of parent node. 
	// Check IsOk() of returned value to see if found.
	//--------------------------------------------------------------------
	wxTreeItemId find_node(wxTreeCtrl* io_pTreeCtrl, 
						   wxTreeItemId i_Parent, 
						   const wxString &i_Text)
	{
		wxTreeItemId id;
		wxTreeItemIdValue cookie;
		for (id = io_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = io_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			if (i_Text == io_pTreeCtrl->GetItemText(id))
			{
				// found it
				break;
			}
		}
		return id; // id.IsOk() == false if not found
	}

}	// end of namespace


BEGIN_EVENT_TABLE(twcTreeView, wxTreeCtrl)
	//EVT_SET_FOCUS(twcTreeView::OnSetFocus)
	//EVT_KILL_FOCUS(twcTreeView::OnLostFocus)
	EVT_LEFT_DOWN(twcTreeView::treeCtrl_LeftMouseDown)
	EVT_LEFT_UP(twcTreeView::treeCtrl_LeftMouseUp)
END_EVENT_TABLE()


//--------------------------------------------------------------------
//--------------------------------------------------------------------
twcTreeView::twcTreeView( wxWindow* parent )
: wxTreeCtrl( parent, wxID_ANY, wxDefaultPosition, wxDefaultSize, c_TreeWindowStyle  ),
	m_bSelectOnCheckChange(true),
	//m_bHasFocus(false),
	m_bOurChange(false),
	m_bUpdating(false)
{
	m_bCtrlPressed = m_bShiftPressed = m_bAltPressed = false;

	// Connect Events
	this->Connect( wxEVT_COMMAND_TREE_SEL_CHANGED, wxTreeEventHandler( 
		twcTreeView::treeCtrl_Selection ), NULL, this );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twcTreeView::~twcTreeView()
{
	this->DeleteAllItems();
}


//--------------------------------------------------------------------
// Give directory where the tree view should find the icons it needs.
// It will load from this directory:
//		tree-unchecked.png 
//		tree-checked.png
//		tree-mixchecked.png
// The alternative is to call AssignStateImageList with images such that
// the first 3 images should be for the checked state...
//		NoCheckbox = 0
//		Unchecked = 1
//		Checked = 2
//		MixChecked = 3
//--------------------------------------------------------------------
void twcTreeView::LoadImagesFromDirectory( const fsLocator& i_IconDirectory )
{
	// Create state images to represent checked state
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( i_IconDirectory, icon_dir );

	wxImageList *pCheckImages = new wxImageList(13, 13, false, e_NumCheckboxImageStates);
	std::wstring image_dir( icon_dir.GetString() );
    {
        wxLogNull nullLog;
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG)); // need extra image to hold the zeroth slot
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
    }
    this->AssignStateImageList(pCheckImages); // tree ctrl takes ownership
	//this->AssignImageList(pCheckImages); // tree ctrl takes ownership
}

//--------------------------------------------------------------------
// By default, clicks on the check box will also act as a selection
// action. To turn off this behavior pass false to this function.
//--------------------------------------------------------------------
void twcTreeView::SetSelectOnCheckChange(bool i_bSelect)
{
	m_bSelectOnCheckChange = i_bSelect;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void twcTreeView::Clear()
{
	m_bUpdating  = true;

	//	clear the control
	this->DeleteAllItems();
	m_FirstShift.Unset();

	// clear the tree data
	//m_RootNodes.clear();

	m_bUpdating  = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------		
void twcTreeView::UpdateTreeData(const shared_ptr<twcTreeNode>& i_RootNode)
{
	// fill simple vector, apss to other function
	std::vector< shared_ptr<twcTreeNode> > root_nodes;
	root_nodes.push_back(i_RootNode);
	this->UpdateTreeData(root_nodes);
}

//--------------------------------------------------------------------
// Set/Update tree data, this control will maintain a reference
//	to the tree data and will pass the nodes to the callbacks.
//--------------------------------------------------------------------
void twcTreeView::UpdateTreeData(const std::vector< shared_ptr<twcTreeNode> >& i_RootNodes)
{
	m_bUpdating  = true;

	// Whatever tree nodes we don't confirm should be deleted.
	// Start with all tree nodes and remove them as we confirm them.
	TreeItemIdSet items_to_delete;

	// Get root node
	wxTreeItemId root = this->GetRootItem();
	if (!root.IsOk())
	{
		root = this->AddRoot(L"Root");
	}
	else
	{
		get_all_tree_items(root, items_to_delete);
	}

	//DBG_LOG("Before update, have " << items_to_delete.size() << " items" );

	//DBG_LOG("twcTreeView: Updating tree data");
	this->Freeze();

	// Make a copy?
	//m_RootNodes = i_RootNodes;

	const int num_roots = i_RootNodes.size();
	for (int i=0; i<num_roots; i++)
	{
		find_or_create_tree_node(items_to_delete, root, i_RootNodes[i]);
	}
	this->SortChildren(root);

	// Check to see if we are about to delete the selected node
	wxTreeItemId sel_node = this->GetSelection();
	if (sel_node.IsOk())
	{
		if (envSTLHelpers::Contains(items_to_delete, sel_node))
			this->Unselect();
	}

	// Now remove all the old nodes that we did not confirm
	//DBG_LOG("After update, have " << items_to_delete.size() << " items" );
	TreeItemIdSet::iterator item_it;
	for (item_it = items_to_delete.begin(); item_it != items_to_delete.end(); ++item_it)
	{
		this->Delete(*item_it);
	}

	this->Thaw();

	// Clear out interaction cache
	m_FirstShift.Unset();

	m_bUpdating  = false;
}

//--------------------------------------------------------------------
// Find function finds node that satisfies the given predicate.
//--------------------------------------------------------------------
wxTreeItemId twcTreeView::FindNode(NodeFilterFunction i_Function)
{
	wxTreeItemId root = this->GetRootItem();
	DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");
	return (find_child_node(root, i_Function));
}

//--------------------------------------------------------------------
// Get user data associated with tree item id
//--------------------------------------------------------------------
shared_ptr<twcTreeNode> twcTreeView::GetNodeData(wxTreeItemId i_Node)
{
	shared_ptr<twcTreeNode> result;
	if (i_Node.IsOk())
	{
		if (wxTreeItemData *pItemData = this->GetItemData(i_Node))
		{
			TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
			result = pData->m_Node;
		}
	}
	return result;
}	

//--------------------------------------------------------------------
//	Find the item on the placed tree and highlight it. 
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//--------------------------------------------------------------------
void twcTreeView::SelectNode(wxTreeItemId i_Node)
{
	if (i_Node.IsOk())
	{
		//const bool bSendEvent = false;
		//this->select_item(i_Node, bSendEvent);

		// Use actual wxTreeControl's selection, which will
		// trigger the rest
		m_bOurChange = true;
		this->SelectItem(i_Node);
		m_bOurChange = false;
	}

}

//--------------------------------------------------------------------
// Handle multiple selection by highlighting other tree nodes
//--------------------------------------------------------------------
void twcTreeView::AddToSelectedNodes(wxTreeItemId i_Node)
{
	if (i_Node.IsOk())
	{
		if (!is_node_selected(i_Node))
		{
			// add the tree item to our selected item list
			m_SelectedList.push_back(i_Node);
			highlight_item(i_Node, true);
		}
	}

}
void twcTreeView::RemoveFromSelectedNodes(wxTreeItemId i_Node)
{
	if (i_Node.IsOk())
	{
		if (is_node_selected(i_Node))
		{
			// add the tree item to our selected item list
			envSTLHelpers::RemoveOneValue(m_SelectedList, i_Node);
			highlight_item(i_Node, false);
		}
	}
}

//------------------------------------------------------------------------
// Apply predicate to all nodes in the tree in order to find the node
// in which the predicate returns true, 
// or return a node with IsOk()==false.
//------------------------------------------------------------------------
wxTreeItemId twcTreeView::find_child_node(wxTreeItemId i_Parent, NodeFilterFunction i_Function)
{
	// Iterate through children
	wxTreeItemId id, found_node;
	wxTreeItemIdValue cookie;
	for (id = this->GetFirstChild(i_Parent, cookie);
		 id.IsOk(); 
		 id = this->GetNextChild(i_Parent, cookie))
	{
		// Call predicate with the user tree node data associated with this node
		if (wxTreeItemData *pItemData = this->GetItemData(id))
		{
			TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
			if (i_Function(pData->m_Node))
				return id;
		}

		// recursively look through children
		found_node = find_child_node(id, i_Function);
		if (found_node.IsOk())
			return found_node;
	}

	return found_node; // will be IsOk() == false if not found

}

//------------------------------------------------------------------------
// Return current checked state of tree control item.
// Since there is no support for checked items in wxWidgets, look at
// the image state we are using.
//------------------------------------------------------------------------
bool twcTreeView::get_item_checked(wxTreeItemId i_Node)
{
	// Using state image list now for checkboxes
	//return (this->GetItemImage(i_Node) != e_Unchecked);
	return (this->GetItemState(i_Node) != e_Unchecked);
}
bool twcTreeView::is_mixed_checked(wxTreeItemId i_Node)
{
	// Using state image list now for checkboxes
	//return (this->GetItemImage(i_Node) == e_MixChecked);
	return (this->GetItemState(i_Node) == e_MixChecked);
}
void twcTreeView::set_item_checked(wxTreeItemId i_Node, bool i_bChecked)
{
	// Using state image list now for checkboxes
	//return (this->SetItemImage(i_Node, (i_bChecked) ? e_Checked : e_Unchecked));
    return (this->SetItemState(i_Node, (i_bChecked) ? e_Checked : e_Unchecked));
}

//------------------------------------------------------------------------
// Set checked state of tree node based on checked state of children
//------------------------------------------------------------------------
void twcTreeView::update_parent_checked_state(wxTreeItemId i_Parent)
{
	bool bAllChecked = true;
	bool bAllUnchecked = true;
	bool bHaveCheckableChildren = false;

	wxTreeItemIdValue cookie;
	for (wxTreeItemId id = this->GetFirstChild(i_Parent, cookie);
		 id.IsOk(); 
		 id = this->GetNextChild(i_Parent, cookie))
	{
		// See if this node has a custom image icon. If so, don't
		// consider its checked state.
		//if (this->GetItemImage(id) < e_NumCheckboxImageStates)
		if ((this->GetItemState(id) != e_NoCheckbox) && 
			(this->GetItemState(id) < e_NumCheckboxImageStates))
		{
			bHaveCheckableChildren = true;
			if (is_mixed_checked(id)) 
			{
				// any mixed child means the parent is mixed
				bAllUnchecked = false;
				bAllChecked = false;
				break;
			}
			else if (get_item_checked(id))
				bAllUnchecked = false;
			else
				bAllChecked = false;
		}
	}

	if (bHaveCheckableChildren)
	{
		// Using item state for checkboxes now
		//if (bAllChecked)
		//	this->SetItemImage(i_Parent, e_Checked);
		//else if (bAllUnchecked)
		//	this->SetItemImage(i_Parent, e_Unchecked);
		//else
		//	this->SetItemImage(i_Parent, e_MixChecked);
		if (bAllChecked)
			this->SetItemState(i_Parent, e_Checked);
		else if (bAllUnchecked)
			this->SetItemState(i_Parent, e_Unchecked);
		else
			this->SetItemState(i_Parent, e_MixChecked);
	}
}

//--------------------------------------------------------------------
// Recursively set all children of the given node to the given 
// checked state.
//--------------------------------------------------------------------
void twcTreeView::check_all_children(wxTreeItemId i_Item, bool i_bNewCheckState)
{
	// set checked state of child nodes, but don't notify
	wxTreeItemIdValue cookie;
	for (wxTreeItemId id = this->GetFirstChild(i_Item, cookie);
		 id.IsOk(); 
		 id = this->GetNextChild(i_Item, cookie))
	{
		if (is_node_checkable(id))
		{
			set_item_checked(id, i_bNewCheckState);
			check_all_children(id, i_bNewCheckState);
		}
	}
}

//------------------------------------------------------------------------
// Get the ids of all nodes under the given parent node
//------------------------------------------------------------------------
void twcTreeView::get_all_tree_items(wxTreeItemId i_Parent, 
									 TreeItemIdSet& o_TreeIds)
{
	wxTreeItemIdValue cookie;
	for (wxTreeItemId id = this->GetFirstChild(i_Parent, cookie);
		 id.IsOk(); 
		 id = this->GetNextChild(i_Parent, cookie))
	{
		// recurse on children first because we will remove them in that order
		if (this->ItemHasChildren(id))
			get_all_tree_items(id, o_TreeIds);

		// then add the parent
		o_TreeIds.push_back(id.m_pItem);
		//DBG_LOG2("Added tree id %x, num ids: %d", id, o_TreeIds.size());

	}
}
	
//------------------------------------------------------------------------
// Create subtree for this node under the given parent
//------------------------------------------------------------------------
void twcTreeView::find_or_create_tree_node(TreeItemIdSet& io_ItemsToDelete,
										   wxTreeItemId i_Parent,
										   const shared_ptr<twcTreeNode>& i_Node,
										   bool i_bCreateAlways)
{
	wxTreeItemId node_item;
	wxString text_str = i_Node->GetDisplayString();

	// In some cases, we know we will always need to create a new node
	// and can skip the "find" step.
	if (!i_bCreateAlways)
		node_item = find_node(this, i_Parent, text_str);

	// State index defines checkbox, use "0" for no check box.
	// Image index comes from user, -1 means don't use an image at all.
	int image_index = -1, state_index = 0;
	if (i_Node->IsCheckable())
		state_index = (i_Node->IsChecked() ? e_Checked : e_Unchecked);
	else
		state_index = i_Node->GetStateIndex();
	image_index = i_Node->GetImageIndex();

	if (!node_item.IsOk())
	{
		// Need to create the node 
		node_item = this->AppendItem(i_Parent, text_str, image_index );
		this->SetItemState( node_item, state_index );

		// Store reference to the twcTreeNode in the item data
		this->SetItemData(node_item, new TreeNodeItemData(i_Node));
	}
	else 
	{
		// Found node, reuse it, and remove from list of nodes to delete
		envSTLHelpers::RemoveOneValue(io_ItemsToDelete, node_item);

		// Update image in case it changed
		this->SetItemImage( node_item, image_index );
		this->SetItemState( node_item, state_index );

		// Assign new node to the item data
		if (wxTreeItemData *pItemData = this->GetItemData(node_item))
		{
			TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
			//DBG_ASSERT(pData, "All item data should have index.");
			pData->m_Node = i_Node;
		}
	}

	const int num_kids = i_Node->m_Children.size();
	for (int k=0; k<num_kids; k++)
	{
		find_or_create_tree_node(io_ItemsToDelete, node_item, i_Node->m_Children[k]);
	}
	if (i_Node->IsCheckable() && (num_kids > 0))
	{
		update_parent_checked_state(node_item);
	}

	if (i_Node->ShouldSortChildren())
		this->SortChildren(node_item);
}

//------------------------------------------------------------------------
// Create subtree for this node under the given parent
//------------------------------------------------------------------------
void twcTreeView::create_tree_node(wxTreeItemId i_Parent,
										  const shared_ptr<twcTreeNode>& i_Node)
{
	// Either use a checkbox for the image, or use the custom image index
	// from the user. "-1" means don't use an image at all.
	int image_index = -1, state_index = 0;
	if (i_Node->IsCheckable())
		state_index = (i_Node->IsChecked() ? e_Checked : e_Unchecked);
	else
		state_index = i_Node->GetStateIndex();
	image_index = i_Node->GetImageIndex();
	wxTreeItemId node_item = this->AppendItem(i_Parent, 
		i_Node->GetDisplayString(), image_index	);
	this->SetItemState(node_item, state_index);

	// Store reference to the twcTreeNode in the item data
	this->SetItemData(node_item, new TreeNodeItemData(i_Node));

	const int num_kids = i_Node->m_Children.size();
	for (int k=0; k<num_kids; k++)
	{
		create_tree_node(node_item, i_Node->m_Children[k]);
	}
	if (i_Node->IsCheckable() && (num_kids > 0))
	{
		update_parent_checked_state(node_item);
	}

	if (i_Node->ShouldSortChildren())
		this->SortChildren(node_item);
}


//------------------------------------------------------------------------
// Find index of this item (position of the item within its siblings)
//------------------------------------------------------------------------
int twcTreeView::get_item_index(wxTreeItemId i_Node)
{
	int index = 0;
	wxTreeItemId id = this->GetPrevSibling(i_Node);
	while (id.IsOk())
	{
		index++;
		id = this->GetPrevSibling(id);
	}
	return index;
}

//--------------------------------------------------------------------
// Item was clicked on, toggle its checked state and adjust
// check state of children and parents.
//--------------------------------------------------------------------
void twcTreeView::set_checked_state(wxTreeItemId i_Item, bool i_bNewCheckState)
{
	set_item_checked(i_Item, i_bNewCheckState);

	// Set the state of all children to match this item's checked state
	// (should there be a flag on this control or on the node that controls this?)
	check_all_children(i_Item, i_bNewCheckState);

	wxTreeItemId root = this->GetRootItem();
	DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

	// Set the checked state of all parent nodes (i.e. may be mixed checked now)
	wxTreeItemId parent_item = this->GetItemParent(i_Item);
	while ((parent_item.IsOk()) && (parent_item != root))
	{
		update_parent_checked_state(parent_item);
		parent_item = this->GetItemParent(parent_item);
	}
}

//------------------------------------------------------------------------
// Set text and background colors to represent highlight state of node
//------------------------------------------------------------------------
void twcTreeView::highlight_item(wxTreeItemId i_ChildItem, bool i_bHighlight)
{
	//this->Freeze();
	if( i_bHighlight )
	{
		//DBG_LOG("Highlight on: " << this->GetItemText(i_ChildItem).char_str());
		//wxColor text_color = (m_bHasFocus ? wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) : this->GetForegroundColour());
		wxColor text_color = wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT );
		this->SetItemTextColour(i_ChildItem, text_color );
		this->SetItemBackgroundColour(i_ChildItem,  wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );			
	}
	else
	{
		//DBG_LOG("Highlight off: " << this->GetItemText(i_ChildItem).char_str());
		this->SetItemTextColour(i_ChildItem, this->GetForegroundColour() );
		this->SetItemBackgroundColour(i_ChildItem,  this->GetBackgroundColour() );			
	}
	//this->Update();
	//this->Thaw();
}

//------------------------------------------------------------------------
// remove highlights from all selected tree items
//------------------------------------------------------------------------
void twcTreeView::remove_all_highlight()
{
	std::vector<wxTreeItemId>::iterator it, end = m_SelectedList.end();
	for(it = m_SelectedList.begin(); it != end; ++it )
	{
		if( (*it).IsOk() )
			highlight_item((*it), false);
	}
}

//------------------------------------------------------------------------
// remove all items from the selected list
//------------------------------------------------------------------------
void twcTreeView::clear_selection()
{
	this->remove_all_highlight();
	m_SelectedList.clear();
}

//------------------------------------------------------------------------
// Update highlights throughout tree based on selected list
//------------------------------------------------------------------------
void twcTreeView::update_highlights()
{
	remove_all_highlight();

	std::vector<wxTreeItemId>::iterator it, end = m_SelectedList.end();
	for(it = m_SelectedList.begin(); it != end; ++it )
	{
		if( (*it).IsOk() )
			highlight_item((*it), true);
	}
}


//------------------------------------------------------------------------
// return true if the node is selected
//------------------------------------------------------------------------
bool twcTreeView::is_node_selected(wxTreeItemId i_Item)
{
	return envSTLHelpers::Contains(m_SelectedList, i_Item);
}

bool twcTreeView::is_node_checkable(wxTreeItemId i_Item)
{
	if (wxTreeItemData *pItemData = this->GetItemData(i_Item))
	{
		TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
		return (pData->m_Node->IsCheckable());
	}
	return true;	// checkable by default
}

//------------------------------------------------------------------------
// Get user data for tree item id
//------------------------------------------------------------------------
shared_ptr<twcTreeNode> twcTreeView::get_tree_node(wxTreeItemId i_Node)
{
	shared_ptr<twcTreeNode> ret_val;
	if (wxTreeItemData *pItemData = this->GetItemData(i_Node))
	{
		TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
		ret_val = pData->m_Node;
	}
	return ret_val;
}

//------------------------------------------------------------------------
// select the given node, deselecting all old nodes
//------------------------------------------------------------------------
void twcTreeView::select_item(wxTreeItemId i_Item, bool i_bSendEvent)
{
	// see if selection is changing
	bool bSameSelection = ((m_SelectedList.size() == 1) && (m_SelectedList[0] == i_Item));
	if (!bSameSelection)
	{
		// clear old selection list
		clear_selection();

		// add the tree item to our selected item list
		m_SelectedList.push_back(i_Item);
		highlight_item(i_Item, true);
	
		if (i_bSendEvent)
		{
			// Fire off the tree view event, passing in the node that was selected
			twcTreeViewEvent select_event( wxEVT_SELECTION_TREE_VIEW, GetId() );
			select_event.SetEventObject( this );
			select_event.SetNode( get_tree_node(i_Item) );
			select_event.SetSelectionType( twcTreeViewEvent::e_Select );
			GetEventHandler()->ProcessEvent( select_event );
		}
	}
}

//------------------------------------------------------------------------
// select the given node, deselecting all old nodes
//------------------------------------------------------------------------
void twcTreeView::toggle_selection(wxTreeItemId i_Item, bool i_bSendEvent)
{
	bool bSelect = (!is_node_selected(i_Item));
	highlight_item(i_Item, bSelect);
	if (bSelect)
		m_SelectedList.push_back(i_Item);
	else
	{
		envSTLHelpers::RemoveOneValue(m_SelectedList, i_Item);
		if (this->GetSelection() == i_Item)
		{
			this->Unselect();	// turn off the single selection on this item
			// Not sure if this is needed...
			// Select something else in the list so that the tree ctrl thinks
			// something is selected.
			//if (!m_SelectedList.empty())
			//{
			//	m_bOurChange = true;
			//	this->SelectItem(m_SelectedList[0]);
			//	m_bOurChange = false;
			//}
		}
	}
	if (i_bSendEvent)
	{
		// Fire off the tree view event, passing in the node that was selected
		twcTreeViewEvent select_event( wxEVT_SELECTION_TREE_VIEW, GetId() );
		select_event.SetEventObject( this );
		select_event.SetNode( get_tree_node(i_Item) );
		if (bSelect)
			select_event.SetSelectionType( twcTreeViewEvent::e_AppendToSelection );
		else
			select_event.SetSelectionType( twcTreeViewEvent::e_RemoveFromSelection );
		GetEventHandler()->ProcessEvent( select_event );
	}
}

//------------------------------------------------------------------------
// append the given nodes to the selection
//------------------------------------------------------------------------
void twcTreeView::append_selection(const std::vector<wxTreeItemId>& i_NodeList, 
										  bool i_bSendEvent)
{
	std::vector< shared_ptr<twcTreeNode> > tree_nodes;

	int num_selected = i_NodeList.size();
	for (int i=0; i<num_selected; ++i)
	{
		wxTreeItemId sel_node = i_NodeList[i];
		if (!is_node_selected(sel_node))
		{
			// add the tree item to our selected item list
			m_SelectedList.push_back(sel_node);
			highlight_item(sel_node, true);

			if (i_bSendEvent)
			{
				shared_ptr<twcTreeNode> node = get_tree_node(sel_node);
				if (node) tree_nodes.push_back(node);
			}
		}
	}

	if (i_bSendEvent)
	{
		// Fire off the tree view event, passing in the node that was selected
		twcTreeViewEvent select_event( wxEVT_SELECTION_TREE_VIEW, GetId() );
		select_event.SetEventObject( this );
		select_event.SetNodes( tree_nodes );
		select_event.SetSelectionType( twcTreeViewEvent::e_AppendToSelection );
		GetEventHandler()->ProcessEvent( select_event );
	}
}

//------------------------------------------------------------------------
// Return nodes between Id1 and Id2. If the nodes do not have the same
// parent, then no nodes are returned.
//------------------------------------------------------------------------
void twcTreeView::get_items_in_range(wxTreeItemId i_Id1, wxTreeItemId i_Id2,
											std::vector<wxTreeItemId>& o_NodeList)
{
	if (i_Id1.IsOk() && i_Id2.IsOk())
	{
		// Make sure the nodes have the same parent
		wxTreeItemId parent_node = this->GetItemParent(i_Id1);
		if (parent_node == this->GetItemParent(i_Id2))
		{
			bool bBetweenNodes = false;
			wxTreeItemIdValue cookie;
			for (wxTreeItemId id = this->GetFirstChild(parent_node, cookie);
				 id.IsOk(); 
				 id = this->GetNextChild(parent_node, cookie))
			{
				if (id == i_Id1 || id == i_Id2)
				{
					if (bBetweenNodes)
						break;
					else
						bBetweenNodes = true;
				}
				else if (bBetweenNodes)
				{
					// Append select the items between the old and new selection
					o_NodeList.push_back(id);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// Selection has changed.
// This is the single selection version
//--------------------------------------------------------------------
void twcTreeView::treeCtrl_Selection(wxTreeEvent& i_Event)
{
	if (m_bOurChange || m_bUpdating)
		return;

	//if (i_Event.GetItem().IsOk())
	//	DBG_LOG("Selection node: " << this->GetItemText(i_Event.GetItem()).c_str());
	//else
	//	DBG_LOG("Selection node: null");	

	wxTreeItemId sel_node = this->GetSelection();
	if (sel_node.IsOk() && this->GetItemData(sel_node))
	{
		// can't get the modifier keys from here in wxWidgets,
		// so we get the state of the buttons in mouse events elsewhere
		if (this->m_bCtrlPressed)
		{
			// CTRL modifier means "toggle selection"
			toggle_selection(sel_node);
		}
		else if (this->m_bShiftPressed )
		{
			// Append selections
			std::vector<wxTreeItemId> node_list;

			// Get items between old node and selection node
			wxTreeItemId old_node = i_Event.GetOldItem();
			get_items_in_range(sel_node, old_node, node_list);

			// Select new item last
			node_list.push_back(sel_node);
			append_selection(node_list);
		}
		//else if (this->m_bAltPressed )
		//{
		//	wxArrayTreeItemIds selection;
		//	gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed);
		//	int num_selected = this->GetSelections(selection);
		//	for(int i =0;i<num_selected;i++)
		//	{
		//		wxTreeItemId sel_node = selection[i];
		//		if(sel_node.IsOk())
		//		{
		//			if(this->ItemHasChildren(sel_node))
		//			{
		//				this->Expand(sel_node);
		//			}
		//		}
		//	}
		//}
		else
		{
			// Select item alone
			select_item(sel_node);
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcTreeView::treeCtrl_LeftMouseDown( wxMouseEvent& i_Event )
{
	bool bSkip = true;

	// Store states of modifier keys to get them later
	// during a selection event
	m_bCtrlPressed = i_Event.m_controlDown;
	m_bShiftPressed = i_Event.m_shiftDown;
	m_bAltPressed = i_Event.m_altDown;


	int flags = 0;
	wxTreeItemId pick_item = this->HitTest(i_Event.GetPosition(), flags);
	if (pick_item.IsOk())
	{
		// Check for click on the icon
		//if (flags & wxTREE_HITTEST_ONITEMICON)
		if (flags & wxTREE_HITTEST_ONITEMSTATEICON) // state image used for checkbox now
		{
			// Do we have to confirm that this node is checkable here first?

			// Get the new check state based on the item that was clicked on
			// and then apply it to all selected items
			bool bNewCheckState = (!get_item_checked(pick_item));

			std::vector<wxTreeItemId> node_list;
			if (!m_bCtrlPressed)
			{
				if (is_node_selected(pick_item) || m_bShiftPressed)
				{
					// Apply check change to all previously selected nodes.
					node_list = m_SelectedList;
				}

				// If the user was pressing SHIFT when clicking on the checkbox, then 
				// include all nodes between the last click and the current item also.
				if (m_bShiftPressed && m_FirstShift.IsOk())
				{
					std::vector<wxTreeItemId> shift_selected;
					get_items_in_range(pick_item, m_FirstShift, shift_selected);
					for (int i=0; i<shift_selected.size(); ++i)
					{
						if (!envSTLHelpers::Contains(node_list, shift_selected[i]))
							node_list.push_back(shift_selected[i]);
					}
				}
			}
			
			// Finally, add in the pick item itself 
			// if not already in the list
			if (!envSTLHelpers::Contains(node_list, pick_item))
			{
				node_list.push_back(pick_item);
			}

			if (node_list.size() > 1)
			{
				// Do three modifications to the selection list...
				// 1) Remove all nodes that are not checkable
				// 2) Remove all children of the picked node because they will be set
				// by the picked node's change.
				// 3) Remove all parents of the selection (not including the pick node)
				// from the selection list because the children are a more specific selection
				// and changing the parent would change the child nodes that aren't selected.
				//TODO - still to do this filtering
			}

			// Convert from tree ids to twcTreeNode objects to pass to callback.
			std::vector< shared_ptr<twcTreeNode> > selected_nodes;
			int num_selected = node_list.size();
			for (int i=0; i<num_selected; ++i)
			{
				wxTreeItemId sel_node = node_list[i];
				if (wxTreeItemData *pItemData = this->GetItemData(sel_node))
				{
					TreeNodeItemData *pData = static_cast<TreeNodeItemData*>(pItemData);
					selected_nodes.push_back(pData->m_Node);

					// Should the twcTreeNode class itself have a "checked" state for us to maintain,
					// or is that up to the user?

					if (pData->m_Node->IsCheckable())
						set_checked_state(sel_node, bNewCheckState);
				}
			}

			// Fire off the tree view event, passing in the nodes that were checked
			twcTreeViewEvent check_event( wxEVT_CHECKED_TREE_VIEW, GetId() );
			check_event.SetEventObject( this );
			check_event.SetNodes( selected_nodes );
			check_event.SetCheckedState( bNewCheckState );
			GetEventHandler()->ProcessEvent( check_event );
			
			// Use flag to decide if we pass the mouse event on to the code that
			// selects the item itself.
			// This might be better handled by looking at the "Selectable"
			// state of the node itself.
			bool bAlreadySelected = is_node_selected(pick_item);
			bSkip = (m_bSelectOnCheckChange && !bAlreadySelected);
		}
		else if (!m_bCtrlPressed && !m_bShiftPressed)
		{
			// This is not a pick on the checkbox icon. We used to skip the
			// event and let the tree control handle the selection, but 
			// there was an odd behavior if you moved too fast and moved the
			// mouse off the item before lifted the mouse button where the
			// item would appear to select and then go back to the old selection.
			// So, now I am going to do the SelectItem right here on left mouse down
			// and skip the event in order to let wxWidgets handle the focus switched 
			// and expand/compress events.
			if (this->GetSelection() != pick_item)
				this->SelectItem(pick_item);
			//bSkip = false;
		}
		else if (m_bCtrlPressed && (this->GetSelection() == pick_item))
		{
			// Ctrl on the selected item means deselect. It won't pass through the
			// selection callback above, so we have to catch it here.
			//this->SelectItem(pick_item, false); // this would be for multiple selection tree view
			this->Unselect();	// this is for single selection tree view (with highlights)
		}
		
		m_FirstShift = pick_item;
	}
	i_Event.Skip(bSkip);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void twcTreeView::treeCtrl_LeftMouseUp( wxMouseEvent& i_Event )
{
	m_bCtrlPressed = false;
	m_bShiftPressed = false;
	m_bAltPressed = false;
	i_Event.Skip();
}

//--------------------------------------------------------------------
// Have to update our highlight text color when we lose and gain focus
//--------------------------------------------------------------------
//void twcTreeView::OnSetFocus(wxFocusEvent& i_Event)
//{
//	m_bHasFocus = true;
//
//	std::vector<wxTreeItemId>::iterator it, end = m_SelectedList.end();
//	for(it = m_SelectedList.begin(); it != end; ++it )
//	{
//		if( (*it).IsOk() )
//			highlight_item((*it), true);
//	}
//}
//void twcTreeView::OnLostFocus(wxFocusEvent& i_Event)
//{
//	m_bHasFocus = false;
//
//	std::vector<wxTreeItemId>::iterator it, end = m_SelectedList.end();
//	for(it = m_SelectedList.begin(); it != end; ++it )
//	{
//		if( (*it).IsOk() )
//			highlight_item((*it), true);
//	}
//}


//--------------------------------------------------------------------
// code implementing the event type and the event class
//--------------------------------------------------------------------
DEFINE_LOCAL_EVENT_TYPE( wxEVT_CHECKED_TREE_VIEW )
DEFINE_LOCAL_EVENT_TYPE( wxEVT_SELECTION_TREE_VIEW )

//--------------------------------------------------------------------
//--------------------------------------------------------------------
twcTreeViewEvent::twcTreeViewEvent(wxEventType i_CommandType, int i_Id)
: wxCommandEvent(i_CommandType, i_Id)
{
}

//--------------------------------------------------------------------
// required for sending with wxPostEvent()
//--------------------------------------------------------------------
wxEvent* twcTreeViewEvent::Clone()
{
	return new twcTreeViewEvent(*this);
}


#endif // USE_WXWIDGETS
