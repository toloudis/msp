/*****************************************************************************
**	tqcTreeView.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqc/tqcTreeView.hpp"

#include "Core/Env/envSTLHelpers.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include <string>
#include "Core/dbg/dbgMsg.hpp"

#include <algorithm>

#ifdef USE_QT

namespace
{
	// enumeration for the icons in the tree control
	enum CheckboxImageState
	{
		e_NoCheckbox = 0,
		e_Unchecked = 1,
		e_Checked = 2,
		e_MixChecked = 3,
		e_NumCheckboxImageStates
	};

	// Size of the state and item images
	const int c_ImageSize = 13;
	// Space between the images in an item's icon
	const int c_ImageGap = 2;

	//------------------------------------------------------------------------
	// The item's children by their text; the first child with each text,
	// as a search of the children in order would find
	//------------------------------------------------------------------------
	QHash<QString, tqcTreeItemId> children_by_text(QTreeWidgetItem* i_Parent)
	{
		QHash<QString, tqcTreeItemId> children;
		const int count = i_Parent->childCount();
		children.reserve(count);
		for (int i = 0; i < count; ++i)
		{
			QTreeWidgetItem* child = i_Parent->child(i);
			const QString text = child->text(0);
			if (!children.contains(text))
				children.insert(text, child);
		}
		return children;
	}
}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcTreeView::tqcTreeView( QWidget* parent )
: QTreeWidget( parent ),
	m_FirstShift(NULL),
	m_bSelectOnCheckChange(true),
	m_bOurChange(false),
	m_bUpdating(false),
	m_DragItem(NULL),
	m_bDragging(false)
{
	this->setHeaderHidden(true);
	this->setColumnCount(1);
	this->setSelectionMode(QAbstractItemView::ExtendedSelection);
	this->setIconSize(QSize(2 * c_ImageSize + c_ImageGap, c_ImageSize));
	this->setUniformRowHeights(true);

	QObject::connect(this, &QTreeWidget::itemSelectionChanged, [this]() { selection_changed(); });
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcTreeView::~tqcTreeView()
{
	m_bUpdating = true;
	this->clear();
}

//--------------------------------------------------------------------
// Give directory where the tree view should find the icons it needs.
//--------------------------------------------------------------------
void tqcTreeView::LoadImagesFromDirectory( const fsLocator& i_IconDirectory )
{
	std::string icon_dir;
	fsFileUtil::LocatorToUnicodeString( i_IconDirectory, icon_dir );
	QString image_dir = QString::fromStdString(icon_dir);

	std::vector<QPixmap> images;
	images.push_back(QPixmap());	// no checkbox
	images.push_back(QPixmap(image_dir + "\\tree-unchecked.png"));
	images.push_back(QPixmap(image_dir + "\\tree-checked.png"));
	images.push_back(QPixmap(image_dir + "\\tree-mixchecked.png"));
	SetStateImageList(images);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcTreeView::SetStateImageList(const std::vector<QPixmap>& i_Images)
{
	m_StateImages = i_Images;
	m_IconCache.clear();
}
void tqcTreeView::SetImageList(const std::vector<QPixmap>& i_Images)
{
	m_Images = i_Images;
	m_IconCache.clear();
}

//--------------------------------------------------------------------
// By default, clicks on the check box will also act as a selection
// action. To turn off this behavior pass false to this function.
//--------------------------------------------------------------------
void tqcTreeView::SetSelectOnCheckChange(bool i_bSelect)
{
	m_bSelectOnCheckChange = i_bSelect;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void tqcTreeView::Clear()
{
	m_bUpdating  = true;
	this->clear();
	m_NodeData.clear();
	m_ItemState.clear();
	m_ItemImage.clear();
	m_SelectedList.clear();
	m_FirstShift = NULL;
	m_DragItem = NULL;
	m_bUpdating  = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcTreeView::UpdateTreeData(const shared_ptr<tqcTreeNode>& i_RootNode)
{
	std::vector< shared_ptr<tqcTreeNode> > root_nodes;
	root_nodes.push_back(i_RootNode);
	this->UpdateTreeData(root_nodes);
}

//--------------------------------------------------------------------
// Set/Update tree data, this control will maintain a reference
//	to the tree data and will pass the nodes to the callbacks.
//--------------------------------------------------------------------
void tqcTreeView::UpdateTreeData(const std::vector< shared_ptr<tqcTreeNode> >& i_RootNodes)
{
	m_bUpdating  = true;
	this->setUpdatesEnabled(false);

	// Whatever tree nodes we don't confirm should be deleted.
	// List all tree nodes, and collect the ones we confirm.
	TreeItemIdList old_items;
	QTreeWidgetItem* root = this->invisibleRootItem();
	get_all_tree_items(root, old_items);
	TreeItemIdSet items_to_keep;
	items_to_keep.reserve(old_items.size());

	ChildrenByText root_children = children_by_text(root);
	for (size_t i=0; i<i_RootNodes.size(); i++)
	{
		find_or_create_tree_node(items_to_keep, root, root_children, i_RootNodes[i]);
	}
	root->sortChildren(0, Qt::AscendingOrder);

	// Now remove all the old nodes that we did not confirm
	// (children were listed before their parents)
	for (size_t i = 0; i < old_items.size(); ++i)
	{
		if (items_to_keep.find(old_items[i]) == items_to_keep.end())
			delete_item(old_items[i]);
	}

	this->setUpdatesEnabled(true);

	// Clear out interaction cache
	m_FirstShift = NULL;

	// The selection may have lost deleted items
	m_SelectedList.clear();
	QList<QTreeWidgetItem*> selected = this->selectedItems();
	for (int i = 0; i < selected.size(); ++i)
		m_SelectedList.push_back(selected[i]);

	m_bUpdating  = false;
}

//--------------------------------------------------------------------
// Find function finds node that satisfies the given predicate.
//--------------------------------------------------------------------
tqcTreeItemId tqcTreeView::FindNode(NodeFilterFunction i_Function)
{
	return find_child_node(this->invisibleRootItem(), i_Function);
}

//--------------------------------------------------------------------
// Get user data associated with tree item id
//--------------------------------------------------------------------
shared_ptr<tqcTreeNode> tqcTreeView::GetNodeData(tqcTreeItemId i_Node)
{
	std::map<QTreeWidgetItem*, shared_ptr<tqcTreeNode> >::iterator it = m_NodeData.find(i_Node);
	if (it != m_NodeData.end())
		return it->second;
	return shared_ptr<tqcTreeNode>();
}

//--------------------------------------------------------------------
//	Find the item on the placed tree and highlight it.
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//--------------------------------------------------------------------
void tqcTreeView::SelectNode(tqcTreeItemId i_Node)
{
	if (i_Node)
	{
		m_bOurChange = true;
		this->setCurrentItem(i_Node, 0, QItemSelectionModel::ClearAndSelect);
		this->scrollToItem(i_Node);
		m_SelectedList.clear();
		m_SelectedList.push_back(i_Node);
		m_bOurChange = false;
	}
}

//--------------------------------------------------------------------
// Handle multiple selection
//--------------------------------------------------------------------
void tqcTreeView::AddToSelectedNodes(tqcTreeItemId i_Node)
{
	if (i_Node && !i_Node->isSelected())
	{
		m_bOurChange = true;
		i_Node->setSelected(true);
		m_SelectedList.push_back(i_Node);
		m_bOurChange = false;
	}
}
void tqcTreeView::RemoveFromSelectedNodes(tqcTreeItemId i_Node)
{
	if (i_Node && i_Node->isSelected())
	{
		m_bOurChange = true;
		i_Node->setSelected(false);
		envSTLHelpers::RemoveOneValue(m_SelectedList, i_Node);
		m_bOurChange = false;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
tqcTreeItemId tqcTreeView::GetSelection() const
{
	return this->currentItem();
}
void tqcTreeView::SelectItem(tqcTreeItemId i_Node)
{
	if (i_Node)
		this->setCurrentItem(i_Node, 0, QItemSelectionModel::ClearAndSelect);
}

//--------------------------------------------------------------------
// Apply predicate to all nodes in the tree in order to find the node
// in which the predicate returns true, or return NULL.
//--------------------------------------------------------------------
tqcTreeItemId tqcTreeView::find_child_node(QTreeWidgetItem* i_Parent, NodeFilterFunction i_Function)
{
	for (int i = 0; i < i_Parent->childCount(); ++i)
	{
		QTreeWidgetItem* id = i_Parent->child(i);
		shared_ptr<tqcTreeNode> node = GetNodeData(id);
		if (node && i_Function(node))
			return id;

		// recursively look through children
		if (tqcTreeItemId found_node = find_child_node(id, i_Function))
			return found_node;
	}
	return NULL;
}

//--------------------------------------------------------------------
// Item state and image, shown together as the item's icon
//--------------------------------------------------------------------
int tqcTreeView::get_item_state(tqcTreeItemId i_Node)
{
	std::map<QTreeWidgetItem*, int>::iterator it = m_ItemState.find(i_Node);
	return (it != m_ItemState.end()) ? it->second : e_NoCheckbox;
}
void tqcTreeView::set_item_state(tqcTreeItemId i_Node, int i_State)
{
	if (get_item_state(i_Node) != i_State || m_ItemState.find(i_Node) == m_ItemState.end())
	{
		m_ItemState[i_Node] = i_State;
		update_item_icon(i_Node);
	}
}
void tqcTreeView::set_item_image(tqcTreeItemId i_Node, int i_Image)
{
	std::map<QTreeWidgetItem*, int>::iterator it = m_ItemImage.find(i_Node);
	if (it == m_ItemImage.end() || it->second != i_Image)
	{
		m_ItemImage[i_Node] = i_Image;
		update_item_icon(i_Node);
	}
}
void tqcTreeView::update_item_icon(tqcTreeItemId i_Node)
{
	int state = get_item_state(i_Node);
	std::map<QTreeWidgetItem*, int>::iterator it = m_ItemImage.find(i_Node);
	int image = (it != m_ItemImage.end()) ? it->second : -1;

	std::pair<int,int> key(state, image);
	std::map<std::pair<int,int>, QIcon>::iterator cached = m_IconCache.find(key);
	if (cached == m_IconCache.end())
	{
		// state image on the left, item image on the right
		QPixmap composed(2 * c_ImageSize + c_ImageGap, c_ImageSize);
		composed.fill(Qt::transparent);
		QPainter painter(&composed);
		if (state > e_NoCheckbox && state < (int)m_StateImages.size() && !m_StateImages[state].isNull())
			painter.drawPixmap(QRect(0, 0, c_ImageSize, c_ImageSize), m_StateImages[state]);
		if (image >= 0 && image < (int)m_Images.size() && !m_Images[image].isNull())
			painter.drawPixmap(QRect(c_ImageSize + c_ImageGap, 0, c_ImageSize, c_ImageSize), m_Images[image]);
		painter.end();
		cached = m_IconCache.insert(std::make_pair(key, QIcon(composed))).first;
	}
	i_Node->setIcon(0, cached->second);
}

//--------------------------------------------------------------------
// Return current checked state of tree control item.
//--------------------------------------------------------------------
bool tqcTreeView::get_item_checked(tqcTreeItemId i_Node)
{
	return (get_item_state(i_Node) != e_Unchecked);
}
bool tqcTreeView::is_mixed_checked(tqcTreeItemId i_Node)
{
	return (get_item_state(i_Node) == e_MixChecked);
}
void tqcTreeView::set_item_checked(tqcTreeItemId i_Node, bool i_bChecked)
{
	set_item_state(i_Node, (i_bChecked) ? e_Checked : e_Unchecked);
}

//--------------------------------------------------------------------
// Set checked state of tree node based on checked state of children
//--------------------------------------------------------------------
void tqcTreeView::update_parent_checked_state(tqcTreeItemId i_Parent)
{
	bool bAllChecked = true;
	bool bAllUnchecked = true;
	bool bHaveCheckableChildren = false;
	for (int i = 0; i < i_Parent->childCount(); ++i)
	{
		QTreeWidgetItem* id = i_Parent->child(i);
		// Only children showing checkbox states count
		int state = get_item_state(id);
		if ((state != e_NoCheckbox) && (state < e_NumCheckboxImageStates))
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
		if (bAllChecked)
			set_item_state(i_Parent, e_Checked);
		else if (bAllUnchecked)
			set_item_state(i_Parent, e_Unchecked);
		else
			set_item_state(i_Parent, e_MixChecked);
	}
}

//--------------------------------------------------------------------
// Recursively set all children of the given node to the given
// checked state.
//--------------------------------------------------------------------
void tqcTreeView::check_all_children(tqcTreeItemId i_Item, bool i_bNewCheckState)
{
	for (int i = 0; i < i_Item->childCount(); ++i)
	{
		QTreeWidgetItem* id = i_Item->child(i);
		if (is_node_checkable(id))
		{
			set_item_checked(id, i_bNewCheckState);
			check_all_children(id, i_bNewCheckState);
		}
	}
}

//--------------------------------------------------------------------
// Get the ids of all nodes under the given parent node, children
// before their parents.
//--------------------------------------------------------------------
void tqcTreeView::get_all_tree_items(QTreeWidgetItem* i_Parent,
									 TreeItemIdList& o_TreeIds)
{
	for (int i = 0; i < i_Parent->childCount(); ++i)
	{
		QTreeWidgetItem* id = i_Parent->child(i);
		// recurse on children first because we will remove them in that order
		if (id->childCount() > 0)
			get_all_tree_items(id, o_TreeIds);
		o_TreeIds.push_back(id);
	}
}

//--------------------------------------------------------------------
// Create subtree for this node under the given parent
//--------------------------------------------------------------------
void tqcTreeView::find_or_create_tree_node(TreeItemIdSet& io_ItemsToKeep,
										   QTreeWidgetItem* i_Parent,
										   ChildrenByText& io_ParentChildren,
										   const shared_ptr<tqcTreeNode>& i_Node)
{
	QString text_str = i_Node->GetDisplayString();
	tqcTreeItemId node_item = io_ParentChildren.value(text_str, NULL);

	// State index defines checkbox, use "0" for no check box.
	// Image index comes from user, -1 means don't use an image at all.
	int state_index = 0;
	if (i_Node->IsCheckable())
		state_index = (i_Node->IsChecked() ? e_Checked : e_Unchecked);
	else
		state_index = i_Node->GetStateIndex();
	int image_index = i_Node->GetImageIndex();

	if (!node_item)
	{
		// Need to create the node
		node_item = new QTreeWidgetItem(i_Parent, QStringList(text_str));
		io_ParentChildren.insert(text_str, node_item);
	}
	else
	{
		// Found node, reuse it, and keep it from being deleted
		io_ItemsToKeep.insert(node_item);
	}
	m_NodeData[node_item] = i_Node;
	m_ItemImage[node_item] = image_index;
	m_ItemState[node_item] = state_index;
	update_item_icon(node_item);

	if (!i_Node->m_Children.empty())
	{
		ChildrenByText node_children = children_by_text(node_item);
		for (size_t k=0; k<i_Node->m_Children.size(); k++)
		{
			find_or_create_tree_node(io_ItemsToKeep, node_item, node_children, i_Node->m_Children[k]);
		}
	}

	if (i_Node->IsCheckable() && !i_Node->m_Children.empty())
	{
		update_parent_checked_state(node_item);
	}

	if (i_Node->ShouldSortChildren())
		node_item->sortChildren(0, Qt::AscendingOrder);
}

//--------------------------------------------------------------------
// Delete an item that has no children left
//--------------------------------------------------------------------
void tqcTreeView::delete_item(tqcTreeItemId i_Item)
{
	m_NodeData.erase(i_Item);
	m_ItemState.erase(i_Item);
	m_ItemImage.erase(i_Item);
	envSTLHelpers::RemoveOneValue(m_SelectedList, i_Item);
	if (m_FirstShift == i_Item)
		m_FirstShift = NULL;
	if (m_DragItem == i_Item)
		m_DragItem = NULL;
	delete i_Item;
}

//--------------------------------------------------------------------
// Item's check was clicked, set its checked state and adjust
// check state of children and parents.
//--------------------------------------------------------------------
void tqcTreeView::set_checked_state(tqcTreeItemId i_Item, bool i_bNewCheckState)
{
	set_item_checked(i_Item, i_bNewCheckState);

	// Set the state of all children to match this item's checked state
	check_all_children(i_Item, i_bNewCheckState);

	// Set the checked state of all parent nodes (i.e. may be mixed checked now)
	QTreeWidgetItem* parent_item = i_Item->parent();
	while (parent_item)
	{
		update_parent_checked_state(parent_item);
		parent_item = parent_item->parent();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool tqcTreeView::is_node_checkable(tqcTreeItemId i_Item)
{
	shared_ptr<tqcTreeNode> node = GetNodeData(i_Item);
	return node ? node->IsCheckable() : true;	// checkable by default
}

//--------------------------------------------------------------------
// Is the position (in viewport coordinates) on the item's state image?
//--------------------------------------------------------------------
bool tqcTreeView::is_on_state_image(tqcTreeItemId i_Item, const QPoint& i_Pos)
{
	int state = get_item_state(i_Item);
	if (state == e_NoCheckbox)
		return false;

	// the icon is drawn at the start of the item's rectangle
	QRect item_rect = this->visualItemRect(i_Item);
	QStyleOptionViewItem option;
	option.initFrom(this);
	option.rect = item_rect;
	option.decorationSize = this->iconSize();
	option.features |= QStyleOptionViewItem::HasDecoration;
	option.icon = i_Item->icon(0);
	option.text = i_Item->text(0);
	option.features |= QStyleOptionViewItem::HasDisplay;
	QRect icon_rect = this->style()->subElementRect(QStyle::SE_ItemViewItemDecoration, &option, this);
	QRect state_rect(icon_rect.left(), icon_rect.top(), c_ImageSize, icon_rect.height());
	return state_rect.contains(i_Pos);
}

//--------------------------------------------------------------------
// Return nodes between Id1 and Id2. If the nodes do not have the same
// parent, then no nodes are returned.
//--------------------------------------------------------------------
void tqcTreeView::get_items_in_range(tqcTreeItemId i_Id1, tqcTreeItemId i_Id2,
									 std::vector<tqcTreeItemId>& o_NodeList)
{
	if (i_Id1 && i_Id2)
	{
		QTreeWidgetItem* parent_node = i_Id1->parent() ? i_Id1->parent() : this->invisibleRootItem();
		QTreeWidgetItem* parent_node2 = i_Id2->parent() ? i_Id2->parent() : this->invisibleRootItem();
		if (parent_node == parent_node2)
		{
			bool bBetweenNodes = false;
			for (int i = 0; i < parent_node->childCount(); ++i)
			{
				QTreeWidgetItem* id = parent_node->child(i);
				if (id == i_Id1 || id == i_Id2)
				{
					if (bBetweenNodes)
						break;
					else
						bBetweenNodes = true;
				}
				else if (bBetweenNodes)
				{
					o_NodeList.push_back(id);
				}
			}
		}
	}
}

//--------------------------------------------------------------------
// The user changed the selection. Report what changed.
//--------------------------------------------------------------------
void tqcTreeView::selection_changed()
{
	if (m_bOurChange || m_bUpdating)
		return;

	std::vector<tqcTreeItemId> new_selection;
	QList<QTreeWidgetItem*> selected = this->selectedItems();
	for (int i = 0; i < selected.size(); ++i)
	{
		if (GetNodeData(selected[i]))
			new_selection.push_back(selected[i]);
	}

	std::vector<tqcTreeItemId> added, removed;
	for (size_t i = 0; i < new_selection.size(); ++i)
		if (!envSTLHelpers::Contains(m_SelectedList, new_selection[i]))
			added.push_back(new_selection[i]);
	for (size_t i = 0; i < m_SelectedList.size(); ++i)
		if (!envSTLHelpers::Contains(new_selection, m_SelectedList[i]))
			removed.push_back(m_SelectedList[i]);

	m_SelectedList = new_selection;

	if (!m_SelectionCallback || (added.empty() && removed.empty()))
		return;

	const bool bExtending = (QApplication::keyboardModifiers() & (Qt::ControlModifier | Qt::ShiftModifier)) != 0;
	if (!bExtending && new_selection.size() == 1)
	{
		// Select item alone
		tqcTreeViewEvent select_event;
		select_event.SetNode( GetNodeData(new_selection[0]) );
		select_event.SetSelectionType( tqcTreeViewEvent::e_Select );
		m_SelectionCallback( select_event );
		return;
	}

	for (size_t i = 0; i < removed.size(); ++i)
	{
		tqcTreeViewEvent select_event;
		select_event.SetNode( GetNodeData(removed[i]) );
		select_event.SetSelectionType( tqcTreeViewEvent::e_RemoveFromSelection );
		m_SelectionCallback( select_event );
	}
	if (!added.empty())
	{
		std::vector< shared_ptr<tqcTreeNode> > tree_nodes;
		for (size_t i = 0; i < added.size(); ++i)
			tree_nodes.push_back(GetNodeData(added[i]));
		tqcTreeViewEvent select_event;
		select_event.SetNodes( tree_nodes );
		select_event.SetSelectionType( tqcTreeViewEvent::e_AppendToSelection );
		m_SelectionCallback( select_event );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcTreeView::mousePressEvent(QMouseEvent* i_pEvent)
{
	const QPoint pos = i_pEvent->position().toPoint();
	tqcTreeItemId pick_item = this->itemAt(pos);

	if (pick_item && i_pEvent->button() == Qt::LeftButton && is_on_state_image(pick_item, pos))
	{
		const Qt::KeyboardModifiers modifiers = i_pEvent->modifiers();
		const bool bCtrlPressed = (modifiers & Qt::ControlModifier) != 0;
		const bool bShiftPressed = (modifiers & Qt::ShiftModifier) != 0;

		// Get the new check state based on the item that was clicked on
		// and then apply it to all selected items
		bool bNewCheckState = (!get_item_checked(pick_item));
		std::vector<tqcTreeItemId> node_list;
		if (!bCtrlPressed)
		{
			if (pick_item->isSelected() || bShiftPressed)
			{
				// Apply check change to all previously selected nodes.
				node_list = m_SelectedList;
			}

			// If the user was pressing SHIFT when clicking on the checkbox, then
			// include all nodes between the last click and the current item also.
			if (bShiftPressed && m_FirstShift)
			{
				std::vector<tqcTreeItemId> shift_selected;
				get_items_in_range(pick_item, m_FirstShift, shift_selected);
				for (size_t i=0; i<shift_selected.size(); ++i)
				{
					if (!envSTLHelpers::Contains(node_list, shift_selected[i]))
						node_list.push_back(shift_selected[i]);
				}
			}
		}

		// Finally, add in the pick item itself if not already in the list
		if (!envSTLHelpers::Contains(node_list, pick_item))
			node_list.push_back(pick_item);

		// Convert from tree ids to nodes to pass to callback.
		std::vector< shared_ptr<tqcTreeNode> > selected_nodes;
		for (size_t i=0; i<node_list.size(); ++i)
		{
			shared_ptr<tqcTreeNode> node = GetNodeData(node_list[i]);
			if (node)
			{
				selected_nodes.push_back(node);
				if (node->IsCheckable())
					set_checked_state(node_list[i], bNewCheckState);
			}
		}

		m_FirstShift = pick_item;
		bool bAlreadySelected = pick_item->isSelected();

		if (m_CheckedCallback)
		{
			tqcTreeViewEvent check_event;
			check_event.SetNodes( selected_nodes );
			check_event.SetCheckedState( bNewCheckState );
			m_CheckedCallback( check_event );
		}

		// Let the click also select the item, if wanted
		if (m_bSelectOnCheckChange && !bAlreadySelected && !bCtrlPressed && !bShiftPressed)
			QTreeWidget::mousePressEvent(i_pEvent);
		return;
	}

	if (pick_item)
		m_FirstShift = pick_item;

	// Remember a possible start of a drag
	m_bDragging = false;
	m_DragItem = NULL;
	if (pick_item && i_pEvent->button() == Qt::LeftButton && m_CanDrag)
	{
		shared_ptr<tqcTreeNode> node = GetNodeData(pick_item);
		if (node && m_CanDrag(node))
		{
			m_DragItem = pick_item;
			m_DragStart = pos;
		}
	}

	QTreeWidget::mousePressEvent(i_pEvent);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcTreeView::mouseMoveEvent(QMouseEvent* i_pEvent)
{
	if (m_DragItem && (i_pEvent->buttons() & Qt::LeftButton))
	{
		if (!m_bDragging &&
			(i_pEvent->position().toPoint() - m_DragStart).manhattanLength() >= QApplication::startDragDistance())
		{
			m_bDragging = true;
			this->viewport()->setCursor(Qt::DragMoveCursor);
		}
		if (m_bDragging)
			return;	// don't change the selection while dragging
	}
	QTreeWidget::mouseMoveEvent(i_pEvent);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void tqcTreeView::mouseReleaseEvent(QMouseEvent* i_pEvent)
{
	if (m_bDragging)
	{
		m_bDragging = false;
		m_DragItem = NULL;
		this->viewport()->unsetCursor();
		tqcTreeItemId target = this->itemAt(i_pEvent->position().toPoint());
		if (m_DragReleased)
			m_DragReleased(GetNodeData(target));
		return;
	}
	m_DragItem = NULL;
	QTreeWidget::mouseReleaseEvent(i_pEvent);
}

#endif // USE_QT
