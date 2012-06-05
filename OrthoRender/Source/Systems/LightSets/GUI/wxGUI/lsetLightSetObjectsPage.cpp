/*****************************************************************************
**	lsetLightSetObjectsPage.cpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetObjectsPage.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>

#ifdef USE_WXWIDGETS

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

}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
lsetLightSetObjectsPage* lsetLightSetObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObjectsPage::lsetLightSetObjectsPage( wxWindow* parent )
: lsetLightSetObjectsPageBase( parent )
{
	// Create state images to represent checked state
	wxImageList *pCheckImages = new wxImageList(13, 13, false, e_NumPlacedImageStates);
	pCheckImages->Add(wxBitmap("Data/Icons/tree-unchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("Data/Icons/tree-checked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap("Data/Icons/tree-mixchecked.png", wxBITMAP_TYPE_PNG));
	m_treeCtrl_Objects->AssignImageList(pCheckImages); // tree ctrl takes ownership
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObjectsPage::~lsetLightSetObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG0("Closing lsetLightSetObjectsPage()");
	if (lsetLightSetObjectsPage::Instance == this)
		lsetLightSetObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------	
void lsetLightSetObjectsPage::Clear()
{
	//	clear the controls
	m_treeCtrl_Objects->DeleteAllItems();
	m_LightSetName = nameString();
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------	
void lsetLightSetObjectsPage::Update(const nameString& i_LightSetName)
{	
	m_LightSetName = i_LightSetName;

	std::vector<nameString> all_objects;
	ltstLightSetMgr::GetAllObjects(all_objects);

	std::vector<nameString> set_objects;
	ltstLightSetMgr::GetObjectsInSet(m_LightSetName, set_objects);

	m_treeCtrl_Objects->Freeze();
	m_treeCtrl_Objects->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl_Objects->AddRoot("Root");

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());

		wxTreeItemId object_item = m_treeCtrl_Objects->AppendItem(root, all_objects[i].GetString(), 
			checked ? e_Checked : e_Unchecked);

		std::vector<std::string> node_names;
		ltstLightSetMgr::GetFragmentNodeNames(all_objects[i], node_names);

		std::vector<int> lit_fragment_indices;
		if (checked)
		{
			ltstLightSetMgr::GetLitFragmentIndices(m_LightSetName, 
						   all_objects[i],
						   lit_fragment_indices);
		}

		const int num_nodes = node_names.size();
		bool bChildChecked;
		for (int n=0; n<num_nodes; n++)
		{
			if (!checked)
				bChildChecked = false;
			else if (lit_fragment_indices.empty())
				bChildChecked = true;
			else
				bChildChecked = envSTLHelpers::Contains(lit_fragment_indices, n);

			m_treeCtrl_Objects->AppendItem(object_item, node_names[n], 
				bChildChecked ? e_Checked : e_Unchecked);
		}
		if (num_nodes > 0)
			update_parent_checked_state(m_treeCtrl_Objects, object_item);
	}
	m_treeCtrl_Objects->Thaw();

}


//--------------------------------------------------------------------
// Item was clicked on, toggle its checked state
//--------------------------------------------------------------------
void lsetLightSetObjectsPage::toggle_checked_state(wxTreeItemId i_Item)
{
	bool bNewCheckState = (!get_item_checked(m_treeCtrl_Objects, i_Item));
	set_item_checked(m_treeCtrl_Objects, i_Item, bNewCheckState);

	// Identify the object
	std::vector<nameString> all_objects;
	ltstLightSetMgr::GetAllObjects(all_objects);

	wxTreeItemId root = m_treeCtrl_Objects->GetRootItem();
	DBG_ASSERT0(root.IsOk(), "Assuming that there is a root node already in tree control.");

	bool bRoot = (m_treeCtrl_Objects->GetItemParent(i_Item) == root);
	wxTreeItemId object_item = (bRoot) ? i_Item : m_treeCtrl_Objects->GetItemParent(i_Item);
	int index = get_item_index(m_treeCtrl_Objects, object_item);
	if (index >= 0)
	{
		if (bRoot)
		{
			// Root node, turn on/off whole object
			if (bNewCheckState)
				lsetOperations::AddObjectToLightSet(m_LightSetName, all_objects[index]);
			else
				lsetOperations::RemoveObjectFromLightSet(m_LightSetName, all_objects[index]);

			// set checked state of child nodes, but don't notify
			wxTreeItemIdValue cookie;
			for (wxTreeItemId id = m_treeCtrl_Objects->GetFirstChild(i_Item, cookie);
				 id.IsOk(); 
				 id = m_treeCtrl_Objects->GetNextChild(i_Item, cookie))
			{
				set_item_checked(m_treeCtrl_Objects, id, bNewCheckState);
			}
		}
		else
		{
			// Child node, turn on/off just the fragment
			int node_index = get_item_index(m_treeCtrl_Objects, i_Item);
			if (node_index >= 0)
			{
				if (bNewCheckState)
				{
					lsetOperations::AddNodeToLightSet(m_LightSetName, all_objects[index], node_index);
				}
				else
				{
					lsetOperations::RemoveNodeFromLightSet(m_LightSetName, all_objects[index], node_index);
				}
				update_parent_checked_state(m_treeCtrl_Objects, object_item);
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void lsetLightSetObjectsPage::treeCtrl_Objects_LeftMouseDown( wxMouseEvent& i_Event)
{
	bool bSkip = true;

	int flags = 0;
	wxTreeItemId pick_item = m_treeCtrl_Objects->HitTest(i_Event.GetPosition(), flags);
	if (pick_item.IsOk())
	{
		// Check for click on the icon
		if (flags & wxTREE_HITTEST_ONITEMICON)
		{
			toggle_checked_state(pick_item);
			bSkip = false;
		}
	}
	i_Event.Skip(bSkip);
}

#endif // USE_WXWIDGETS
