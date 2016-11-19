/*****************************************************************************
**	cmmPlacedPane.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/wxGUI/cmmPlacedPane.hpp"
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSceneOperations.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneViewCategory.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneViewGroup.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneViewLights.hpp"
#include "Systems/Common/GUI/wxGUI/cmmPlacedImages.hpp"
#include "Systems/Common/GUI/wxGUI/cmmSceneTreeNode.hpp"

#include "Support/gsup/gsupTreeCtrlUtil.hpp"
#include "Support/mnm/mnmThinkMgr.hpp"
#include "Support/vis/visMgr.hpp"

#include "Core/App/appTime.hpp"
#include "Core/Env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"
#include "ToolUIWx/twx/twxContextMenu.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twc/twcTreeView.hpp"


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default

//============================================================================
//============================================================================
namespace
{
	const bool c_bExpandObjectParts = false; // Should object parts be expanded by default?
	nameString prevPickName("");
	//typedef std::set<wxTreeItemId> TreeItemIdSet;
	typedef std::list<wxTreeItemIdValue> TreeItemIdSet;

	//--------------------------------------------------------------------
	// Icon directory as wxString
	//--------------------------------------------------------------------
	wxString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return wxString(wide_dir.GetString());
	}

	struct pick_object_filter
	{
		sel3dObject* m_pPickObject;
		pick_object_filter(sel3dObject* i_pPickObject) : m_pPickObject(i_pPickObject) {}

		bool operator()(const shared_ptr<twcTreeNode>& i_Node)
		{
			cmmSceneTreeNode* pNode  = static_cast<cmmSceneTreeNode*>(i_Node.get());
			return (m_pPickObject == pNode->GetSceneObject());
		}

	};
		typedef boost::function<bool (const shared_ptr<twcTreeNode>&)> NodeFilterFunction;

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmPlacedPane::cmmPlacedPane( wxWindow* parent, ViewType i_Type ) 
//bga - If this next line isn't compiling, it may be that the "Base"
// class was regenerated from wxFormBuilder, which means it is using
// the wrong path to the icons. That file has to be altered by hand
// to use the given icon path.
:	cmmPlacedPaneBase( parent, get_icon_directory() ),
	m_bOurChange(false),
	m_bCtrlPressed(false), 
	m_bShiftPressed(false),
	m_bAltPressed(false)
{
	// Create scene view that will determine how the placed objects are displayed
	// in our tree view
	if (i_Type == e_GroupView)
		m_SceneView.reset( new cmmSceneViewGroup() );
	else if (i_Type == e_LightsView)
		m_SceneView.reset( new cmmSceneViewLights() );
	else
		m_SceneView.reset( new cmmSceneViewCategory() );

	// Select Available pane to display
	//m_Notebook1->SetSelection(0);

	// Create state images to represent checked state
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	std::wstring icondir = icon_dir.GetString();
	wxImageList *pCheckImages = new wxImageList(FromDIP(13), FromDIP(13), false, cmmPlacedImages::e_NumPlacedImageStates);
	if (i_Type == e_LightsView)
	{
        wxLogNull nullLog;
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-disabled.png", wxBITMAP_TYPE_PNG)); // duplicate for the "no checkbox" state
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-disabled.png", wxBITMAP_TYPE_PNG));
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-enabled.png", wxBITMAP_TYPE_PNG));
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-mixenabled.png", wxBITMAP_TYPE_PNG));
	}
	else
	{
        wxLogNull nullLog;
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG)); // duplicate for the "no checkbox" state
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
		pCheckImages->Add(wxBitmap(icondir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
	}

    {
        wxLogNull nullLog;
    	pCheckImages->Add(wxBitmap(icondir + L"\\tree-controlpart.png", wxBITMAP_TYPE_PNG));
	    //pCheckImages->Add(wxBitmap(icondir + L"\\tree-expressionpart.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(icondir + L"\\tree-materialpart.png", wxBITMAP_TYPE_PNG));
	    pCheckImages->Add(wxBitmap(icondir + L"\\tree-surfacepart.png", wxBITMAP_TYPE_PNG));
    }

	m_treeCtrl_Placed->AssignStateImageList(pCheckImages); // tree ctrl takes ownership

	
	// Event callbacks
	m_treeCtrl_Placed->Connect( wxEVT_CHECKED_TREE_VIEW, 
		EVT_TWC_TREE_VIEW_FUNC( cmmPlacedPane::treeCtrl_Placed_Checked ), NULL, this );
	m_treeCtrl_Placed->Connect( wxEVT_SELECTION_TREE_VIEW, 
		EVT_TWC_TREE_VIEW_FUNC( cmmPlacedPane::treeCtrl_Placed_SelectionChange ), NULL, this );

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmPlacedPane::~cmmPlacedPane()
{
	// wxWidgets will delete the dialog when the main form closes
	//DBG_LOG("Closing cmmPlacedPane()");

	m_treeCtrl_Placed->Disconnect( wxEVT_CHECKED_TREE_VIEW, 
		EVT_TWC_TREE_VIEW_FUNC( cmmPlacedPane::treeCtrl_Placed_Checked ), NULL, this );
	m_treeCtrl_Placed->Disconnect( wxEVT_SELECTION_TREE_VIEW, 
		EVT_TWC_TREE_VIEW_FUNC( cmmPlacedPane::treeCtrl_Placed_SelectionChange ), NULL, this );

}

//--------------------------------------------------------------------
// Assign image list to tree control
//--------------------------------------------------------------------
void cmmPlacedPane::AssignImageList(wxImageList *i_ImageList)
{
	m_treeCtrl_Placed->AssignImageList(i_ImageList);
}
	
//--------------------------------------------------------------------
// Clear scene data from form
//--------------------------------------------------------------------	
void cmmPlacedPane::Clear()
{
	//	clear the tree
	m_treeCtrl_Placed->Clear();

	//	change the button states
	enable_placed_buttons();
	//enable_available_buttons();
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmPlacedPane::UpdateDialog()
{
	//DBG_LOG(" cmmPlacedPane::UpdateDialog(), loading: " << docSingleDocumentMgr::IsLoading());

	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		m_DataListPlaced.clear();
		cmmDialogInterestMgr::GetPlacedObjects(m_DataListPlaced);

		updatetreeview_system_placed();

		// When first building the placed list, expand all systems
		gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts);
					
		enable_placed_buttons();
	}
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------	
void cmmPlacedPane::UpdatePlacedList(const std::string& i_SystemName, 
									  cmmDialogDataList& i_DataList)
{
	//DBG_LOG(" cmmPlacedPane::UpdatePlacedList(), loading: " << docSingleDocumentMgr::IsLoading());

	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		cmmDialogInterestMgr::UpdateList(i_SystemName, i_DataList, m_DataListPlaced);

		//Note: could handle this better by searching for only the needed changes
		updatetreeview_system_placed();

	}
}

//------------------------------------------------------------------------
//	Find the item on the placed tree an highlight it. 
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//------------------------------------------------------------------------
void cmmPlacedPane::SelectObjectOnPlacedList(sel3dObject* i_pPickObject)
{
	wxTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node.IsOk())
	{
	//	m_bOurChange = true;
		m_treeCtrl_Placed->SelectNode(found_node);
	//	enable_placed_buttons();
	//	m_bOurChange = false;
	}
	else
	{
		//DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetDisplayName().c_str());

		// Because of the multiple views (tabs) into the scnee manager, it sometimes
		// happens that an object does not exists in a certain view - so we need to
		// clear the selection in this case.
		m_treeCtrl_Placed->UnselectAll();
	}
}


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmPlacedPane::AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	wxTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node.IsOk())
	{
		m_treeCtrl_Placed->AddToSelectedNodes(found_node);
	}
	//else
	//{
	//	DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetDisplayName().c_str());
	//}
}

//------------------------------------------------------------------------
//------------------------------------------------------------------------
void cmmPlacedPane::RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	wxTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node.IsOk())
	{
		m_treeCtrl_Placed->RemoveFromSelectedNodes(found_node);
	}
	//else
	//{
	//	DBG_WARNING1("Could not find item: %s in placed list", i_pPickObject->GetDisplayName().c_str());
	//}
}

//--------------------------------------------------------------------
// set enabled state of buttons based on selection
//--------------------------------------------------------------------
void cmmPlacedPane::enable_placed_buttons()
 {
	wxTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	if (sel_node.IsOk())
	{
		shared_ptr<twcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(sel_node);
		if (tree_node)
		{
			cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(tree_node.get());

			// do enable/disable of buttons by passing path to selection
			// to dialog manager and asking which operations are allowed
			//
			if ((pNode->GetNodeType() == cmmSceneTreeNode::e_Object) ||
				(pNode->GetNodeType() == cmmSceneTreeNode::e_Part))
			{
				fsLocator path;
				path.Push(pNode->GetSystemName().c_str());
				path.Push(pNode->GetObjectName().GetString().c_str());
				if (pNode->GetNodeType() == cmmSceneTreeNode::e_Part)
				{
					path.Push(pNode->GetCategoryName().c_str());
					path.Push(pNode->GetPartName().c_str());
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
	}
 }

//--------------------------------------------------------------------
// build tree view of placed objects
//--------------------------------------------------------------------
//void cmmPlacedPane::buildtreeview_system_placed()
//{
//	//	Call the mgr to get all placed objects in the world first
//	//
//	m_DataListPlaced.clear();
//	cmmDialogInterestMgr::GetPlacedObjects(m_DataListPlaced);
//	
//	updatetreeview_system_placed();
//
//	// When first building the placed list, expand all systems
//	gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts);
//
//}

//--------------------------------------------------------------------
// clear and refill tree view with items
//--------------------------------------------------------------------
void cmmPlacedPane::updatetreeview_system_placed()
{
	std::vector< shared_ptr<twcTreeNode> > treeView;
	m_SceneView->CreateTreeView(m_DataListPlaced, treeView);
	m_treeCtrl_Placed->UpdateTreeData(treeView);
}

//--------------------------------------------------------------------
// Check box in tree view changed
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_Checked(twcTreeViewEvent& i_Event)
{
	bool bNewCheckState =  i_Event.GetCheckedState();
	m_SceneView->CheckChanged(i_Event.GetNodes(), bNewCheckState);
	//DBG_LOG("Check changed, num nodes: " << i_Event.GetNodes().size());
	//for (int i=0; i<i_Event.GetNodes().size(); ++i)
	//{
	//	cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_Event.GetNodes()[i].get());
	//	//DBG_LOG("Check changed: " << pNode->m_Name);
	//	if (pNode->GetNodeType() == cmmSceneTreeNode::e_System)
	//	{
	//		cmmSceneOperations::SetSystemVisibility(pNode->GetSystemName(), bNewCheckState);
	//	}
	//	else if (pNode->GetNodeType() == cmmSceneTreeNode::e_Object)
	//	{
	//		//bga - this would be better if it used the nameString
	//		visMgr::SetVisibleInEditor(pNode->GetObjectName().GetString(), bNewCheckState);
	//	}
	//}
}

//--------------------------------------------------------------------
// Selection in tree view changed
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_SelectionChange( twcTreeViewEvent& i_Event)
{
	// Convert from nodes to scene objects
	std::list<sel3dObject*> objects;
	for (int i=0; i<i_Event.GetNodes().size(); ++i)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(i_Event.GetNodes()[i].get());
		sel3dObject* pObject = pNode->GetSceneObject();
		if (pObject)
			objects.push_back( pObject );
	}

	switch (i_Event.GetSelectionType())
	{
	default:
		break;
	case twcTreeViewEvent::e_Select:
		cmmSceneOperations::Select( objects );
		break;
	case twcTreeViewEvent::e_AppendToSelection:
		cmmSceneOperations::AddToSelection( objects );
		break;
	case twcTreeViewEvent::e_RemoveFromSelection:
		cmmSceneOperations::RemoveFromSelection( objects );
		break;
	}
	enable_placed_buttons();
}

//--------------------------------------------------------------------
// Delete key pressed in tree control
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_CharPressed( wxKeyEvent& i_Event )
{
	if (i_Event.GetKeyCode() == WXK_DELETE)
	{
		cmmSceneOperations::DeleteSelected();
	}
	else
		i_Event.Skip();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_Activated( wxTreeEvent& i_Event )
{
	shared_ptr<twcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(i_Event.GetItem());
	if (tree_node)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(tree_node.get());
		if (pNode->GetNodeType() == cmmSceneTreeNode::e_Object)
		{
			cmmDialogInterestMgr::ActivateObject(pNode->GetObjectName(), pNode->GetSystemName());
		}
		else if (pNode->GetNodeType() == cmmSceneTreeNode::e_Part)
		{
			cmmDialogInterestMgr::ActivateObjectPart(pNode->GetObjectName(), pNode->GetSystemName(),
				pNode->GetPartName(), pNode->GetCategoryName() );
		}
	}
}

//--------------------------------------------------------------------
// Drag and Drop handling
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_BeginDrag(wxTreeEvent& i_Event)
{
	shared_ptr<twcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(i_Event.GetItem());
	if (tree_node)
	{
		//DBG_LOG("Begin drag, " << m_treeCtrl_Placed->GetItemText(i_Event.GetItem()).char_str());
		if (m_SceneView->CanDragItem( tree_node ))
			i_Event.Allow();
	}
}
void cmmPlacedPane::treeCtrl_Placed_EndDrag(wxTreeEvent& i_Event)
{
	// If the tree node is NULL, then this could be an operation
	// that moves the selection to the root - so allow this to go to SceneView
	shared_ptr<twcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(i_Event.GetItem());
	//if (tree_node)
	{
		//DBG_LOG("End drag, " << m_treeCtrl_Placed->GetItemText(i_Event.GetItem()).char_str());
		m_SceneView->DragReleased( tree_node );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_ContextMenu( wxTreeEvent& i_Event )
{
	//bga - seems to work better popping up the context menu on right mouse click,
	// something strange happened with the selection code when it was here.
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_RightClick( wxTreeEvent& i_Event )
{
	sel3dObject *pChosenObject = NULL;

	// Make sure chosen item is selected
	wxTreeItemId pick_item = i_Event.GetItem();
	if (m_treeCtrl_Placed->GetSelection() != pick_item)
		m_treeCtrl_Placed->SelectItem(pick_item);

	// Get the chosen object from tree node
	shared_ptr<twcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(pick_item);
	if (tree_node)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(tree_node.get());
		pChosenObject = pNode->GetSceneObject();
	}

	twxContextMenu menu;
	g3dPickInfo *pPickInfo = NULL;	// No 3d pick info in scene manager
	ctxmContextMenu::AddToContextMenu(menu, pChosenObject, pPickInfo);
	if (!menu.IsEmpty())
		PopupMenu(&menu, i_Event.GetPoint());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Placed_OpenAll_Click( wxCommandEvent& i_Event )
{
	gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Placed_CollapseAll_Click( wxCommandEvent& i_Event )
{
	gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Placed_OpenPartial_Click( wxCommandEvent& i_Event )
{
	 wxArrayTreeItemIds selection;
	 gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed);
	int num_selected = m_treeCtrl_Placed->GetSelections(selection);
	for(int i = 0 ; i < num_selected ; i++)
	{
		wxTreeItemId sel_node = selection[i];
		if(sel_node.IsOk())
		{
			if(m_treeCtrl_Placed->ItemHasChildren(sel_node))
			{
				m_treeCtrl_Placed->Expand(sel_node);
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Edit_Click( wxCommandEvent& i_Event )
{
	cmmObjectDialogUtil::Show();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Duplicate_Click( wxCommandEvent& i_Event )
{
	cmmSceneOperations::DuplicateSelected();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Reload_Click( wxCommandEvent& i_Event )
{
	cmmSceneOperations::ReloadSelected();

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Delete_Click( wxCommandEvent& i_Event )
{
	cmmSceneOperations::DeleteSelected();
}



#endif // USE_WXWIDGETS