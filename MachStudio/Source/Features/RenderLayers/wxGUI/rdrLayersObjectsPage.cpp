/*****************************************************************************
**	rdrLayersObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderLayers/wxGUI/rdrLayersObjectsPage.hpp"

#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <vector>


#ifdef USE_WXWIDGETS
//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum PlacedImageState
	{
		e_Unchecked = 0,
		e_Checked = 1,
		e_MixChecked = 2,
		e_NumPlacedImageStates
	};

	//--------------------------------------------------------------------
	// Tree item data contains index for fragment node within object.
	// Needed because some fragment names are not unique when
	// using mesh instancing.
	//--------------------------------------------------------------------
	class NodeIndexTreeItemData : public wxTreeItemData
	{
	public:
		NodeIndexTreeItemData(int i_Index)
			: m_Index(i_Index) {}
		int m_Index;
	};

	bool l_bCtrl;
	bool l_bShift;
	bool l_bItemRemoved;
	wxTreeItemId l_FirstShift;
	std::vector<wxTreeItemId> l_SelectedList;

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
	// Find index of this item (position of the item within its siblings)
	//------------------------------------------------------------------------
	int get_item_index( wxTreeCtrl* i_pTreeCtrl,
						wxTreeItemId i_Node)
	{
		int index = 0;
		wxTreeItemId id = i_pTreeCtrl->GetPrevSibling(i_Node);
		while (id.IsOk())
		{
			index++;
			id = i_pTreeCtrl->GetPrevSibling(id);
		}
		return index;
	}

	//------------------------------------------------------------------------
	// return the oldest child in the given tree
	//------------------------------------------------------------------------
	wxTreeItemId& get_oldest_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildOne, wxTreeItemId& i_ChildTwo)
	{
		wxTreeItemId parent = i_TreeCtrl->GetItemParent(i_ChildOne);
		wxTreeItemIdValue cookie;

		wxTreeItemId id;
		for (id = i_TreeCtrl->GetFirstChild(parent, cookie);
			 id.IsOk(); 
			 id = i_TreeCtrl->GetNextChild(id, cookie))
		{
			if( id == i_ChildOne )
				return i_ChildOne;
			else if( id == i_ChildTwo )
				return i_ChildTwo;
		}
		//invalidate i_ChildOne to return it as not OK.
		i_ChildOne.Unset();
		return i_ChildOne;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void highlight_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem, bool i_bHighlight)
	{
		i_TreeCtrl->Freeze();
		if( i_bHighlight )
		{
			i_TreeCtrl->SetItemTextColour(i_ChildItem, wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHTTEXT ) );
			i_TreeCtrl->SetItemBackgroundColour(i_ChildItem,  wxSystemSettings::GetColour( wxSYS_COLOUR_HIGHLIGHT ) );			
		}
		else
		{
			i_TreeCtrl->SetItemTextColour(i_ChildItem, i_TreeCtrl->GetForegroundColour() );
			i_TreeCtrl->SetItemBackgroundColour(i_ChildItem,  i_TreeCtrl->GetBackgroundColour() );			
		}
		i_TreeCtrl->Update();
		i_TreeCtrl->Thaw();
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list
	//------------------------------------------------------------------------
	void add_selected_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for( it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
			{
				//DBG_TRACE( "tree item is already highlighted ");
				return;
			}
		}

		// add the tree item to our selected item list
		l_SelectedList.push_back(i_ChildItem);
		highlight_child(i_TreeCtrl, i_ChildItem, true);
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list, return true if the item was removed
	//------------------------------------------------------------------------
	bool remove_if_selected_child(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
			{
				l_SelectedList.erase(it);
				highlight_child(i_TreeCtrl, i_ChildItem, false);
				return true;
			}		
		}
		return false;
	}

	//------------------------------------------------------------------------
	// add a tree item to our selected list
	//------------------------------------------------------------------------
	bool child_selected(wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_ChildItem)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it) == i_ChildItem )
				return true;
		}
		return false;
	}
	//------------------------------------------------------------------------
	// remove all items from the selected list
	//------------------------------------------------------------------------
	void remove_all_highlight(wxTreeCtrl* i_TreeCtrl)
	{
		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it).IsOk() )
				highlight_child(i_TreeCtrl, (*it), false);
		}
		l_SelectedList.clear();
	}

	//------------------------------------------------------------------------
	// Highlight all child items in the range given
	//------------------------------------------------------------------------
	void highlight_child_range( wxTreeCtrl* i_TreeCtrl, wxTreeItemId& i_StartChild, wxTreeItemId& i_FinishChild )
	{
		//remove_all_highlight(i_TreeCtrl);
		wxTreeItemIdValue cookie;
		for (wxTreeItemId id = i_StartChild;
			 id.IsOk(); 
			 id = i_TreeCtrl->GetNextChild(id, cookie))
		{
			add_selected_child(i_TreeCtrl, id);
			if( id == i_FinishChild )
				return;
		}
	}

	void update_highlights(wxTreeCtrl* i_TreeCtrl)
	{
		std::vector<wxTreeItemId> temp = l_SelectedList;
		remove_all_highlight(i_TreeCtrl);
		l_SelectedList = temp;

		std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
		for(it = l_SelectedList.begin(); it != end; ++it )
		{
			if( (*it).IsOk() )
				highlight_child(i_TreeCtrl, (*it), true);
		}
	}
	//int get_node_index_by_name( const nameString& i_NodeName, 
	//							std::vector<std::string> i_NodeNames )
	//{
	//	for( int i = 0; i < i_NodeNames.size(); ++i )
	//	{
	//		if( i_NodeName.GetString() == i_NodeNames[i] )
	//			return i;
	//	}
	//	return -1;
	//}

}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
rdrLayersObjectsPage* rdrLayersObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rdrLayersObjectsPage::rdrLayersObjectsPage( wxWindow* parent )
: rdrLayersObjectsPageBase( parent )
{
	// Create state images to represent checked state
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	wxImageList *pCheckImages = new wxImageList(FromDIP(13), FromDIP(13), false, e_NumPlacedImageStates);
	std::wstring image_dir( icon_dir.GetString() );
	{
        wxLogNull nullLog;
        pCheckImages->Add(wxBitmap(image_dir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(image_dir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
    }
	m_treeCtrl_Objects->AssignImageList(pCheckImages); // tree ctrl takes ownership
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rdrLayersObjectsPage::~rdrLayersObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing rdrLayersObjectsPage()");
	if (rdrLayersObjectsPage::Instance == this)
		rdrLayersObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void rdrLayersObjectsPage::Clear()
{
	//	clear the controls
	m_treeCtrl_Objects->DeleteAllItems();
	m_RenderLayerName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void rdrLayersObjectsPage::Update(const nameString& i_RenderLayerName)
{	
	m_RenderLayerName = i_RenderLayerName;

	std::vector<nameString> all_objects;
	rlyrRenderLayerMgr::GetAllObjects(all_objects);
	
	m_treeCtrl_Objects->Freeze();
	m_treeCtrl_Objects->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl_Objects->AddRoot(L"Root");

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		bool checked = rlyrRenderLayerMgr::GetLayerObjectState(m_RenderLayerName, all_objects[i]);

		wxTreeItemId object_item = m_treeCtrl_Objects->AppendItem(root, 
			wxString(all_objects[i].GetString().c_str(), wxConvUTF8), 
			(checked ? e_Checked : e_Unchecked));

		std::vector<std::string> node_names;
		rlyrRenderLayerMgr::GetFragmentNodeNames(all_objects[i], node_names);

		const int num_nodes = node_names.size();
		bool bChildChecked;
		for (int n=0; n<num_nodes; n++)
		{
			if (!checked)
				bChildChecked = false;
			else
				bChildChecked = rlyrRenderLayerMgr::GetLayerFragmentState(m_RenderLayerName, all_objects[i], n);

			wxTreeItemId node_item = m_treeCtrl_Objects->AppendItem(object_item, 
				wxString(node_names[n].c_str(), wxConvUTF8), 
				(bChildChecked ? e_Checked : e_Unchecked));		

			// Because mesh instancing is going to cause duplicate names to 
			// appear in this list, have to store the index by hand using tree item data.
			m_treeCtrl_Objects->SetItemData(node_item, new NodeIndexTreeItemData(n));

		}
		if (num_nodes > 0)
			update_parent_checked_state(m_treeCtrl_Objects, object_item);

		m_treeCtrl_Objects->SortChildren(object_item);
	}
	//l_FirstShift = m_treeCtrl_Objects->GetSelection();
	m_treeCtrl_Objects->SortChildren(m_treeCtrl_Objects->GetRootItem());
	m_treeCtrl_Objects->Thaw();

}


//--------------------------------------------------------------------
// Item was clicked on, toggle its checked state
//--------------------------------------------------------------------
void rdrLayersObjectsPage::toggle_checked_state(wxTreeItemId i_Item)
{
	bool bNewCheckState = (!get_item_checked(m_treeCtrl_Objects, i_Item));

	// Identify the object
	std::vector<nameString> all_objects;
	rlyrRenderLayerMgr::GetAllObjects(all_objects);

	wxTreeItemId root = m_treeCtrl_Objects->GetRootItem();
	DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

	wxTreeItemId cur_item;
	std::vector<wxTreeItemId>::iterator it, end = l_SelectedList.end();
	for( it = l_SelectedList.begin(); it != end; ++it )
	{
		cur_item = (*it);
		set_item_checked(m_treeCtrl_Objects, cur_item, bNewCheckState);
		bool bRoot = (m_treeCtrl_Objects->GetItemParent(cur_item) == root);
		wxTreeItemId object_item = (bRoot) ? cur_item : m_treeCtrl_Objects->GetItemParent(cur_item);
		int index = get_item_index(m_treeCtrl_Objects, object_item);
		wxString itemName = m_treeCtrl_Objects->GetItemText(object_item);
		if (index >= 0)
		{
			nameString objectName(std::string(itemName.utf8_str()));
			if (bRoot)
			{
				
				// set checked state of child nodes, but don't notify
				wxTreeItemIdValue cookie;
				for (wxTreeItemId id = m_treeCtrl_Objects->GetFirstChild(cur_item, cookie);
					 id.IsOk(); 
					 id = m_treeCtrl_Objects->GetNextChild(cur_item, cookie))
				{
					set_item_checked(m_treeCtrl_Objects, id, bNewCheckState);
					//also set the state of each fragment in the current render layer
				}
				rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, bNewCheckState);
				int num_nodes = rlyrRenderLayerMgr::GetObjectNumNodes(objectName);
				for(int n = 0; n < num_nodes; ++n)
				{
					//make sure each node matches the object's state
					rlyrRenderLayerMgr::SetLayerFragmentState(m_RenderLayerName, objectName, n, bNewCheckState);
				}
				
				rlyrLayersDocumentChunk::ActiveDataChanged();
			}
			else
			{
				// Child node, turn on/off just the fragment
				//int node_index = get_item_index(m_treeCtrl_Objects, cur_item);
				std::vector<std::string> node_names;
				rlyrRenderLayerMgr::GetFragmentNodeNames(objectName, node_names);
				
				if (wxTreeItemData *pItemData = m_treeCtrl_Objects->GetItemData(cur_item))
				{
					NodeIndexTreeItemData *pData = static_cast<NodeIndexTreeItemData*>(pItemData);
					int node_index = pData->m_Index;
					//set the state of the current fragment in the current render layer
					
					rlyrRenderLayerMgr::SetLayerFragmentState(m_RenderLayerName, objectName, node_index, bNewCheckState);
					
					//if turning on a fragment, make sure the parent object is turned on
					if(bNewCheckState)
					{
						rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, bNewCheckState);
					}
					rlyrLayersDocumentChunk::ActiveDataChanged();

					update_parent_checked_state(m_treeCtrl_Objects, object_item);
					int image = m_treeCtrl_Objects->GetItemImage( object_item );
					if( image == e_Unchecked )
					{
						rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, false);
					}
				
				}
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersObjectsPage::treeCtrl_Objects_LeftMouseDown( wxMouseEvent& i_Event)
{
	bool bSkip = true;

	l_bCtrl = i_Event.m_controlDown;
	l_bShift = i_Event.m_shiftDown;

	int flags = 0;
	wxTreeItemId pick_item = m_treeCtrl_Objects->HitTest(i_Event.GetPosition(), flags);
	if (pick_item.IsOk())
	{
		// Check for click on the icon
		if (flags & wxTREE_HITTEST_ONITEMICON)
		{
			if( !child_selected(m_treeCtrl_Objects, pick_item) )
			{
				remove_all_highlight(m_treeCtrl_Objects);
			}
			m_treeCtrl_Objects->SelectItem( pick_item );
			add_selected_child(m_treeCtrl_Objects, pick_item);
			l_FirstShift = pick_item;
			toggle_checked_state(pick_item);
			bSkip = false;
		}

		//remove the highlights if not doing multiple selection
		if( !l_bCtrl && !l_bShift )
		{
			m_treeCtrl_Objects->SelectItem( pick_item );
			l_FirstShift = pick_item;
			remove_all_highlight(m_treeCtrl_Objects);
			add_selected_child(m_treeCtrl_Objects, pick_item);
		}	
		else if( l_bCtrl && !l_bShift )
		{
			l_FirstShift = pick_item;
			//if the current selection isn't highlighted, highlight
			//if it is remove it's highlight
			if(remove_if_selected_child(m_treeCtrl_Objects, pick_item))
			{
				if( pick_item == m_treeCtrl_Objects->GetSelection() )
				{
					if(l_SelectedList.size() > 0)
						m_treeCtrl_Objects->SelectItem( l_SelectedList[0] );
				}
				update_highlights(m_treeCtrl_Objects);
				return;
			}
			add_selected_child(m_treeCtrl_Objects, pick_item);
			update_highlights(m_treeCtrl_Objects);
			return;
		}
		else if( !l_bCtrl && l_bShift )
		{
			if( !l_FirstShift.IsOk() )
			{
				l_FirstShift = pick_item;
			}
			else
			{
				//check for oldest child
				wxTreeItemId oldest_item = get_oldest_child(m_treeCtrl_Objects, l_FirstShift, pick_item);

				if( !oldest_item.IsOk() )
					return;

				//highlight in range oldest to youngest child
				if( oldest_item == l_FirstShift )
					highlight_child_range(m_treeCtrl_Objects, l_FirstShift, pick_item);
				else
					highlight_child_range(m_treeCtrl_Objects, pick_item,l_FirstShift);
			}
		}
	}
	i_Event.Skip(bSkip);
}

#endif // USE_WXWIDGETS
