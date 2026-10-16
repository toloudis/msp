/*****************************************************************************
**	rdrLayersObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/RenderLayers/qtGUI/rdrLayersObjectsPage.hpp"

#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"

#include <vector>

#ifdef USE_QT

#include <QTreeWidget>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	// Fragment nodes keep their index within the object, because
	// fragment names are not unique when using mesh instancing.
	const int c_NodeIndexRole = Qt::UserRole + 1;

	QTreeWidgetItem* new_item(QTreeWidgetItem* i_pParent, const std::string& i_Name, bool i_bChecked)
	{
		QTreeWidgetItem* pItem = new QTreeWidgetItem(i_pParent, QStringList(tqtQString(i_Name)));
		pItem->setFlags(pItem->flags() | Qt::ItemIsUserCheckable);
		pItem->setCheckState(0, i_bChecked ? Qt::Checked : Qt::Unchecked);
		return pItem;
	}

	//------------------------------------------------------------------------
	// Set checked state of tree node based on checked state of children
	//------------------------------------------------------------------------
	void update_parent_checked_state(QTreeWidgetItem* i_pParent)
	{
		bool bAllChecked = true;
		bool bAllUnchecked = true;
		for (int i = 0; i < i_pParent->childCount(); ++i)
		{
			if (i_pParent->child(i)->checkState(0) != Qt::Unchecked)
				bAllUnchecked = false;
			else
				bAllChecked = false;
		}

		if (bAllChecked)
			i_pParent->setCheckState(0, Qt::Checked);
		else if (bAllUnchecked)
			i_pParent->setCheckState(0, Qt::Unchecked);
		else
			i_pParent->setCheckState(0, Qt::PartiallyChecked);
	}
}	// end of namespace


//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
rdrLayersObjectsPage* rdrLayersObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rdrLayersObjectsPage::rdrLayersObjectsPage( QWidget* parent )
: QWidget( parent ),
  m_bUpdating( false )
{
	QVBoxLayout* bSizer3 = new QVBoxLayout( this );
	m_treeCtrl_Objects = new QTreeWidget( this );
	m_treeCtrl_Objects->setHeaderHidden( true );
	m_treeCtrl_Objects->setColumnCount( 1 );
	m_treeCtrl_Objects->setSelectionMode( QAbstractItemView::ExtendedSelection );
	bSizer3->addWidget( m_treeCtrl_Objects, 1 );

	QObject::connect( m_treeCtrl_Objects, &QTreeWidget::itemChanged,
		[this](QTreeWidgetItem* i_pItem, int) { item_changed(i_pItem); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
rdrLayersObjectsPage::~rdrLayersObjectsPage()
{
	if (rdrLayersObjectsPage::Instance == this)
		rdrLayersObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void rdrLayersObjectsPage::Clear()
{
	//	clear the controls
	m_treeCtrl_Objects->clear();
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

	m_bUpdating = true;
	m_treeCtrl_Objects->setUpdatesEnabled(false);
	m_treeCtrl_Objects->clear();
	QTreeWidgetItem* root = m_treeCtrl_Objects->invisibleRootItem();

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		bool checked = rlyrRenderLayerMgr::GetLayerObjectState(m_RenderLayerName, all_objects[i]);
		QTreeWidgetItem* object_item = new_item(root, all_objects[i].GetString(), checked);

		std::vector<std::string> node_names;
		rlyrRenderLayerMgr::GetFragmentNodeNames(all_objects[i], node_names);

		const int num_nodes = node_names.size();
		for (int n=0; n<num_nodes; n++)
		{
			bool bChildChecked = checked && rlyrRenderLayerMgr::GetLayerFragmentState(m_RenderLayerName, all_objects[i], n);
			QTreeWidgetItem* node_item = new_item(object_item, node_names[n], bChildChecked);
			node_item->setData(0, c_NodeIndexRole, n);
		}

		if (num_nodes > 0)
			update_parent_checked_state(object_item);

		object_item->sortChildren(0, Qt::AscendingOrder);
	}
	root->sortChildren(0, Qt::AscendingOrder);

	m_treeCtrl_Objects->setUpdatesEnabled(true);
	m_bUpdating = false;
}

//--------------------------------------------------------------------
// A check box was toggled
//--------------------------------------------------------------------
void rdrLayersObjectsPage::item_changed(QTreeWidgetItem* i_pItem)
{
	if (m_bUpdating)
		return;

	// Clicking a partially checked object checks all of it
	const bool bNewCheckState = (i_pItem->checkState(0) != Qt::Unchecked);

	QList<QTreeWidgetItem*> items;
	if (i_pItem->isSelected())
		items = m_treeCtrl_Objects->selectedItems();
	else
		items << i_pItem;

	m_bUpdating = true;
	for (QTreeWidgetItem* pItem : items)
		set_checked_state(pItem, bNewCheckState);
	m_bUpdating = false;

	rlyrLayersDocumentChunk::ActiveDataChanged();
}

//--------------------------------------------------------------------
// Turn an object or one of its fragments on or off in the layer
//--------------------------------------------------------------------
void rdrLayersObjectsPage::set_checked_state(QTreeWidgetItem* i_pItem, bool i_bChecked)
{
	i_pItem->setCheckState(0, i_bChecked ? Qt::Checked : Qt::Unchecked);

	QTreeWidgetItem* parent = i_pItem->parent();
	QTreeWidgetItem* object_item = (parent) ? parent : i_pItem;
	nameString objectName(tqtStdString(object_item->text(0)));

	if (parent == NULL)
	{
		// set checked state of child nodes, and of each fragment in the render layer
		for (int i = 0; i < i_pItem->childCount(); ++i)
			i_pItem->child(i)->setCheckState(0, i_bChecked ? Qt::Checked : Qt::Unchecked);

		rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, i_bChecked);
		int num_nodes = rlyrRenderLayerMgr::GetObjectNumNodes(objectName);
		for(int n = 0; n < num_nodes; ++n)
		{
			//make sure each node matches the object's state
			rlyrRenderLayerMgr::SetLayerFragmentState(m_RenderLayerName, objectName, n, i_bChecked);
		}
	}
	else
	{
		// Child node, turn on/off just the fragment
		QVariant data = i_pItem->data(0, c_NodeIndexRole);
		if (data.isValid())
		{
			rlyrRenderLayerMgr::SetLayerFragmentState(m_RenderLayerName, objectName, data.toInt(), i_bChecked);

			//if turning on a fragment, make sure the parent object is turned on
			if(i_bChecked)
			{
				rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, true);
			}

			update_parent_checked_state(object_item);
			if( object_item->checkState(0) == Qt::Unchecked )
			{
				rlyrRenderLayerMgr::SetLayerObjectState(m_RenderLayerName, objectName, false);
			}
		}
	}
}

#endif // USE_QT
