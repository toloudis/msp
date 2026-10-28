/*****************************************************************************
**	ImportDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Import/qtGUI/ImportDialog.hpp"

#include "Features/Import/ImportDialogUtil.hpp"
#include "Features/Import/ImportUtil.hpp"
#include "Features/Import/imptSceneAppend.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "ImportExport/iscn/iscnSceneChoice.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "ToolUIQt/tqc/tqcFilePicker.hpp"

#include <set>

#ifdef USE_QT

#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QTreeWidget>
#include <QVBoxLayout>

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ImportDialog::ImportDialog( QWidget* parent, ImportData& i_Data )
:	QDialog( parent ),
	m_Data(i_Data)
{
	this->setWindowTitle( "Import" );

	QVBoxLayout* bSizer1 = new QVBoxLayout( this );

	QGroupBox* pBox = new QGroupBox( "Root Scene/Shot file", this );
	QVBoxLayout* sbSizer2 = new QVBoxLayout( pBox );
	m_filePicker1 = new tqcFilePicker( pBox );
	m_filePicker1->SetFilter( itString(ImportDialogUtil::FileFilter().c_str()) );
	sbSizer2->addWidget( m_filePicker1 );
	m_checkBox_AllowDupes = new QCheckBox( "Import Duplicates by Renaming", pBox );
	m_checkBox_AllowDupes->setEnabled( false );
	sbSizer2->addWidget( m_checkBox_AllowDupes );
	bSizer1->addWidget( pBox );

	bSizer1->addWidget( new QLabel( "Root Scene/Shot Inventory List", this ) );

	// Chunks are checked with tristate parents, the items under them
	// are checked individually.
	m_treeCtrl_Import = new QTreeWidget( this );
	m_treeCtrl_Import->setHeaderHidden( true );
	m_treeCtrl_Import->setColumnCount( 1 );
	bSizer1->addWidget( m_treeCtrl_Import, 1 );

	QPushButton* pImport = new QPushButton( "Import", this );
	pImport->setAutoDefault( false );
	bSizer1->addWidget( pImport, 0, Qt::AlignHCenter );

	this->resize( 452, 411 );

	m_filePicker1->SetValueChangedCallback( [this]() { filePicker_FileChanged(); } );
	QObject::connect( pImport, &QPushButton::clicked, [this]() { buttonImport_Click(); } );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ImportDialog::~ImportDialog()
{
	clear_file();
}

//--------------------------------------------------------------------
// Forget the previously picked file
//--------------------------------------------------------------------
void ImportDialog::clear_file()
{
	m_Data.m_Chunks.clear();
	m_Scene = iscnSceneDesc();
	m_pDoc.reset();
	m_treeCtrl_Import->clear();
}

//--------------------------------------------------------------------
// Read a file another format's importer reads; m_Scene stays
// empty if it can't be read (ReadFile has told the user)
//--------------------------------------------------------------------
void ImportDialog::load_scene(const fsLocator& i_ImportLoc)
{
	m_Data.m_LastFile = i_ImportLoc;
	if (!imptSceneAppend::ReadFile( i_ImportLoc, m_Scene ))
		m_Scene = iscnSceneDesc();
}

//--------------------------------------------------------------------
// The file's objects as a tree, all checked; each item holds its
// object's index. A group's check state follows its children, and a
// partly checked group is brought in (iscnSceneChoice::WithAncestors).
//--------------------------------------------------------------------
void ImportDialog::fillintree_scene()
{
	m_treeCtrl_Import->clear();
	add_scene_items( NULL, -1 );
	m_treeCtrl_Import->expandAll();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ImportDialog::add_scene_items(QTreeWidgetItem* i_pParentItem, int i_Parent)
{
	for (int index : iscnSceneChoice::Children( m_Scene, i_Parent ))
	{
		const QStringList text( tqtQString(iscnSceneChoice::ItemLabel( m_Scene.m_Objects[index] )) );
		QTreeWidgetItem* item = i_pParentItem ? new QTreeWidgetItem( i_pParentItem, text )
											  : new QTreeWidgetItem( m_treeCtrl_Import, text );
		item->setData( 0, Qt::UserRole, index );
		item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
		item->setCheckState( 0, Qt::Checked );
		add_scene_items( item, index );
		if (item->childCount() > 0)
			item->setFlags( item->flags() | Qt::ItemIsAutoTristate );
	}
}

//--------------------------------------------------------------------
// Per object: is its item checked or partly checked?
//--------------------------------------------------------------------
std::vector<bool> ImportDialog::chosen_scene_objects() const
{
	std::vector<bool> chosen( m_Scene.m_Objects.size(), false );
	std::vector<QTreeWidgetItem*> items;
	for (int i = 0; i < m_treeCtrl_Import->topLevelItemCount(); ++i)
		items.push_back( m_treeCtrl_Import->topLevelItem(i) );
	while (!items.empty())
	{
		QTreeWidgetItem* item = items.back();
		items.pop_back();
		const int index = item->data( 0, Qt::UserRole ).toInt();
		if (index >= 0 && index < (int)chosen.size())
			chosen[index] = item->checkState(0) != Qt::Unchecked;
		for (int i = 0; i < item->childCount(); ++i)
			items.push_back( item->child(i) );
	}
	return chosen;
}

//--------------------------------------------------------------------
// Load document to import, create data chunks for what was read
//--------------------------------------------------------------------
void ImportDialog::load_document(const fsLocator& i_ImportLoc)
{
	// Store filename that was loaded
	m_Data.m_LastFile = i_ImportLoc;

	//	load in the doc file
	m_pDoc.reset( docSingleTypeMgr::CreateDocument() );
	const bool l_cUPDATE_WITH_CURRENT_DATA = false;
	m_pDoc->SetInactive( l_cUPDATE_WITH_CURRENT_DATA );

	// Do actual load - need to handle exceptions here
	m_Data.m_LoadMethod = docDocument::e_Append_NoDupes;	//	reset it here, user can still set via checkbox
	m_pDoc->Load( i_ImportLoc, m_Data.m_LoadMethod );

	// Get data chunk information from the document
	ImportUtil::BuildDataList( m_pDoc.get(), m_Data );
}

//--------------------------------------------------------------------
// Display the data chunks in checked tree box format
//--------------------------------------------------------------------
void ImportDialog::fillintree()
{
	m_treeCtrl_Import->clear();

	const int num_chunks = m_Data.m_Chunks.size();
	for (int ci= 0; ci < num_chunks; ci++)
	{
		ImportChunkListData &chunk_data = m_Data.m_Chunks[ci];
		const int num_items = chunk_data.m_Items.size();
		if ( num_items > 0 )
		{
			QTreeWidgetItem* chunk_node = new QTreeWidgetItem( m_treeCtrl_Import, QStringList(tqtQString(chunk_data.m_Desc)) );
			chunk_node->setFlags( chunk_node->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsAutoTristate );
			chunk_node->setCheckState( 0, Qt::Unchecked );

			for (int ii = 0 ; ii < num_items ; ii++ )
			{
				QTreeWidgetItem* item = new QTreeWidgetItem( chunk_node, QStringList(tqtQString(chunk_data.m_Items[ii])) );
				item->setFlags( item->flags() | Qt::ItemIsUserCheckable );
				item->setCheckState( 0, Qt::Unchecked );
			}
		}
	}
}

//--------------------------------------------------------------------
//	Update the data list with ONLY the items NOT checked.  These are
//	the items that will get REMOVED from the base scene.  The remaining
//	items in the base scene will then be added to the current scene
//--------------------------------------------------------------------
void ImportDialog::update_ImportDataList()
{
	// Iterate through the top level chunk names
	int chunkindex = 0;
	for (int c = 0; c < m_treeCtrl_Import->topLevelItemCount(); ++c, ++chunkindex)
	{
		QTreeWidgetItem* chunk_id = m_treeCtrl_Import->topLevelItem(c);

		// Skip over chunks that have no items, they weren't added to the tree
		while (m_Data.m_Chunks[chunkindex].m_Items.empty())
			chunkindex++;

		// Note, chunkindex assumes that the tree is the same order as the chunks,
		// that it is not sorted alphabetically.
		ImportChunkListData &chunk_data = m_Data.m_Chunks[chunkindex];

		//	go through each item for this chunk and see if it is unchecked.
		//	Unchecked items get added to the list for removal
		bool bAllUnchecked = true;
		int itemindex = 0;
		for (int i = 0; i < chunk_id->childCount(); ++i)
		{
			QTreeWidgetItem* item_id = chunk_id->child(i);
			if ( item_id->checkState(0) != Qt::Unchecked )
			{
				bAllUnchecked = false;
			}
			else
			{
				// we know the item list has to be big enough because it is where the data
				// originally came from
				DBG_ASSERT(itemindex < chunk_data.m_Items.size(), "Item index out of range: " << itemindex << " < " << chunk_data.m_Items.size());

				//	set the string into its new slot
				chunk_data.m_Items[itemindex] = tqtStdString(item_id->text(0));
				++itemindex;
			}
		}

		if (bAllUnchecked)
		{
			chunk_data.m_bRemovableChunk = true;
		}

		// Shorten up the item list based on how many we processed
		chunk_data.m_Items.resize(itemindex);
	}
}

//--------------------------------------------------------------------
// event handling
//--------------------------------------------------------------------
void ImportDialog::filePicker_FileChanged()
{
	const fsLocator& file_loc = m_filePicker1->GetFullpath();
	clear_file();
	if ( file_loc.GetNumNames() > 0 )
	{
		if (imptSceneAppend::IsSceneFile(file_loc))
		{
			load_scene(file_loc);
			fillintree_scene();
		}
		else
		{
			load_document(file_loc);
			fillintree();
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ImportDialog::buttonImport_Click()
{
	if (m_pDoc == NULL && !m_Scene.m_Objects.empty())
	{
		// Add the checked objects (and their groups) to the scene
		imptSceneAppend::AppendScene( m_Data.m_LastFile, m_Scene, chosen_scene_objects() );
		this->accept();
	}
	else if (m_pDoc != NULL)
	{
		// Update our m_Data structure based on the state of the check boxes
		update_ImportDataList();

		if (this->m_checkBox_AllowDupes->isChecked())
		{
			m_Data.m_LoadMethod = docDocument::e_Append_RenameDupes;
		}
		else
		{
			m_Data.m_LoadMethod = docDocument::e_Append_NoDupes;
		}

		// Merge data from our imported document into the main document
		ImportUtil::ImportDoc( docSingleDocumentMgr::GetDocument(), m_pDoc.get(), m_Data );
		m_pDoc.reset();

		// Close the dialog
		this->accept();
	}
}

#endif // USE_QT
