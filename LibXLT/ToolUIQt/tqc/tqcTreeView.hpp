/*****************************************************************************
**	tqcTreeView.hpp
**
**		Tree control showing tqcTreeNode data, with check box images and
**	multiple selection.
**
**	Each item shows a "state" image (unchecked / checked / mixed, or a
**	custom state image) followed by an optional image from the image list.
**	Clicking the state image toggles the check, for all selected items if
**	the clicked item is selected.
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef TQC_TREEVIEW_HPP
#error tqcTreeView.hpp multiply included
#endif
#define TQC_TREEVIEW_HPP

#ifndef TQC_TREENODE_HPP
#include "ToolUIQt/tqc/tqcTreeNode.hpp"
#endif

#include <functional>
#include <list>
#include <map>

#ifdef USE_QT

class fsLocator;

//============================================================================
// Item of the tree control. NULL means "no item" (like !wxTreeItemId::IsOk)
//============================================================================
typedef QTreeWidgetItem* tqcTreeItemId;

//============================================================================
// Information passed to the checked and selection callbacks
//============================================================================
class tqcTreeViewEvent
{
public:
	//--------------------------------------------------------------------
	// Type of selection change
	//--------------------------------------------------------------------
	enum SelectionChangeType
	{
		e_Select = 0,			// select nodes, removing old selection
		e_AppendToSelection,	// add nodes to selection
		e_RemoveFromSelection	// remove nodes from selection
	};

	tqcTreeViewEvent() : m_bCheckedState(false), m_SelectionType(e_Select) {}

	std::vector< shared_ptr<tqcTreeNode> >& GetNodes()
        { return m_Nodes; }
	void SetNodes(const std::vector< shared_ptr<tqcTreeNode> >& i_Nodes)
        { m_Nodes = i_Nodes; }
	void SetNode(shared_ptr<tqcTreeNode> i_Node)
        { m_Nodes.clear(); if (i_Node) m_Nodes.push_back(i_Node); }

	bool GetCheckedState() const { return m_bCheckedState; }
	void SetCheckedState(bool i_bState) { m_bCheckedState = i_bState; }

	SelectionChangeType GetSelectionType() const { return m_SelectionType; }
	void SetSelectionType(SelectionChangeType i_Type) { m_SelectionType = i_Type; }

private:
	std::vector< shared_ptr<tqcTreeNode> >  m_Nodes;
	bool m_bCheckedState;
	SelectionChangeType m_SelectionType;
};

//============================================================================
//============================================================================
class tqcTreeView : public QTreeWidget
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		tqcTreeView( QWidget* parent);
		~tqcTreeView();

		//--------------------------------------------------------------------
		// Callbacks for checking and selecting by the user
		//--------------------------------------------------------------------
		typedef std::function<void (tqcTreeViewEvent&)> TreeViewEventFunction;
		void SetCheckedCallback(TreeViewEventFunction i_Callback) { m_CheckedCallback = i_Callback; }
		void SetSelectionCallback(TreeViewEventFunction i_Callback) { m_SelectionCallback = i_Callback; }

		//--------------------------------------------------------------------
		// Dragging items: CanDrag says whether a node may be dragged, and
		// DragReleased is called with the node under the mouse when the
		// drag ends (NULL when released over no item).
		//--------------------------------------------------------------------
		typedef std::function<bool (const shared_ptr<tqcTreeNode>&)> CanDragFunction;
		typedef std::function<void (const shared_ptr<tqcTreeNode>&)> DragReleasedFunction;
		void SetDragCallbacks(CanDragFunction i_CanDrag, DragReleasedFunction i_Released)
			{ m_CanDrag = i_CanDrag; m_DragReleased = i_Released; }

		//--------------------------------------------------------------------
		// Images: the state images (index 0 none, 1 unchecked, 2 checked,
		// 3 mixed, then custom states) and the item images.
		// LoadImagesFromDirectory loads tree-unchecked.png, tree-checked.png
		// and tree-mixchecked.png as the state images.
		//--------------------------------------------------------------------
		void LoadImagesFromDirectory( const fsLocator& i_IconDirectory );
		void SetStateImageList(const std::vector<QPixmap>& i_Images);
		void SetImageList(const std::vector<QPixmap>& i_Images);

		//--------------------------------------------------------------------
		// By default, clicks on the check box will also act as a selection
		// action. To turn off this behavior pass false to this function.
		//--------------------------------------------------------------------
		void SetSelectOnCheckChange(bool i_bSelect);

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Set/Update tree data, this control will maintain a reference
		//	to the tree data and will pass the nodes to the callbacks.
		//--------------------------------------------------------------------
		void UpdateTreeData(const shared_ptr<tqcTreeNode>& i_RootNode);
		void UpdateTreeData(const std::vector< shared_ptr<tqcTreeNode> >& i_RootNodes);

		//--------------------------------------------------------------------
		// Find function finds node that satisfies the given predicate.
		//--------------------------------------------------------------------
		typedef std::function<bool (const shared_ptr<tqcTreeNode>&)> NodeFilterFunction;
		tqcTreeItemId FindNode(NodeFilterFunction i_Function);

		//--------------------------------------------------------------------
		// Get user data associated with tree item id
		//--------------------------------------------------------------------
		shared_ptr<tqcTreeNode> GetNodeData(tqcTreeItemId i_Node);

		//--------------------------------------------------------------------
		//	Make the item the only selected one, without calling the
		//	selection callback (it reflects a selection made elsewhere).
		//--------------------------------------------------------------------
		void SelectNode(tqcTreeItemId i_Node);

		//--------------------------------------------------------------------
		// Add or remove items from the selection, without callbacks
		//--------------------------------------------------------------------
		void AddToSelectedNodes(tqcTreeItemId i_Node);
		void RemoveFromSelectedNodes(tqcTreeItemId i_Node);

		//--------------------------------------------------------------------
		// The current item, and selecting an item as the user would
		//--------------------------------------------------------------------
		tqcTreeItemId GetSelection() const;
		void SelectItem(tqcTreeItemId i_Node);

	protected:
		//--------------------------------------------------------------------
		// Qt event handlers
		//--------------------------------------------------------------------
		virtual void mousePressEvent(QMouseEvent* i_pEvent);
		virtual void mouseMoveEvent(QMouseEvent* i_pEvent);
		virtual void mouseReleaseEvent(QMouseEvent* i_pEvent);

	private:
		//--------------------------------------------------------------------
		// private functions
		//--------------------------------------------------------------------
		typedef std::list<tqcTreeItemId> TreeItemIdSet;

		tqcTreeItemId find_child_node(QTreeWidgetItem* i_Parent, NodeFilterFunction i_Function);
		int get_item_state(tqcTreeItemId i_Node);
		void set_item_state(tqcTreeItemId i_Node, int i_State);
		void set_item_image(tqcTreeItemId i_Node, int i_Image);
		void update_item_icon(tqcTreeItemId i_Node);
		bool get_item_checked(tqcTreeItemId i_Node);
		bool is_mixed_checked(tqcTreeItemId i_Node);
		void set_item_checked(tqcTreeItemId i_Node, bool i_bChecked);
		void check_all_children(tqcTreeItemId i_Item, bool i_bNewCheckState);
		void get_all_tree_items(QTreeWidgetItem* i_Parent, TreeItemIdSet& o_TreeIds);
		void find_or_create_tree_node(TreeItemIdSet& io_ItemsToDelete,
									  QTreeWidgetItem* i_Parent,
									  const shared_ptr<tqcTreeNode>& i_Node);
		void update_parent_checked_state(tqcTreeItemId i_Parent);
		void set_checked_state(tqcTreeItemId i_Item, bool i_bNewCheckState);
		bool is_node_checkable(tqcTreeItemId i_Item);
		bool is_on_state_image(tqcTreeItemId i_Item, const QPoint& i_Pos);
		void get_items_in_range(tqcTreeItemId i_Id1, tqcTreeItemId i_Id2,
								std::vector<tqcTreeItemId>& o_NodeList);
		void delete_item(tqcTreeItemId i_Item);
		void selection_changed();

		std::map<QTreeWidgetItem*, shared_ptr<tqcTreeNode> > m_NodeData;
		std::map<QTreeWidgetItem*, int> m_ItemState;
		std::map<QTreeWidgetItem*, int> m_ItemImage;
		std::vector<QPixmap> m_StateImages;
		std::vector<QPixmap> m_Images;
		std::map<std::pair<int,int>, QIcon> m_IconCache;

		std::vector<tqcTreeItemId> m_SelectedList;	// last selection reported
		tqcTreeItemId m_FirstShift;
		bool m_bSelectOnCheckChange;
		bool m_bOurChange;
		bool m_bUpdating;

		// dragging
		QPoint m_DragStart;
		tqcTreeItemId m_DragItem;
		bool m_bDragging;

		TreeViewEventFunction m_CheckedCallback;
		TreeViewEventFunction m_SelectionCallback;
		CanDragFunction m_CanDrag;
		DragReleasedFunction m_DragReleased;
};

#endif // USE_QT
