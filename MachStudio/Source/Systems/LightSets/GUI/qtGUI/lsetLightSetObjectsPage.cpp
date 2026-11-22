/*****************************************************************************
**	lsetLightSetObjectsPage.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/LightSets/GUI/qtGUI/lsetLightSetObjectsPage.hpp"

#include "Systems/LightSets/Undo/lsetOperations.hpp"
#include "Systems/LightSets/Object/lsetScriptObject.hpp"
#include "Support/ltst/ltstLightSetMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <algorithm>
#include <vector>

#ifdef USE_QT

#include <QSignalBlocker>
#include <QTreeWidget>
#include <QVBoxLayout>

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

	//--------------------------------------------------------------------
	// Set checked state of tree node based on checked state of children
	//--------------------------------------------------------------------
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
lsetLightSetObjectsPage* lsetLightSetObjectsPage::Instance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObjectsPage::lsetLightSetObjectsPage( QWidget* parent )
: QWidget( parent ),
  m_bUpdating( false )
{
	QVBoxLayout* bSizer3 = new QVBoxLayout( this );
	m_treeCtrl_Objects = new QTreeWidget( this );
	m_treeCtrl_Objects->setHeaderHidden( true );
	m_treeCtrl_Objects->setColumnCount( 1 );
	bSizer3->addWidget( m_treeCtrl_Objects, 1 );

	QObject::connect( m_treeCtrl_Objects, &QTreeWidget::itemChanged,
		[this](QTreeWidgetItem* i_pItem, int) { item_changed(i_pItem); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
lsetLightSetObjectsPage::~lsetLightSetObjectsPage()
{
	if (lsetLightSetObjectsPage::Instance == this)
		lsetLightSetObjectsPage::Instance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void lsetLightSetObjectsPage::Clear()
{
	//	clear the controls
	m_treeCtrl_Objects->clear();
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

	m_bUpdating = true;
	m_treeCtrl_Objects->setUpdatesEnabled(false);
	m_treeCtrl_Objects->clear();
	QTreeWidgetItem* root = m_treeCtrl_Objects->invisibleRootItem();

	const int num_objects = all_objects.size();
	for (int i=0; i<num_objects; i++)
	{
		std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
		bool checked = (it != set_objects.end());
		QTreeWidgetItem* object_item = new_item(root, all_objects[i].GetString(), checked);

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
// An item's checkbox was toggled
//--------------------------------------------------------------------
void lsetLightSetObjectsPage::item_changed(QTreeWidgetItem* i_pItem)
{
	if (m_bUpdating || m_LightSetName.IsEmpty())
		return;

	// Clicking a partially checked object checks all of it
	bool bNewCheckState = (i_pItem->checkState(0) != Qt::Unchecked);

	QTreeWidgetItem* parent = i_pItem->parent();
	bool bRoot = (parent == NULL);
	QTreeWidgetItem* object_item = (bRoot) ? i_pItem : parent;
	nameString objectName(tqtStdString(object_item->text(0)));

	// Updating the children's and the parent's check marks
	// below should not notify again.
	m_bUpdating = true;
	if (bRoot)
	{
		// Root node, turn on/off whole object
		if (bNewCheckState)
			lsetOperations::AddObjectToLightSet(m_LightSetName, objectName);
		else
			lsetOperations::RemoveObjectFromLightSet(m_LightSetName, objectName);

		// set checked state of child nodes, but don't notify
		i_pItem->setCheckState(0, bNewCheckState ? Qt::Checked : Qt::Unchecked);
		for (int i = 0; i < i_pItem->childCount(); ++i)
		{
			i_pItem->child(i)->setCheckState(0, bNewCheckState ? Qt::Checked : Qt::Unchecked);
		}
	}
	else
	{
		// Child node, turn on/off just the fragment
		QVariant data = i_pItem->data(0, c_NodeIndexRole);
		if (data.isValid())
		{
			int node_index = data.toInt();
			if (bNewCheckState)
			{
				lsetOperations::AddNodeToLightSet(m_LightSetName, objectName, node_index);
			}
			else
			{
				lsetOperations::RemoveNodeFromLightSet(m_LightSetName, objectName, node_index);
			}
			update_parent_checked_state(object_item);
		}
	}
	m_bUpdating = false;
}

#endif // USE_QT
