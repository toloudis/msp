/*****************************************************************************
**	ResourceTrackerDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/ResourceTracker/qtGUI/ResourceTrackerDialog.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envString.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#include "Core/it/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#include <stack>

#ifdef USE_QT

#include <QPushButton>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace
{
	QString to_qstring(const fsLocator& i_Locator)
	{
		std::string str;
		fsFileUtil::LocatorToUnicodeString(i_Locator, str);
		return QString::fromStdString(str);
	}

	QTreeWidget* create_tree(QWidget* i_pParent)
	{
		QTreeWidget* pTree = new QTreeWidget(i_pParent);
		pTree->setHeaderHidden(true);
		pTree->setColumnCount(1);
		return pTree;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the dialog, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
ResourceTrackerDialog* ResourceTrackerDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
ResourceTrackerDialog::ResourceTrackerDialog( QWidget* parent )
:	QWidget(parent)
{
	QVBoxLayout* bSizer_resources = new QVBoxLayout( this );

	QTabWidget* pNotebook = new QTabWidget( this );
	m_treeCtrl_resources = create_tree( pNotebook );
	pNotebook->addTab( m_treeCtrl_resources, "Resources" );
	m_treeCtrl_hierarchy = create_tree( pNotebook );
	pNotebook->addTab( m_treeCtrl_hierarchy, "Hierarchy" );
	bSizer_resources->addWidget( pNotebook, 1 );

	QPushButton* pRefresh = new QPushButton( "Refresh", this );
	bSizer_resources->addWidget( pRefresh, 0, Qt::AlignHCenter );
	QObject::connect( pRefresh, &QPushButton::clicked, [this]() { Update(); } );

	ResourceTrackerDialog::Instance = this;

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name("Resource Tracker").Caption("Resource Tracker").Show().Layer(1).Left().BestSize(500, 300));

	Update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ResourceTrackerDialog::~ResourceTrackerDialog()
{
	ResourceTrackerDialog::Instance = NULL;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ResourceTrackerDialog::Update()
{
	fsResourceTrackerData resource_tracker_data;
	docSingleDocumentMgr::GetResourceList(resource_tracker_data);

	update_tree_resources(resource_tracker_data);
	update_tree_hierarchy(resource_tracker_data);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ResourceTrackerDialog::update_tree_resources(const fsResourceTrackerData& i_ResourceTrackerData)
{
	std::set<fsLocator> file_paths;
	for (int j = 0; j < i_ResourceTrackerData.m_Resources.size(); ++j)
	{
		fsLocator filepath;
		filepath = i_ResourceTrackerData.m_Resources[j].GetFilePath();
		file_paths.insert(filepath);
	}

	m_treeCtrl_resources->clear();
	QTreeWidgetItem* pRoot = new QTreeWidgetItem( m_treeCtrl_resources, QStringList(to_qstring(docSingleDocumentMgr::GetFilename())) );

	std::set<fsLocator>::const_iterator it;
	for (it = file_paths.begin(); it != file_paths.end(); ++it)
	{
		if ((*it).GetNumNames() > 0)
			new QTreeWidgetItem( pRoot, QStringList(to_qstring(*it)) );
	}

	//	expand the tree
	m_treeCtrl_resources->expandAll();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ResourceTrackerDialog::update_tree_hierarchy(const fsResourceTrackerData& i_ResourceTrackerData)
{
	std::stack<QTreeWidgetItem*> parents;

	m_treeCtrl_hierarchy->clear();
	QTreeWidgetItem* pRoot = new QTreeWidgetItem( m_treeCtrl_hierarchy, QStringList(to_qstring(docSingleDocumentMgr::GetFilename())) );

	QTreeWidgetItem* node_id = NULL;
	int last_depth = -1;
	parents.push(pRoot);	// root of the tree
	for (int i = 0; i < i_ResourceTrackerData.m_Resources.size(); ++i)
	{
		//	get the depth and determine the parent
		int depth = i_ResourceTrackerData.m_Resources[i].GetDepth();
		if (depth == last_depth)
		{
			parents.pop();
		}
		else if (depth < last_depth)
		{
			int diff = last_depth - depth;
			for (int j=0; j <= diff; ++j)
			{
				parents.pop();
			}
		}

		//	add the item to the tree
		QString text = to_qstring(i_ResourceTrackerData.m_Resources[i].GetFilePath());
		if (text.isEmpty())
		{
			text = tqtQString(i_ResourceTrackerData.m_Resources[i].GetName().GetString());
		}
		node_id = new QTreeWidgetItem( parents.top(), QStringList(text) );

		//	keep track of the node id + depth
		parents.push(node_id);
		last_depth = depth;
	}

	//	collapse the tree, leaving the scene's files showing
	m_treeCtrl_hierarchy->collapseAll();
	pRoot->setExpanded(true);
}

#endif
