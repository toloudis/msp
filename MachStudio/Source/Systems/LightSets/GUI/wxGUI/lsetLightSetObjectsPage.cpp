/*****************************************************************************
**	lsetLightSetObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/wxGUI/lsetLightSetObjectsPage.hpp"
#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"

#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

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
lsetLightSetObjectsPage* lsetLightSetObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObjectsPage::lsetLightSetObjectsPage( wxWindow* parent )
: lsetLightSetObjectsPageBase( parent )
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
lsetLightSetObjectsPage::~lsetLightSetObjectsPage()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing lsetLightSetObjectsPage()");
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
	wxTreeItemId root = m_treeCtrl_Objects->AddRoot(L"Root");

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());

		wxTreeItemId object_item = m_treeCtrl_Objects->AppendItem(root, 
			wxString(all_objects[i].GetString().c_str(), wxConvUTF8), 
			(checked ? e_Checked : e_Unchecked));

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
	m_treeCtrl_Objects->SortChildren(m_treeCtrl_Objects->GetRootItem());
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
	DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

	bool bRoot = (m_treeCtrl_Objects->GetItemParent(i_Item) == root);
	wxTreeItemId object_item = (bRoot) ? i_Item : m_treeCtrl_Objects->GetItemParent(i_Item);
	int index = get_item_index(m_treeCtrl_Objects, object_item);
	wxString itemName = m_treeCtrl_Objects->GetItemText(object_item);
	if (index >= 0)
	{
		nameString objectName(std::string(itemName.utf8_str()));
		if (bRoot)
		{
			// Root node, turn on/off whole object
			if (bNewCheckState)
				lsetOperations::AddObjectToLightSet(m_LightSetName, objectName);
			else
				lsetOperations::RemoveObjectFromLightSet(m_LightSetName, objectName);

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
			//int node_index = get_item_index(m_treeCtrl_Objects, i_Item);
			std::vector<std::string> node_names;
			ltstLightSetMgr::GetFragmentNodeNames(objectName, node_names);
			//wxString nodeName = m_treeCtrl_Objects->GetItemText(i_Item);
			//int node_index = get_node_index_by_name(nameString(std::string(nodeName.utf8_str())), node_names);
			
			if (wxTreeItemData *pItemData = m_treeCtrl_Objects->GetItemData(i_Item))
			{
				NodeIndexTreeItemData *pData = static_cast<NodeIndexTreeItemData*>(pItemData);
				int node_index = pData->m_Index;
				if (bNewCheckState)
				{
					lsetOperations::AddNodeToLightSet(m_LightSetName, objectName, node_index);
				}
				else
				{
					lsetOperations::RemoveNodeFromLightSet(m_LightSetName, objectName, node_index);
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
