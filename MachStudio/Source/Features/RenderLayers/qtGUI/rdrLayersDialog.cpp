/*****************************************************************************
**	rdrLayersDialog.cpp
**
**		see .hpp
**
\****************************************************************************/
#include "Features/RenderLayers/qtGUI/rdrLayersDialog.hpp"
#include "Features/RenderLayers/qtGUI/rdrLayersObjectsPage.hpp"

#include "Features/RenderLayers/rdrLayersDialogUtil.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/pfx/pfxPostEffectObject.hpp"
#include "Support/rlyr/data/rlyrLayersDocumentChunk.hpp"
#include "Support/rlyr/rlyrPassesObject.hpp"
#include "Support/rlyr/rlyrRenderLayer.hpp"
#include "Support/rlyr/rlyrRenderLayerMgr.hpp"
#include "Support/rprf/rprfPrefsObject.hpp"

#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#include <vector>
#include <string>

#ifdef USE_QT

#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

namespace
{
	// The layer name stored in the item, to rename a layer when its
	// text is edited.
	const int c_NameRole = Qt::UserRole + 1;

	const char* c_RootName = "Render Layers";
	const char* c_MasterName = "Master";

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	std::string get_layer_name(QTreeWidgetItem* i_pItem)
	{
		return tqtStdString(i_pItem->data(0, c_NameRole).toString());
	}

	//------------------------------------------------------------------------
	// Scrolled tab page that a form is built into
	//------------------------------------------------------------------------
	QWidget* add_form_page(QTabWidget* i_pTabs, const char* i_pTitle, QScrollArea** o_ppScroll = NULL)
	{
		QScrollArea* pScroll = new QScrollArea( i_pTabs );
		pScroll->setWidgetResizable( true );
		pScroll->setFrameShape( QFrame::NoFrame );
		pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
		QWidget* pPage = new QWidget();
		pScroll->setWidget( pPage );
		i_pTabs->addTab( pScroll, i_pTitle );
		if (o_ppScroll)
			*o_ppScroll = pScroll;
		return pPage;
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
rdrLayersDialog* rdrLayersDialog::Instance = NULL;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rdrLayersDialog::rdrLayersDialog( QWidget* parent, const std::string& i_Title,
									const std::string& i_Caption )
:	QWidget(parent),
	m_pRoot(NULL),
	m_pTabPage_Pfx(NULL),
	m_pTabPage_Pfx_Sizer(NULL),
	m_pTabPage_Pfx_Shader(NULL),
	m_curSelection(NULL),
	m_CurPage(0),
	m_bUpdating(false)
{
	QHBoxLayout* fgSizer2 = new QHBoxLayout( this );

	QVBoxLayout* sizer_rdrLayer_Tree1 = new QVBoxLayout();
	m_layerTree = new QTreeWidget( this );
	m_layerTree->setHeaderHidden( true );
	m_layerTree->setColumnCount( 1 );
	m_layerTree->setSelectionMode( QAbstractItemView::ExtendedSelection );
	m_layerTree->setEditTriggers( QAbstractItemView::EditKeyPressed | QAbstractItemView::SelectedClicked );
	m_layerTree->setMinimumWidth( 180 );
	m_layerTree->installEventFilter( this );
	sizer_rdrLayer_Tree1->addWidget( m_layerTree, 1 );

	QHBoxLayout* gSizer5 = new QHBoxLayout();
	QPushButton* pAdd = new QPushButton( "Add", this );
	pAdd->setToolTip( "Add New Render Layer" );
	gSizer5->addWidget( pAdd );
	QPushButton* pDuplicate = new QPushButton( "Duplicate", this );
	pDuplicate->setToolTip( "Duplicate Render Layer" );
	gSizer5->addWidget( pDuplicate );
	QPushButton* pDelete = new QPushButton( "Delete", this );
	pDelete->setToolTip( "Delete Render Layer" );
	gSizer5->addWidget( pDelete );
	QPushButton* pRename = new QPushButton( "Rename", this );
	pRename->setToolTip( "Rename Render Layer" );
	gSizer5->addWidget( pRename );
	sizer_rdrLayer_Tree1->addLayout( gSizer5 );
	fgSizer2->addLayout( sizer_rdrLayer_Tree1 );

	QVBoxLayout* bSizer81 = new QVBoxLayout();
	m_layerNameText = new QLabel( "Layer Name", this );
	bSizer81->addWidget( m_layerNameText );
	m_layerTabPages = new QTabWidget( this );
	m_layerTabPages->setUsesScrollButtons( true );
	bSizer81->addWidget( m_layerTabPages, 1 );
	fgSizer2->addLayout( bSizer81, 1 );

#if(SGPU_APP == MS_CORE)
	pDelete->setEnabled( false );
	pDuplicate->setEnabled( false );
	pAdd->setEnabled( false );
	pRename->setEnabled( false );
#endif

	QObject::connect( m_layerTree, &QTreeWidget::itemChanged, [this](QTreeWidgetItem* i_pItem, int) { layerTree_ItemChanged(i_pItem); } );
	QObject::connect( m_layerTree, &QTreeWidget::currentItemChanged, [this](QTreeWidgetItem* i_pItem, QTreeWidgetItem*) { layerTree_CurrentChanged(i_pItem); } );
	QObject::connect( m_layerTabPages, &QTabWidget::currentChanged, [this](int i_Page) { if (!m_bUpdating) m_CurPage = i_Page; } );
	QObject::connect( pAdd, &QPushButton::clicked, [this]() { AddButtonClick(); } );
	QObject::connect( pDuplicate, &QPushButton::clicked, [this]() { DuplicateButtonClick(); } );
	QObject::connect( pDelete, &QPushButton::clicked, [this]() { deleteSelection(); } );
	QObject::connect( pRename, &QPushButton::clicked, [this]() { RenameButtonClick(); } );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
rdrLayersDialog::~rdrLayersDialog()
{
	if (rdrLayersDialog::Instance == this)
		rdrLayersDialog::Instance = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::Update()
{
	UpdateLayerTree();
	UpdateTabPages();

	//select the master layer by default
	if (m_pRoot && m_pRoot->childCount() > 0)
		m_layerTree->setCurrentItem(m_pRoot->child(0));
	m_layerTree->setFocus();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::Draw()
{
	m_layerTabPages->update();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::ResetSelection()
{
	if (m_curSelection)
		m_layerTree->setCurrentItem(m_curSelection);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
QTreeWidgetItem* rdrLayersDialog::add_layer_item(const std::string& i_Name, bool i_bActive)
{
	QTreeWidgetItem* pItem = new QTreeWidgetItem( m_pRoot, QStringList(tqtQString(i_Name)) );
	pItem->setData( 0, c_NameRole, tqtQString(i_Name) );

	Qt::ItemFlags flags = pItem->flags() | Qt::ItemIsUserCheckable;
	if (i_Name == c_MasterName)
	{
		QFont font = pItem->font(0);
		font.setBold(true);
		pItem->setFont(0, font);
	}
	else
	{
		// The master layer cannot be renamed
		flags |= Qt::ItemIsEditable;
	}
	pItem->setFlags( flags );
	pItem->setCheckState( 0, i_bActive ? Qt::Checked : Qt::Unchecked );
	return pItem;
}

//--------------------------------------------------------------------
// The root shows whether all, none or some of the layers are active
//--------------------------------------------------------------------
void rdrLayersDialog::update_root_state()
{
	if (!m_pRoot)
		return;

	bool bAllChecked = true, bAllUnchecked = true;
	for (int i = 0; i < m_pRoot->childCount(); ++i)
	{
		if (m_pRoot->child(i)->checkState(0) == Qt::Checked)
			bAllUnchecked = false;
		else
			bAllChecked = false;
	}

	const bool bWasUpdating = m_bUpdating;
	m_bUpdating = true;
	if (bAllChecked)
		m_pRoot->setCheckState(0, Qt::Checked);
	else if (bAllUnchecked)
		m_pRoot->setCheckState(0, Qt::Unchecked);
	else
		m_pRoot->setCheckState(0, Qt::PartiallyChecked);
	m_bUpdating = bWasUpdating;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateLayerTree()
{
	m_bUpdating = true;
	m_layerTree->clear();
	m_curSelection = NULL;

	m_pRoot = new QTreeWidgetItem( m_layerTree, QStringList(c_RootName) );
	m_pRoot->setFlags( m_pRoot->flags() | Qt::ItemIsUserCheckable );
	m_pRoot->setCheckState( 0, Qt::Checked );

	std::vector<rlyrRenderLayer*> all_layers = rlyrRenderLayerMgr::GetRenderLayers();
	std::vector<rlyrRenderLayer*>::iterator it, end = all_layers.end();
	for(it = all_layers.begin(); it != end; ++it)
	{
		add_layer_item( (*it)->GetName().GetString(), rlyrRenderLayerMgr::GetLayerActive((*it)->GetName()) );
	}
	m_pRoot->setExpanded(true);
	update_root_state();

	m_layerNameText->setText("-");
	m_bUpdating = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateTabPages(rprfPrefsObject* i_RenderPrefs,
									 rlyrPassesObject* i_RenderPasses,
									 pfxPostEffectObject* i_RenderPfx)
{
	int currentPage = m_CurPage;

	//update the tabs, but wait to show update until everything is finished
	m_bUpdating = true;
	m_layerTabPages->setUpdatesEnabled(false);
	while(m_layerTabPages->count() > 0)
	{
		QWidget* pPage = m_layerTabPages->widget(0);
		m_layerTabPages->removeTab(0);
		delete pPage;
	}
	m_pTabPage_Pfx = NULL;
	m_pTabPage_Pfx_Sizer = NULL;
	m_pTabPage_Pfx_Shader = NULL;

	UpdateObjectList();
	UpdateRenderPrefs(i_RenderPrefs);
	UpdatePassesList(i_RenderPasses);
	UpdatePfx(i_RenderPfx);

	m_layerTabPages->setCurrentIndex(currentPage);
	m_layerTabPages->setUpdatesEnabled(true);
	m_bUpdating = false;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdateObjectList()
{
	//	Add the Objects tab page
	rdrLayersObjectsPage::Instance = new rdrLayersObjectsPage(m_layerTabPages);
	nameString cur_layer( (m_curSelection && m_curSelection != m_pRoot) ? get_layer_name(m_curSelection) : std::string() );
	if(!cur_layer.IsEmpty())
	{
		rdrLayersObjectsPage::Instance->Update(cur_layer);
	}
	m_layerTabPages->addTab( rdrLayersObjectsPage::Instance, "Objects" );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::UpdateRenderPrefs(rprfPrefsObject* i_RenderPrefs)
{
	QWidget* pPage = add_form_page( m_layerTabPages, "Render Prefs" );
	if(i_RenderPrefs != NULL)
	{
		pqtFormControlBuilder::BuildForm(pPage, "Render Prefs", (i_RenderPrefs->GetListContainer()), true, false );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::UpdatePassesList(rlyrPassesObject* i_RenderPasses)
{
	QWidget* pPage = add_form_page( m_layerTabPages, "Passes" );
	if(i_RenderPasses != NULL)
	{
		pqtFormControlBuilder::BuildForm(pPage, "Passes", (i_RenderPasses->GetListContainer()), true, false );
	}
}

//--------------------------------------------------------------------
// The post effect page has the base properties and the shader's
//	properties below them; the shader part is rebuilt when the shader
//	changes.
//--------------------------------------------------------------------
void rdrLayersDialog::UpdatePfx(pfxPostEffectObject* i_RenderPfx)
{
	m_pTabPage_Pfx = add_form_page( m_layerTabPages, "Post Effect" );
	m_pTabPage_Pfx_Sizer = new QVBoxLayout( m_pTabPage_Pfx );
	m_pTabPage_Pfx_Sizer->setContentsMargins( 0, 0, 0, 0 );

	QWidget* pBase = new QWidget( m_pTabPage_Pfx );
	m_pTabPage_Pfx_Shader = new QWidget( m_pTabPage_Pfx );
	if(i_RenderPfx != NULL)
	{
		pqtFormControlBuilder::BuildForm(pBase, "Post Effect Base", (*i_RenderPfx->GetBaseContainer().get()), true, false );
		pqtFormControlBuilder::BuildForm(m_pTabPage_Pfx_Shader, "Post Effect Shader", (*i_RenderPfx->GetShaderContainer().get()), true, false );
	}
	m_pTabPage_Pfx_Sizer->addWidget( pBase );
	m_pTabPage_Pfx_Sizer->addWidget( m_pTabPage_Pfx_Shader );
	m_pTabPage_Pfx_Sizer->addStretch( 1 );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void rdrLayersDialog::UpdatePfxShader()
{
	if (!m_curSelection || m_curSelection == m_pRoot || !m_pTabPage_Pfx_Shader)
		return;

	pfxPostEffectObject* pPfxObj = rlyrRenderLayerMgr::GetLayerRenderPfx(nameString(get_layer_name(m_curSelection)));
	if (!pPfxObj)
		return;

	QWidget* new_window = new QWidget(m_pTabPage_Pfx);
	pqtFormControlBuilder::BuildForm(new_window, "Post Effect Shader", (*pPfxObj->GetShaderContainer().get()), true, false );

	m_pTabPage_Pfx_Sizer->replaceWidget(m_pTabPage_Pfx_Shader, new_window);
	delete m_pTabPage_Pfx_Shader;
	m_pTabPage_Pfx_Shader = new_window;
}

//----------------------------------------------------------------------------
// deleteSelection - removes the selected render layers, but never the
//	master layer
//----------------------------------------------------------------------------
void rdrLayersDialog::deleteSelection()
{
	bool bDeleted = false;
	const QList<QTreeWidgetItem*> selection = m_layerTree->selectedItems();
	for (QTreeWidgetItem* pItem : selection)
	{
		if (pItem == m_pRoot)
			continue;

		const std::string name = get_layer_name(pItem);
		if (name != c_MasterName)
		{
			if (pItem == m_curSelection)
				m_curSelection = NULL;
			rlyrRenderLayerMgr::DeleteRenderLayer(nameString(name));
			delete pItem;
			bDeleted = true;
		}
	}

	if (bDeleted)
	{
		rlyrLayersDocumentChunk::ActiveDataChanged();
		update_root_state();
		m_layerTree->setFocus();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
bool rdrLayersDialog::eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
{
	// While renaming, the keys go to the editor rather than the tree
	if (i_pWatched == m_layerTree && i_pEvent->type() == QEvent::KeyPress)
	{
		if (static_cast<QKeyEvent*>(i_pEvent)->key() == Qt::Key_Delete)
		{
			deleteSelection();
			return true;
		}
	}
	return QWidget::eventFilter(i_pWatched, i_pEvent);
}

//----------------------------------------------------------------------------
// A check box was toggled or a layer was renamed
//----------------------------------------------------------------------------
void rdrLayersDialog::layerTree_ItemChanged( QTreeWidgetItem* i_pItem )
{
	if (m_bUpdating)
		return;

	if (i_pItem == m_pRoot)
	{
		// The root turns all of the layers on or off
		const bool bActive = (m_pRoot->checkState(0) != Qt::Unchecked);
		m_bUpdating = true;
		m_pRoot->setCheckState(0, bActive ? Qt::Checked : Qt::Unchecked);
		for (int i = 0; i < m_pRoot->childCount(); ++i)
			m_pRoot->child(i)->setCheckState(0, bActive ? Qt::Checked : Qt::Unchecked);
		m_bUpdating = false;

		rlyrRenderLayerMgr::SetAllLayersActive(bActive);
		rlyrLayersDocumentChunk::ActiveDataChanged();
		return;
	}

	const std::string old_name = get_layer_name(i_pItem);
	const std::string new_name = tqtStdString(i_pItem->text(0));
	if (old_name != new_name)
	{
		// The layer was renamed
		m_bUpdating = true;
		if (rlyrRenderLayerMgr::ChangeLayerName(nameString(old_name), nameString(new_name)))
		{
			i_pItem->setData(0, c_NameRole, tqtQString(new_name));
			if (i_pItem == m_curSelection)
				m_layerNameText->setText(tqtQString(new_name));
			rlyrLayersDocumentChunk::ActiveDataChanged();
		}
		else
		{
			i_pItem->setText(0, tqtQString(old_name));
		}
		m_bUpdating = false;
		return;
	}

	// The active check box was toggled. Toggling a selected layer
	// toggles all of the selected layers.
	const Qt::CheckState state = i_pItem->checkState(0);
	QList<QTreeWidgetItem*> items;
	if (i_pItem->isSelected())
		items = m_layerTree->selectedItems();
	else
		items << i_pItem;

	m_bUpdating = true;
	for (QTreeWidgetItem* pItem : items)
	{
		if (pItem == m_pRoot)
			continue;
		pItem->setCheckState(0, state);
		rlyrRenderLayerMgr::SetLayerActive(nameString(get_layer_name(pItem)), state == Qt::Checked);
	}
	m_bUpdating = false;

	update_root_state();
	rlyrLayersDocumentChunk::ActiveDataChanged();
}

//----------------------------------------------------------------------------
// Show the pages for the current layer
//----------------------------------------------------------------------------
void rdrLayersDialog::layerTree_CurrentChanged( QTreeWidgetItem* i_pItem )
{
	if (m_bUpdating)
		return;

	m_curSelection = i_pItem;

	nameString cur_layer;
	if (i_pItem == NULL || i_pItem == m_pRoot)
	{
		m_layerNameText->setText("-");
	}
	else
	{
		cur_layer = nameString(get_layer_name(i_pItem));
		m_layerNameText->setText(tqtQString(cur_layer.GetString()));
	}

	//once the selection of the render layer has changed, update the corresponding tab pages
	UpdateTabPages(rlyrRenderLayerMgr::GetLayerRenderPrefs(cur_layer),
		rlyrRenderLayerMgr::GetLayerRenderPasses(cur_layer),
		rlyrRenderLayerMgr::GetLayerRenderPfx(cur_layer));

	//update the current layers capture controls
	rlyrRenderLayerMgr::SetupLayerCaptureControls(cur_layer);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::AddButtonClick()
{
	nameString new_layer_name = rlyrRenderLayerMgr::AddRenderLayer();

	m_bUpdating = true;
	QTreeWidgetItem* new_layer = add_layer_item(new_layer_name.GetString(), true);
	m_bUpdating = false;
	rlyrLayersDocumentChunk::ActiveDataChanged();

	//focus on the new layer
	update_root_state();
	m_layerTree->clearSelection();
	m_layerTree->setCurrentItem(new_layer);
	m_layerTree->setFocus();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::DuplicateButtonClick()
{
	QTreeWidgetItem* pCurrent = m_layerTree->currentItem();
	if (pCurrent == NULL || pCurrent == m_pRoot)
		return;

	nameString source(get_layer_name(pCurrent));
	nameString dup_layer_name = rlyrRenderLayerMgr::CopyRenderLayer(source);

	//set the checked image based on the current node's active state
	m_bUpdating = true;
	QTreeWidgetItem* dup_layer = add_layer_item(dup_layer_name.GetString(), rlyrRenderLayerMgr::GetLayerActive(source));
	m_bUpdating = false;
	rlyrLayersDocumentChunk::ActiveDataChanged();

	update_root_state();
	m_layerTree->clearSelection();
	m_layerTree->setCurrentItem(dup_layer);
	m_layerTree->setFocus();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void rdrLayersDialog::RenameButtonClick()
{
	QTreeWidgetItem* pCurrent = m_layerTree->currentItem();
	if (pCurrent && (pCurrent->flags() & Qt::ItemIsEditable))
		m_layerTree->editItem(pCurrent, 0);
}

#endif
