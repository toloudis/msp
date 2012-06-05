/*****************************************************************************
**	ImportDialog.cpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Import/wxGUI/ImportDialog.hpp"

#include "Features/Import/ImportUtil.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Core/It/itStringUtil.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <set>

#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// enumeration for the icons in the placed tree control
	//--------------------------------------------------------------------
	enum ImportImageState
	{
		e_Unchecked = 0,
		e_Checked = 1,
		e_MixChecked = 2,
		e_NumImportImageStates
	};

	//------------------------------------------------------------------------
	// Return current checked state of tree control item.
	// Since there is no support for checked items in wxWidgets, look at
	// the image state we are using.
	//------------------------------------------------------------------------
	bool get_item_checked( wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node)
	{
		return (i_pTreeCtrl->GetItemImage(i_Node) != e_Unchecked);
	}
	void set_item_checked( wxTreeCtrl* i_pTreeCtrl,
						   wxTreeItemId i_Node,
						   bool i_bChecked)
	{
		return (i_pTreeCtrl->SetItemImage(i_Node, (i_bChecked) ? e_Checked : e_Unchecked));
	}

	//------------------------------------------------------------------------
	// Set checked state of tree node based on checked state of children
	//------------------------------------------------------------------------
	void update_parent_checked_state(wxTreeCtrl* i_pTreeCtrl,
									 wxTreeItemId i_Parent)
	{
		bool bAllChecked = true;
		bool bAllUnchecked = true;

		wxTreeItemIdValue cookie;
		for (wxTreeItemId id = i_pTreeCtrl->GetFirstChild(i_Parent, cookie);
			 id.IsOk(); 
			 id = i_pTreeCtrl->GetNextChild(i_Parent, cookie))
		{
			if (get_item_checked(i_pTreeCtrl, id))
				bAllUnchecked = false;
			else
				bAllChecked = false;
		}

		if (bAllChecked)
			i_pTreeCtrl->SetItemImage(i_Parent, e_Checked);
		else if (bAllUnchecked)
			i_pTreeCtrl->SetItemImage(i_Parent, e_Unchecked);
		else
			i_pTreeCtrl->SetItemImage(i_Parent, e_MixChecked);
	}

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
ImportDialog::ImportDialog( wxWindow* parent, ImportData& i_Data ) 
:	ImportDialogBase( parent ),
	m_Data(i_Data)
{
	// Create state images to represent checked state
	itString icon_dir;
	fsFileUtil::LocatorToUnicodeString( guiMenuMgr::GetIconDirectory(), icon_dir );
	std::wstring icondir = icon_dir.GetString();
	wxImageList *pCheckImages = new wxImageList(13, 13, false, e_NumImportImageStates);
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-unchecked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-checked.png", wxBITMAP_TYPE_PNG));
	pCheckImages->Add(wxBitmap(icondir + L"\\tree-mixchecked.png", wxBITMAP_TYPE_PNG));
	m_treeCtrl_Import->AssignImageList(pCheckImages); // tree ctrl takes ownership

	//
	m_checkBox_AllowDupes->Enable(false);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
ImportDialog::~ImportDialog()
{
	m_Data.m_Chunks.clear();
	m_pDoc.reset();
}

//------------------------------------------------------------------------
// Load document to import, create data chunks for what was read
//------------------------------------------------------------------------
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

//------------------------------------------------------------------------
// Display the data chunks in checked tree box format
//------------------------------------------------------------------------
void ImportDialog::fillintree()
{
	// Clear out the old tree, need a single invisible root node to anchor hierarchy
	m_treeCtrl_Import->DeleteAllItems();
	wxTreeItemId root = m_treeCtrl_Import->AddRoot(L"Root");

	const int num_chunks = m_Data.m_Chunks.size();
	//DBG_LOG( "Building Tree with " << num_chunks << " chunks" );
	for (int ci= 0; ci < num_chunks; ci++)
	{
		ImportChunkListData &chunk_data = m_Data.m_Chunks[ci];
		//DBG_LOG3( "Filling tree chunk %d %s num items: %d", ci, chunk_data.m_Desc.c_str(), chunk_data.m_Items.size() );

		const int num_items = chunk_data.m_Items.size();
		if ( num_items > 0 )
		{
			wxString chunk_text(chunk_data.m_Desc.c_str(), wxConvUTF8);
			wxTreeItemId chunk_node = m_treeCtrl_Import->AppendItem(root, chunk_text, e_Unchecked);
			//node->Expand();
			//DBG_LOG2( " %02d %s", ci, chunk_text.c_str() );

			for (int ii = 0 ; ii < num_items ; ii++ )
			{
				wxString item_text(chunk_data.m_Items[ii].c_str(), wxConvUTF8);
				m_treeCtrl_Import->AppendItem(chunk_node, item_text, e_Unchecked);
				//DBG_LOG2( "   %02d %s", iindex, item_text.c_str() );
			}
		}
	}
}

//------------------------------------------------------------------------
//	Update the data list with ONLY the items NOT checked.  These are 
//	the items that will get REMOVED from the base scene.  The remaining
//	items in the base scene will then be added to the current scene
//------------------------------------------------------------------------
void ImportDialog::update_ImportDataList()
{
	//DBG_LOG("In ImportDialog::update_ImportDataList()");

	wxTreeItemId root = m_treeCtrl_Import->GetRootItem();
	DBG_ASSERT(root.IsOk(), "Assuming that there is a root node already in tree control.");

	// Iterate through the top level chunk names
	int chunkindex = 0;
	wxTreeItemIdValue root_cookie;
	for (wxTreeItemId chunk_id = m_treeCtrl_Import->GetFirstChild(root, root_cookie);
		 chunk_id.IsOk(); 
		 chunk_id = m_treeCtrl_Import->GetNextChild(root, root_cookie),
			chunkindex++)
	{	
		// Skip over chunks that have no items, they weren't added to the tree
		while (m_Data.m_Chunks[chunkindex].m_Items.empty())
			chunkindex++;

		// Note, chunkindex assumes that the tree is the same order as the chunks,
		// that it is not sorted alphabetically.
		ImportChunkListData &chunk_data = m_Data.m_Chunks[chunkindex];
		//DBG_LOG3( "Processing tree chunk %d %s num items: %d", chunkindex, chunk_data.m_Desc.c_str(), chunk_data.m_Items.size() );

		//	go through each item for this chunk and see if it is unchecked.
		//	Unchecked items get added to the list for removal
		//
		bool bAllUnchecked = true;

		int itemindex = 0;
		wxTreeItemIdValue chunk_cookie;
		for (wxTreeItemId item_id = m_treeCtrl_Import->GetFirstChild(chunk_id, chunk_cookie);
			 item_id.IsOk(); 
			 item_id = m_treeCtrl_Import->GetNextChild(chunk_id, chunk_cookie))
		{	
			if ( get_item_checked(m_treeCtrl_Import, item_id) )
			{
				//DBG_LOG("  Found checked item");
				bAllUnchecked = false;
			}
			else
			{
				//	add it to the list (thus destroying the old list by "scrunching" up the names)
				//
				std::string tempstring = m_treeCtrl_Import->GetItemText(item_id).utf8_str();

				// we know the item list has to be big enough because it is where the data
				// originally came from
				DBG_ASSERT(itemindex < chunk_data.m_Items.size(), "Item index out of range: " << itemindex << " < " << chunk_data.m_Items.size());

				//	set the string into its new slot
				chunk_data.m_Items[itemindex] = tempstring;
				//DBG_LOG2( "  --ADDED %02d %s", itemindex, tempstring.c_str() );
				++itemindex;
			}
		}

		if (bAllUnchecked)
		{
			//DBG_LOG("   --No items checked, flag for removing");
			chunk_data.m_bRemovableChunk = true;
		}
		// Shorten up the item list based on how many we processed
		chunk_data.m_Items.resize(itemindex);
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void ImportDialog::clear_data()
{
}

//--------------------------------------------------------------------
// event handling
//--------------------------------------------------------------------
void ImportDialog::filePicker_FileChanged( wxFileDirPickerEvent& i_Event )
{
	fsLocator file_loc;
	fsFileUtil::UnicodeStringToLocator(itString(i_Event.GetPath().c_str()), file_loc);
	if ( file_loc.GetNumNames() > 0 )
	{
		load_document(file_loc);
		fillintree();
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ImportDialog::treeCtrl_LeftMouseDown( wxMouseEvent& i_Event )
{
	bool bSkip = true;

	int flags = 0;
	wxTreeItemId pick_item = m_treeCtrl_Import->HitTest(i_Event.GetPosition(), flags);
	if (pick_item.IsOk())
	{
		// Check for click on the icon
		if (flags & wxTREE_HITTEST_ONITEMICON)
		{
			//TODO: need to toggle state within chunk data

			// Clicked on check box for item
			bool bNewCheckState = (!get_item_checked(m_treeCtrl_Import, pick_item));
			set_item_checked(m_treeCtrl_Import, pick_item, bNewCheckState);

			if (m_treeCtrl_Import->ItemHasChildren(pick_item))
			{
				//set checked state of child nodes
				wxTreeItemIdValue cookie;
				for (wxTreeItemId id = m_treeCtrl_Import->GetFirstChild(pick_item, cookie);
					 id.IsOk(); 
					 id = m_treeCtrl_Import->GetNextChild(pick_item, cookie))
				{	
					set_item_checked(m_treeCtrl_Import, id, bNewCheckState);
				}
			}
			else
			{
				update_parent_checked_state( m_treeCtrl_Import, m_treeCtrl_Import->GetItemParent(pick_item) );
			}

			bSkip = false;
		}
	}
	i_Event.Skip(bSkip);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ImportDialog::buttonImport_Click( wxCommandEvent& i_Event )
{
	if (m_pDoc != NULL)
	{
		// Update our m_Data structure based on the state of the check boxes
		update_ImportDataList();

		if (this->m_checkBox_AllowDupes->IsChecked())
		{
			m_Data.m_LoadMethod = docDocument::e_Append_RenameDupes;
		}
		else
		{
			m_Data.m_LoadMethod = docDocument::e_Append_NoDupes;
		}

		//DBG_LOG( "[IMPORT] data chunks in list = " << m_Data.m_Chunks.size() );

		// Merge data from our imported document into the main document
		ImportUtil::ImportDoc( docSingleDocumentMgr::GetDocument(), m_pDoc.get(), m_Data );

		m_pDoc.reset();

		// Close the dialog
		this->EndModal( wxOK );
	}
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void ImportDialog::AllowDupes_OnCheckBox( wxCommandEvent& event )
{
}

#endif // USE_WXWIDGETS