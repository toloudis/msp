/*****************************************************************************
**	twcTreeView.hpp
**
**	Tree view with check boxes in wxWidgets
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef TWC_TREEVIEW_HPP
#error twcTreeView.hpp multiply included
#endif
#define TWC_TREEVIEW_HPP

#ifndef TWC_TREENODE_HPP
#include "ToolUIWx/twc/twcTreeNode.hpp"
#endif 

#include <boost/function.hpp>
#include <list>

#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#include <wx/treectrl.h>

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class fsLocator;

//----------------------------------------------------------------------------
// Class twcTreeView
//----------------------------------------------------------------------------
class twcTreeView : public wxTreeCtrl
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		twcTreeView( wxWindow* parent);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~twcTreeView();

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
		void LoadImagesFromDirectory( const fsLocator& i_IconDirectory );

		//--------------------------------------------------------------------
		// By default, clicks on the check box will also act as a selection
		// action. To turn off this behavior pass false to this function.
		//--------------------------------------------------------------------
		void SetSelectOnCheckChange(bool i_bSelect);

		//--------------------------------------------------------------------
		// Clear tree data 
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Set/Update tree data, this control will maintain a reference
		//	to the tree data and will pass the nodes to the callbacks.
		//--------------------------------------------------------------------	
		void UpdateTreeData(const shared_ptr<twcTreeNode>& i_RootNode);
		void UpdateTreeData(const std::vector< shared_ptr<twcTreeNode> >& i_RootNodes);

		//--------------------------------------------------------------------
		// Find function finds node that satisfies the given predicate
		// or returns a node with IsOk()==false.
		//--------------------------------------------------------------------
		typedef boost::function<bool (const shared_ptr<twcTreeNode>&)> NodeFilterFunction;
		wxTreeItemId FindNode(NodeFilterFunction i_Function);

		//--------------------------------------------------------------------
		// Get user data associated with tree item id
		//--------------------------------------------------------------------
		shared_ptr<twcTreeNode> GetNodeData(wxTreeItemId i_Node);
		
		//--------------------------------------------------------------------
		//	Find the item on the placed tree and highlight it. 
		//	This is a notfication of what is already selected,
		//	so the tree control should not cause an event to trigger.
		//--------------------------------------------------------------------
		void SelectNode(wxTreeItemId i_Node);

		//--------------------------------------------------------------------
		// Handle multiple selection by highlighting other tree nodes
		//--------------------------------------------------------------------
		void AddToSelectedNodes(wxTreeItemId i_Node);
		void RemoveFromSelectedNodes(wxTreeItemId i_Node);

	private:
		//------------------------------------------------------------------------
		// TreeItemIdSet is used to track which nodes are reused and 
		//	which can be deleted
		//------------------------------------------------------------------------
		//typedef std::set<wxTreeItemId> TreeItemIdSet;
		typedef std::list<wxTreeItemIdValue> TreeItemIdSet;

		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		wxTreeItemId find_child_node(wxTreeItemId i_Parent, NodeFilterFunction i_Function);
		bool get_item_checked(wxTreeItemId i_Node);
		bool is_mixed_checked(wxTreeItemId i_Node);
		void set_item_checked(wxTreeItemId i_Node, bool i_bChecked);
		void check_all_children(wxTreeItemId i_Item, bool i_bNewCheckState);
		void get_all_tree_items(wxTreeItemId i_Parent, TreeItemIdSet& o_TreeIds);
		void find_or_create_tree_node(TreeItemIdSet& io_ItemsToDelete,
									   wxTreeItemId i_Parent,
									   const shared_ptr<twcTreeNode>& i_Node,
									   bool i_bCreateAlways = false);
		void create_tree_node(wxTreeItemId i_Parent, const shared_ptr<twcTreeNode>& i_Node);
		int get_item_index(wxTreeItemId i_Node);
		void update_parent_checked_state(wxTreeItemId i_Parent);
		void set_checked_state(wxTreeItemId i_Item, bool i_bNewCheckState);
		void highlight_item(wxTreeItemId i_ChildItem, bool i_bHighlight);
		void remove_all_highlight();
		void clear_selection();
		void update_highlights();
		bool is_node_selected(wxTreeItemId i_Item);
		bool is_node_checkable(wxTreeItemId i_Item);
		shared_ptr<twcTreeNode> get_tree_node(wxTreeItemId i_Node);
		void select_item(wxTreeItemId i_Item, bool i_bSendEvent = true);
		void toggle_selection(wxTreeItemId i_Item, bool i_bSendEvent = true);
		void append_selection(const std::vector<wxTreeItemId>& i_NodeList, bool i_bSendEvent = true);
		void get_items_in_range(wxTreeItemId i_Id1, wxTreeItemId i_Id2,
								std::vector<wxTreeItemId>& o_NodeList);

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void treeCtrl_Selection(wxTreeEvent& i_Event);
		virtual void treeCtrl_LeftMouseDown( wxMouseEvent& i_Event);
		virtual void treeCtrl_LeftMouseUp( wxMouseEvent& i_Event);
		//virtual void OnSetFocus(wxFocusEvent& i_Event);
		//virtual void OnLostFocus(wxFocusEvent& i_Event);

		//std::vector< shared_ptr<twcTreeNode> > m_RootNodes;
		std::vector<wxTreeItemId> m_SelectedList;
		bool m_bCtrlPressed, m_bShiftPressed, m_bAltPressed;
		wxTreeItemId m_FirstShift;
		bool m_bSelectOnCheckChange;
		//bool m_bHasFocus;
		bool m_bOurChange;
		bool m_bUpdating;

		// Our internal events
		DECLARE_EVENT_TABLE()
};


//----------------------------------------------------------------------------
// Event class used for check state or selection changes in tree view,
// passes the twcTreeNodes to the callback.
//----------------------------------------------------------------------------
class twcTreeViewEvent: public wxCommandEvent
{
public:
	//--------------------------------------------------------------------
	// Type of selection change being reported by this event
	//--------------------------------------------------------------------
	enum SelectionChangeType
	{
		e_Select = 0,		
		e_AppendToSelection,
		e_RemoveFromSelection
	};

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
    twcTreeViewEvent( wxEventType i_CommandType = wxEVT_NULL, int i_Id = 0);

	//--------------------------------------------------------------------
    // List of nodes that this event applies to
	//--------------------------------------------------------------------
	std::vector< shared_ptr<twcTreeNode> >& GetNodes()
        { return m_Nodes; }
	void SetNodes(const std::vector< shared_ptr<twcTreeNode> >& i_Nodes)
        { m_Nodes = i_Nodes; }
	void SetNode(shared_ptr<twcTreeNode> i_Node)
        { m_Nodes.clear(); m_Nodes.push_back(i_Node); }

	//--------------------------------------------------------------------
	// Checked state for when event is a check box change
	//--------------------------------------------------------------------
	bool GetCheckedState() const { return m_bCheckedState; }
	void SetCheckedState(bool i_bState) { m_bCheckedState = i_bState; }

	//--------------------------------------------------------------------
	// Selection type for when event is a selection change
	//--------------------------------------------------------------------
	SelectionChangeType GetSelectionType() const { return m_SelectionType; }
	void SetSelectionType(SelectionChangeType i_Type) { m_SelectionType = i_Type; }

	//--------------------------------------------------------------------
    // required for sending with wxPostEvent()
	//--------------------------------------------------------------------
    wxEvent* Clone();

private:
	std::vector< shared_ptr<twcTreeNode> >  m_Nodes;
	bool m_bCheckedState;
	SelectionChangeType m_SelectionType;
};

DECLARE_LOCAL_EVENT_TYPE( wxEVT_CHECKED_TREE_VIEW, -1 )
DECLARE_LOCAL_EVENT_TYPE( wxEVT_SELECTION_TREE_VIEW, -1 )


typedef void (wxEvtHandler::*twcTreeViewEventFunction)(twcTreeViewEvent&);

#define EVT_TWC_TREE_VIEW_FUNC(fn) \
    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)  \
    wxStaticCastEvent( twcTreeViewEventFunction, & fn )

//#define EVT_CHECKED_TREE_VIEW(id, fn) \
//    DECLARE_EVENT_TABLE_ENTRY( wxEVT_CHECKED_TREE_VIEW_ACTION, id, -1, \
//    (wxObjectEventFunction) (wxEventFunction) (wxCommandEventFunction)  \
//    wxStaticCastEvent( twcTreeViewEventFunction, & fn ), (wxObject *) NULL ),



#endif // USE_WXWIDGETS
