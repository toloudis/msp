/*****************************************************************************
**	cmmPlacedPane.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/qtGUI/cmmPlacedPane.hpp"

#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Systems/Common/GUI/cmmSceneOperations.hpp"
#include "Systems/Common/GUI/qtGUI/cmmSceneViewCategory.hpp"
#include "Systems/Common/GUI/qtGUI/cmmSceneViewGroup.hpp"
#include "Systems/Common/GUI/qtGUI/cmmSceneViewLights.hpp"
#include "Systems/Common/GUI/qtGUI/cmmPlacedImages.hpp"
#include "Systems/Common/GUI/qtGUI/cmmSceneTreeNode.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/ctxm/ctxmContextMenu.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dObject.hpp"
#include "ToolUIQt/tqt/tqtContextMenu.hpp"
#include "ToolUIQt/tqc/tqcTreeView.hpp"

#ifdef USE_QT

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QToolButton>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	const bool c_bExpandObjectParts = false; // Should object parts be expanded by default?

	//--------------------------------------------------------------------
	// Icon directory, with a trailing separator
	//--------------------------------------------------------------------
	QString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return QString::fromWCharArray(wide_dir.GetString()) + "\\";
	}

	struct pick_object_filter
	{
		sel3dObject* m_pPickObject;
		pick_object_filter(sel3dObject* i_pPickObject) : m_pPickObject(i_pPickObject) {}
		bool operator()(const shared_ptr<tqcTreeNode>& i_Node)
		{
			cmmSceneTreeNode* pNode  = static_cast<cmmSceneTreeNode*>(i_Node.get());
			return (m_pPickObject == pNode->GetSceneObject());
		}
	};

	//--------------------------------------------------------------------
	// Small text button for the expand/collapse bar
	//--------------------------------------------------------------------
	QToolButton* create_text_button(QWidget* i_pParent, const char* i_pText, const char* i_pToolTip)
	{
		QToolButton* pButton = new QToolButton(i_pParent);
		pButton->setText(i_pText);
		pButton->setToolTip(i_pToolTip);
		pButton->setFixedSize(18, 18);
		return pButton;
	}

	//--------------------------------------------------------------------
	// Image button with normal, disabled, hover and pressed images
	//--------------------------------------------------------------------
	QToolButton* create_image_button(QWidget* i_pParent, const QString& i_IconDir,
									 const char* i_pName, const char* i_pToolTip)
	{
		const QString base = i_IconDir + i_pName;
		QIcon icon(base + ".PNG");
		icon.addPixmap(QPixmap(base + "_disabled.PNG"), QIcon::Disabled);
		icon.addPixmap(QPixmap(base + "_highlight.PNG"), QIcon::Active);
		icon.addPixmap(QPixmap(base + "_pressed.PNG"), QIcon::Selected);

		QToolButton* pButton = new QToolButton(i_pParent);
		pButton->setIcon(icon);
		pButton->setIconSize(QSize(30, 30));
		pButton->setAutoRaise(true);
		pButton->setToolTip(i_pToolTip);
		return pButton;
	}
}	// end of namespace

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmPlacedPane::cmmPlacedPane( QWidget* parent, ViewType i_Type )
:	QWidget( parent )
{
	const QString icon_dir = get_icon_directory();

	QVBoxLayout* sizer_Placed = new QVBoxLayout( this );
	sizer_Placed->setContentsMargins( 0, 0, 0, 0 );

	m_treeCtrl_Placed = new tqcTreeView( this );
	sizer_Placed->addWidget( m_treeCtrl_Placed, 3 );

	QHBoxLayout* sizer_Placed_ButtonBar = new QHBoxLayout();

	QToolButton* pOpenAll = create_text_button( this, "+", "Expand the List" );
	sizer_Placed_ButtonBar->addWidget( pOpenAll );
	QToolButton* pCollapseAll = create_text_button( this, "-", "Contract the List" );
	sizer_Placed_ButtonBar->addWidget( pCollapseAll );
	QToolButton* pOpenPartial = create_text_button( this, ":", "Expand only the selected item" );
	sizer_Placed_ButtonBar->addWidget( pOpenPartial );
	sizer_Placed_ButtonBar->addStretch( 1 );

	m_bpButton_Edit = create_image_button( this, icon_dir, "placed-edit", "Edit properties of selected objects" );
	sizer_Placed_ButtonBar->addWidget( m_bpButton_Edit );
	m_bpButton_Duplicate = create_image_button( this, icon_dir, "placed-duplicate", "Duplicate selected object" );
	sizer_Placed_ButtonBar->addWidget( m_bpButton_Duplicate );
	m_bpButton_Reload = create_image_button( this, icon_dir, "placed-reload", "Reload selected object" );
	sizer_Placed_ButtonBar->addWidget( m_bpButton_Reload );
	m_bpButton_Delete = create_image_button( this, icon_dir, "placed-delete", "Delete selected object" );
	sizer_Placed_ButtonBar->addWidget( m_bpButton_Delete );

	sizer_Placed->addLayout( sizer_Placed_ButtonBar );

	// Create scene view that will determine how the placed objects are displayed
	// in our tree view
	if (i_Type == e_GroupView)
		m_SceneView.reset( new cmmSceneViewGroup() );
	else if (i_Type == e_LightsView)
		m_SceneView.reset( new cmmSceneViewLights() );
	else
		m_SceneView.reset( new cmmSceneViewCategory() );

	// Create state images to represent checked state
	std::vector<QPixmap> check_images;
	if (i_Type == e_LightsView)
	{
		check_images.push_back(QPixmap(icon_dir + "tree-disabled.png")); // duplicate for the "no checkbox" state
		check_images.push_back(QPixmap(icon_dir + "tree-disabled.png"));
		check_images.push_back(QPixmap(icon_dir + "tree-enabled.png"));
		check_images.push_back(QPixmap(icon_dir + "tree-mixenabled.png"));
	}
	else
	{
		check_images.push_back(QPixmap(icon_dir + "tree-unchecked.png")); // duplicate for the "no checkbox" state
		check_images.push_back(QPixmap(icon_dir + "tree-unchecked.png"));
		check_images.push_back(QPixmap(icon_dir + "tree-checked.png"));
		check_images.push_back(QPixmap(icon_dir + "tree-mixchecked.png"));
	}
	check_images.push_back(QPixmap(icon_dir + "tree-controlpart.png"));
	check_images.push_back(QPixmap(icon_dir + "tree-materialpart.png"));
	check_images.push_back(QPixmap(icon_dir + "tree-surfacepart.png"));
	m_treeCtrl_Placed->SetStateImageList(check_images);

	// Event callbacks
	m_treeCtrl_Placed->SetCheckedCallback( [this](tqcTreeViewEvent& i_Event) { treeCtrl_Placed_Checked(i_Event); } );
	m_treeCtrl_Placed->SetSelectionCallback( [this](tqcTreeViewEvent& i_Event) { treeCtrl_Placed_SelectionChange(i_Event); } );
	m_treeCtrl_Placed->SetDragCallbacks(
		[this](const shared_ptr<tqcTreeNode>& i_Node) { return m_SceneView->CanDragItem(i_Node); },
		[this](const shared_ptr<tqcTreeNode>& i_Node) { m_SceneView->DragReleased(i_Node); } );
	QObject::connect( m_treeCtrl_Placed, &QTreeWidget::itemActivated,
		[this](QTreeWidgetItem* i_pItem, int) { treeCtrl_Placed_Activated(i_pItem); } );
	m_treeCtrl_Placed->setContextMenuPolicy( Qt::CustomContextMenu );
	QObject::connect( m_treeCtrl_Placed, &QWidget::customContextMenuRequested,
		[this](const QPoint& i_Pos) { treeCtrl_Placed_ContextMenu(i_Pos); } );
	m_treeCtrl_Placed->installEventFilter( this );

	QObject::connect( pOpenAll, &QToolButton::clicked, [this]() { gsupTreeCtrlUtil::ExpandRoot(m_treeCtrl_Placed, c_bExpandObjectParts); } );
	QObject::connect( pCollapseAll, &QToolButton::clicked, [this]() { gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed); } );
	QObject::connect( pOpenPartial, &QToolButton::clicked, [this]() { button_Placed_OpenPartial_Click(); } );
	QObject::connect( m_bpButton_Edit, &QToolButton::clicked, []() { cmmObjectDialogUtil::Show(); } );
	QObject::connect( m_bpButton_Duplicate, &QToolButton::clicked, []() { cmmSceneOperations::DuplicateSelected(); } );
	QObject::connect( m_bpButton_Reload, &QToolButton::clicked, []() { cmmSceneOperations::ReloadSelected(); } );
	QObject::connect( m_bpButton_Delete, &QToolButton::clicked, []() { cmmSceneOperations::DeleteSelected(); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmPlacedPane::~cmmPlacedPane()
{
}

//--------------------------------------------------------------------
// Assign image list to tree control
//--------------------------------------------------------------------
void cmmPlacedPane::AssignImageList(const std::vector<QPixmap>& i_ImageList)
{
	m_treeCtrl_Placed->SetImageList(i_ImageList);
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
}

//--------------------------------------------------------------------
// Update scene data in form
//--------------------------------------------------------------------
void cmmPlacedPane::UpdateDialog()
{
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
	// Don't update when loading
	if (!docSingleDocumentMgr::IsLoading())
	{
		cmmDialogInterestMgr::UpdateList(i_SystemName, i_DataList, m_DataListPlaced);
		updatetreeview_system_placed();
	}
}

//--------------------------------------------------------------------
//	Find the item on the placed tree an highlight it.
//	This is a notfication of what is already selected,
//	so the tree control should not cause an event to trigger.
//--------------------------------------------------------------------
void cmmPlacedPane::SelectObjectOnPlacedList(sel3dObject* i_pPickObject)
{
	tqcTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node)
	{
		m_treeCtrl_Placed->SelectNode(found_node);
	}
	else
	{
		// Because of the multiple views (tabs) into the scene manager, it sometimes
		// happens that an object does not exists in a certain view - so we need to
		// clear the selection in this case.
		m_treeCtrl_Placed->clearSelection();
	}
}

void cmmPlacedPane::AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	tqcTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node)
	{
		m_treeCtrl_Placed->AddToSelectedNodes(found_node);
	}
}

void cmmPlacedPane::RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject)
{
	tqcTreeItemId found_node = m_treeCtrl_Placed->FindNode(pick_object_filter(i_pPickObject));
	if (found_node)
	{
		m_treeCtrl_Placed->RemoveFromSelectedNodes(found_node);
	}
}

//--------------------------------------------------------------------
// set enabled state of buttons based on selection
//--------------------------------------------------------------------
void cmmPlacedPane::enable_placed_buttons()
{
	bool bEdit = false, bDelete = false, bDuplicate = false, bReload = false;

	tqcTreeItemId sel_node = m_treeCtrl_Placed->GetSelection();
	shared_ptr<tqcTreeNode> tree_node;
	if (sel_node)
		tree_node = m_treeCtrl_Placed->GetNodeData(sel_node);
	if (tree_node)
	{
		cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(tree_node.get());

		// do enable/disable of buttons by passing path to selection
		// to dialog manager and asking which operations are allowed
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
			bEdit = (interactions & e_DIAllow_Edit) != 0;
			bDelete = (interactions & e_DIAllow_Del) != 0;
			bDuplicate = (interactions & e_DIAllow_Dupe) != 0;
			bReload = (interactions & e_DIAllow_Reload) != 0;
		}
	}

	m_bpButton_Edit->setEnabled( bEdit );
	m_bpButton_Delete->setEnabled( bDelete );
	m_bpButton_Duplicate->setEnabled( bDuplicate );
	m_bpButton_Reload->setEnabled( bReload );
}

//--------------------------------------------------------------------
// clear and refill tree view with items
//--------------------------------------------------------------------
void cmmPlacedPane::updatetreeview_system_placed()
{
	std::vector< shared_ptr<tqcTreeNode> > treeView;
	m_SceneView->CreateTreeView(m_DataListPlaced, treeView);
	m_treeCtrl_Placed->UpdateTreeData(treeView);
}

//--------------------------------------------------------------------
// Check box in tree view changed
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_Checked(tqcTreeViewEvent& i_Event)
{
	bool bNewCheckState =  i_Event.GetCheckedState();
	m_SceneView->CheckChanged(i_Event.GetNodes(), bNewCheckState);
}

//--------------------------------------------------------------------
// Selection in tree view changed
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_SelectionChange( tqcTreeViewEvent& i_Event)
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
	case tqcTreeViewEvent::e_Select:
		cmmSceneOperations::Select( objects );
		break;
	case tqcTreeViewEvent::e_AppendToSelection:
		cmmSceneOperations::AddToSelection( objects );
		break;
	case tqcTreeViewEvent::e_RemoveFromSelection:
		cmmSceneOperations::RemoveFromSelection( objects );
		break;
	}

	enable_placed_buttons();
}

//--------------------------------------------------------------------
// Delete key pressed in tree control
//--------------------------------------------------------------------
bool cmmPlacedPane::eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
{
	if (i_pWatched == m_treeCtrl_Placed && i_pEvent->type() == QEvent::KeyPress)
	{
		if (static_cast<QKeyEvent*>(i_pEvent)->key() == Qt::Key_Delete)
		{
			cmmSceneOperations::DeleteSelected();
			return true;
		}
	}
	return QWidget::eventFilter(i_pWatched, i_pEvent);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_Activated( QTreeWidgetItem* i_pItem )
{
	shared_ptr<tqcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(i_pItem);
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
//--------------------------------------------------------------------
void cmmPlacedPane::treeCtrl_Placed_ContextMenu( const QPoint& i_Pos )
{
	sel3dObject *pChosenObject = NULL;

	// Make sure chosen item is selected
	tqcTreeItemId pick_item = m_treeCtrl_Placed->itemAt(i_Pos);
	if (pick_item)
	{
		if (m_treeCtrl_Placed->GetSelection() != pick_item)
			m_treeCtrl_Placed->SelectItem(pick_item);

		// Get the chosen object from tree node
		shared_ptr<tqcTreeNode> tree_node = m_treeCtrl_Placed->GetNodeData(pick_item);
		if (tree_node)
		{
			cmmSceneTreeNode *pNode = static_cast<cmmSceneTreeNode*>(tree_node.get());
			pChosenObject = pNode->GetSceneObject();
		}
	}

	tqtContextMenu menu;
	g3dPickInfo *pPickInfo = NULL;	// No 3d pick info in scene manager
	ctxmContextMenu::AddToContextMenu(menu, pChosenObject, pPickInfo);
	if (!menu.IsEmpty())
		menu.Popup(m_treeCtrl_Placed->viewport(), i_Pos);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void cmmPlacedPane::button_Placed_OpenPartial_Click()
{
	const QList<QTreeWidgetItem*> selection = m_treeCtrl_Placed->selectedItems();
	gsupTreeCtrlUtil::CollapseAll(m_treeCtrl_Placed);
	for (QTreeWidgetItem* sel_node : selection)
	{
		if (sel_node->childCount() > 0)
			sel_node->setExpanded(true);
	}
}

#endif // USE_QT
