#pragma once

#ifndef IMPORTDATA_HPP
#include "Features/Import/ImportData.hpp"
#endif
#ifndef IMPORTDIALOGUTIL_HPP
#include "Features/Import/ImportDialogUtil.hpp"
#endif

#ifndef IMPORTUTIL_HPP
#include "Features/Import/ImportUtil.hpp"
#endif
#ifndef MNM_PATHS_HPP
#include "Support/mnm/mnmPaths.hpp"
#endif

#ifndef DBG_LOG_HPP
#include "Core/dbg/dbgLog.hpp"
#endif
#ifndef DOC_DOCUMENT_HPP
#include "Tool/doc/docDocument.hpp"
#endif
#ifndef DOC_SINGLEDOCUMENTMGR_HPP
#include "Tool/doc/docSingleDocumentMgr.hpp"
#endif
#ifndef DOC_SINGLETYPEMGR_HPP
#include "Tool/doc/docSingleTypeMgr.hpp"
#endif
#ifndef FS_FILEUTIL_HPP
#include "Core/fs/fsFileUtil.hpp"
#endif
#ifndef GF_PATHS_HPP
#include "Core/gf/gfPaths.hpp"
#endif
#ifndef IT_STRINGUTIL_HPP
#include "Core/it/itStringUtil.hpp"
#endif
#ifndef GUI_FILEDIALOGUTILS_HPP
#include "Tool/gui/guiFileDialogUtils.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace StudioFramework
{
	/// <summary>
	/// Summary for ImportMain
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class ImportMain : public System::Windows::Forms::Form
	{
	public:
		ImportMain( ImportData& i_Data )
			: m_Data( i_Data )
		{
			InitializeComponent();

			update_ImportDialog();
		}

	protected:
		~ImportMain()
		{
			if (components)
			{
				delete components;
			}
		}








	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

	private: System::Windows::Forms::Button ^  button_open;
	private: System::Windows::Forms::GroupBox ^  groupBox_shotfile;

	private: System::Windows::Forms::Label ^  label_tree;
	private: System::Windows::Forms::TextBox ^  textBox_rootfile;
	private: System::Windows::Forms::TreeView ^  treeView_inventory;
	private: System::Windows::Forms::Button ^  button_inventory;


		docDocument* m_pDoc;
	private: System::Windows::Forms::Button^  button_inventory_openpartial;
	private: System::Windows::Forms::Button^  button_inventory_collapseall;
	private: System::Windows::Forms::Button^  button_inventory_openall;
			 ImportData& m_Data;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox_rootfile = (gcnew System::Windows::Forms::TextBox());
			this->button_open = (gcnew System::Windows::Forms::Button());
			this->groupBox_shotfile = (gcnew System::Windows::Forms::GroupBox());
			this->treeView_inventory = (gcnew System::Windows::Forms::TreeView());
			this->label_tree = (gcnew System::Windows::Forms::Label());
			this->button_inventory = (gcnew System::Windows::Forms::Button());
			this->button_inventory_openpartial = (gcnew System::Windows::Forms::Button());
			this->button_inventory_collapseall = (gcnew System::Windows::Forms::Button());
			this->button_inventory_openall = (gcnew System::Windows::Forms::Button());
			this->groupBox_shotfile->SuspendLayout();
			this->SuspendLayout();
			// 
			// textBox_rootfile
			// 
			this->textBox_rootfile->Location = System::Drawing::Point(16, 24);
			this->textBox_rootfile->Name = L"textBox_rootfile";
			this->textBox_rootfile->ReadOnly = true;
			this->textBox_rootfile->Size = System::Drawing::Size(360, 20);
			this->textBox_rootfile->TabIndex = 0;
			// 
			// button_open
			// 
			this->button_open->Location = System::Drawing::Point(392, 24);
			this->button_open->Name = L"button_open";
			this->button_open->Size = System::Drawing::Size(24, 23);
			this->button_open->TabIndex = 1;
			this->button_open->Text = L"...";
			this->button_open->Click += gcnew System::EventHandler(this, &ImportMain::button_open_Click);
			// 
			// groupBox_shotfile
			// 
			this->groupBox_shotfile->Controls->Add(this->textBox_rootfile);
			this->groupBox_shotfile->Controls->Add(this->button_open);
			this->groupBox_shotfile->Location = System::Drawing::Point(8, 16);
			this->groupBox_shotfile->Name = L"groupBox_shotfile";
			this->groupBox_shotfile->Size = System::Drawing::Size(432, 64);
			this->groupBox_shotfile->TabIndex = 2;
			this->groupBox_shotfile->TabStop = false;
			this->groupBox_shotfile->Text = L"Root Scene/Shot file";
			// 
			// treeView_inventory
			// 
			this->treeView_inventory->Location = System::Drawing::Point(8, 112);
			this->treeView_inventory->Name = L"treeView_inventory";
			this->treeView_inventory->Size = System::Drawing::Size(432, 304);
			this->treeView_inventory->TabIndex = 2;
			this->treeView_inventory->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &ImportMain::treeView_inventory_AfterCheck);
			// 
			// label_tree
			// 
			this->label_tree->Location = System::Drawing::Point(8, 88);
			this->label_tree->Name = L"label_tree";
			this->label_tree->Size = System::Drawing::Size(176, 16);
			this->label_tree->TabIndex = 0;
			this->label_tree->Text = L"Root Scene/Shot Inventory List";
			// 
			// button_inventory
			// 
			this->button_inventory->Location = System::Drawing::Point(160, 432);
			this->button_inventory->Name = L"button_inventory";
			this->button_inventory->Size = System::Drawing::Size(128, 23);
			this->button_inventory->TabIndex = 3;
			this->button_inventory->Text = L"Import!";
			this->button_inventory->Click += gcnew System::EventHandler(this, &ImportMain::button_inventory_Click);
			// 
			// button_inventory_openpartial
			// 
			this->button_inventory_openpartial->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_inventory_openpartial->Location = System::Drawing::Point(40, 432);
			this->button_inventory_openpartial->Name = L"button_inventory_openpartial";
			this->button_inventory_openpartial->Size = System::Drawing::Size(16, 16);
			this->button_inventory_openpartial->TabIndex = 8;
			this->button_inventory_openpartial->Text = L":";
			this->button_inventory_openpartial->Click += gcnew System::EventHandler(this, &ImportMain::button_inventory_openpartial_Click);
			// 
			// button_inventory_collapseall
			// 
			this->button_inventory_collapseall->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_inventory_collapseall->Location = System::Drawing::Point(24, 432);
			this->button_inventory_collapseall->Name = L"button_inventory_collapseall";
			this->button_inventory_collapseall->Size = System::Drawing::Size(16, 16);
			this->button_inventory_collapseall->TabIndex = 7;
			this->button_inventory_collapseall->Text = L"-";
			this->button_inventory_collapseall->Click += gcnew System::EventHandler(this, &ImportMain::button_inventory_collapseall_Click);
			// 
			// button_inventory_openall
			// 
			this->button_inventory_openall->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_inventory_openall->Location = System::Drawing::Point(8, 432);
			this->button_inventory_openall->Name = L"button_inventory_openall";
			this->button_inventory_openall->Size = System::Drawing::Size(16, 16);
			this->button_inventory_openall->TabIndex = 6;
			this->button_inventory_openall->Text = L"+";
			this->button_inventory_openall->Click += gcnew System::EventHandler(this, &ImportMain::button_inventory_openall_Click);
			// 
			// ImportMain
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(456, 469);
			this->Controls->Add(this->button_inventory_openpartial);
			this->Controls->Add(this->button_inventory_collapseall);
			this->Controls->Add(this->button_inventory_openall);
			this->Controls->Add(this->button_inventory);
			this->Controls->Add(this->label_tree);
			this->Controls->Add(this->treeView_inventory);
			this->Controls->Add(this->groupBox_shotfile);
			this->MaximizeBox = false;
			this->MinimizeBox = false;
			this->Name = L"ImportMain";
			this->Text = L"Import a Scene/Shot file into the current scene";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &ImportMain::ImportMain_Closing);
			this->groupBox_shotfile->ResumeLayout(false);
			this->groupBox_shotfile->PerformLayout();
			this->ResumeLayout(false);

		}

private: System::Void ImportMain_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		 {
			 ImportDialogUtil::Hide();

			 delete m_pDoc;
			 m_pDoc = 0;
		 }

private: System::Void button_inventory_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 update_ImportDataList();

			 //DBG_LOG1( "[IMPORT] data chunks in list = %d", m_Data.m_Chunks.size() );

			 ImportUtil::ImportDoc( docSingleDocumentMgr::GetDocument(), m_pDoc, m_Data );

			 this->Close();
		 }

private: System::Void treeView_inventory_AfterCheck(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			// The code only executes if the user caused the checked state to change.
			if (e->Action != TreeViewAction::Unknown) 
			{
				if (e->Node->Nodes->Count > 0) 
				{
					// Calls the CheckAllChildNodes method, passing in the current
					// Checked value of the TreeNode whose checked state changed.
					this->CheckAllChildNodes(e->Node, e->Node->Checked);
				}
			}
		 }

private: System::Void button_open_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			std::string filter = "scene-shot files (*.mab)|*.mab|All files (*.*)|*.*";
			fsLocator file_loc;
			fsLocator initial_dir;
			initial_dir = gfPaths::GetPath( mnmPaths::e_SaveShots );

			if (guiFileDialogUtils::GetOpenFileName(filter, initial_dir, file_loc))
			{
				if ( file_loc.GetNumNames() > 0 )
				{
					button_inventory->Enabled = true;

					std::string name;
					name = itStringUtil::GetStdString(file_loc.GetLastName());

					UpdateRootFile( file_loc );
					update_ImportDataFilename();

					fillintree();

					//DBG_LOG1( "(OPEN) data chunks in list = %d", m_Data.m_Chunks.size() );
				}
			}
		 }

private:
		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Void UpdateRootFile( fsLocator& i_dir )
		{
			System::String^ dir = tmaManagedStringUtils::LocatorToManagedString( i_dir );
			textBox_rootfile->Text = dir;
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Void update_ImportDialog()
		{
			button_inventory->Enabled = false;

			treeView_inventory->CheckBoxes = true;
			treeView_inventory->Nodes->Clear();

			textBox_rootfile->Clear();
			//UpdateRootFile( m_Data.m_LastFile );
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Void update_ImportDataFilename()
		{
			tmaManagedStringUtils::ManagedStringToLocator( textBox_rootfile->Text, m_Data.m_LastFile );

			//std::string dir;
			//fsFileUtil::LocatorToANSIFilename( m_Data.m_LastFile, dir );
			//DBG_LOG1( "doc dir (%s)", dir.c_str() );
		}

		//------------------------------------------------------------------------
		//	Update the data list with ONLY the items NOT checked.  These are 
		//	the items that will get REMOVED from the base scene.  The remaining
		//	items in the base scene will then be added to the current scene
		//------------------------------------------------------------------------
		System::Void update_ImportDataList()
		{
			int itemindex;
			int chunkindex;

			//DBG_LOG0( "ImportMain: update_ImportDataList " );

			TreeNode^ chunkNode;
			TreeNode^ itemNode;

			chunkindex = 0;

			IEnumerator^ myItemEnum;
			IEnumerator^ myChunkEnum;

			//	skip the scene root node.
			//
			myChunkEnum = treeView_inventory->Nodes->GetEnumerator();
			myChunkEnum->MoveNext();
			chunkNode = safe_cast<TreeNode^>(myChunkEnum->Current);

			myChunkEnum = chunkNode->Nodes->GetEnumerator();
			while (myChunkEnum->MoveNext()) 
			{
				chunkNode = safe_cast<TreeNode^>(myChunkEnum->Current);

				myItemEnum = chunkNode->Nodes->GetEnumerator();

				// find the correct chunk index
				//
				std::string chunkdesc;
				tmaManagedStringUtils::ManagedStringToStdString( chunkNode->Text, chunkdesc );

				int size = m_Data.m_Chunks.size();
				int chunkindex;
				for ( chunkindex = 0 ; chunkindex < size ; chunkindex++ )
				{
					if ( m_Data.m_Chunks[ chunkindex ].m_Desc == chunkdesc )
					{
						break;
					}
				}

				//DBG_LOG2( "chunk name (%s) %d", chunkdesc.c_str(), chunkindex );

				//	go through each item for this chunk and see if it is unchecked.
				//	Unchecked items get added to the list for removal
				//
				bool bAllUnchecked = true;
				itemindex = 0;
				while (myItemEnum->MoveNext())
				{
					itemNode = safe_cast<TreeNode^>(myItemEnum->Current);

					//DBG_LOG3( " itemnode (%s) %d (%s)", itemNode->Text, itemindex, (itemNode->Checked?"true":"false") );

					if ( itemNode->Checked )
					{
						bAllUnchecked = false;
					}
					else
					{
						//	add it to the list (thus destroying the old list by "scrunching" up the names)
						//
						std::string tempstring;
						tmaManagedStringUtils::ManagedStringToStdString( itemNode->Text, tempstring );

						ImportChunkListData* pCLData;
						pCLData = &(m_Data.m_Chunks[chunkindex]);

						//	is there any items?  if so continue
						//
						//if ( m_Data.m_Chunks[chunkindex].m_Items._Myfirst != 0 )
						//if ( m_Data.m_Chunks[chunkindex].m_Items.size() > 0 )
						{
							//	if the size too small, make it bigger
							if ( m_Data.m_Chunks[chunkindex].m_Items.size() <= itemindex )
							{
								m_Data.m_Chunks[chunkindex].m_Items.resize( itemindex+5 );
							}

							//	if the size of the string is too small, resize it.
							if ( m_Data.m_Chunks[chunkindex].m_Items[itemindex].size() < tempstring.size() )
							{
								m_Data.m_Chunks[chunkindex].m_Items[itemindex].resize( tempstring.size() );
							}

							//	add the string and increment the counter
							m_Data.m_Chunks[chunkindex].m_Items[itemindex] = tempstring;

							//DBG_LOG2( "  --ADDED %02d %s", itemindex, tempstring.c_str() );

							++itemindex;
						}
					}
				}

				if (bAllUnchecked)
				{
					//DBG_LOG0("   --No items checked, flag for removing");
					m_Data.m_Chunks[chunkindex].m_bRemovableChunk = true;
					m_Data.m_Chunks[chunkindex].m_Items.resize(itemindex);
				}
				else
				{
					m_Data.m_Chunks[chunkindex].m_Items.resize(itemindex);
				}
			}
		}

		//------------------------------------------------------------------------
		// Updates all child tree nodes recursively.
		//------------------------------------------------------------------------
		void CheckAllChildNodes(TreeNode^ treeNode, bool nodeChecked) 
		{
			IEnumerator^ myEnum = treeNode->Nodes->GetEnumerator();
			while (myEnum->MoveNext()) 
			{
				TreeNode^ node = safe_cast<TreeNode^>(myEnum->Current);

				node->Checked = nodeChecked;
				if (node->Nodes->Count > 0) 
				{
					// If the current node has child nodes, call the CheckAllChildsNodes method recursively.
					this->CheckAllChildNodes(node, nodeChecked);
				}
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void fillintree()
		{
			treeView_inventory->Nodes->Clear();

			if ( m_Data.m_LastFile.GetNumNames() == 0 )
			{
				return;
			}

			std::string name;
			name = itStringUtil::GetStdString( m_Data.m_LastFile.GetLastName() );
			//DBG_LOG1( "scene item list (%s)", name.c_str() );

			// Add nodes to treeView_inventory.
			//
			TreeNode^ rootnode;
			TreeNode^ node;
			int n=0;

			rootnode = treeView_inventory->Nodes->Add(String::Format(gcnew System::String(name.c_str()), n++));
			rootnode->Expand();

			//	load in the doc file
			m_pDoc = docSingleTypeMgr::CreateDocument();
			const bool l_cUPDATE_WITH_CURRENT_DATA = false;
			m_pDoc->SetInactive( l_cUPDATE_WITH_CURRENT_DATA );

			m_pDoc->Load( m_Data.m_LastFile );

			//	
			ImportUtil::BuildDataList( m_pDoc, m_Data );

			//
			int cindex;
			int iindex;
			int inum;
			int cnum = m_Data.m_Chunks.size();

			//DBG_LOG1( "Building Tree with %d chunks", cnum );
			for ( cindex = 0 ; cindex < cnum ; cindex++ )
			{
				inum = m_Data.m_Chunks[cindex].m_Items.size();

				if ( inum > 0 )
				{
					//DBG_LOG2( " %02d %s", cindex, m_Data.m_Chunks[cindex].m_Desc.c_str() );
					node = rootnode->Nodes->Add(String::Format( gcnew System::String(m_Data.m_Chunks[cindex].m_Desc.c_str()) , n++));
					node->Expand();

					for ( iindex = 0 ; iindex < inum ; iindex++ )
					{
						node->Nodes->Add(String::Format( gcnew System::String(m_Data.m_Chunks[cindex].m_Items[iindex].c_str()), n++ ));
						//DBG_LOG2( "   %02d %s", iindex, m_Data.m_Chunks[cindex].m_Items[iindex].c_str() );
					}
				}
			}
		}
private: System::Void button_inventory_openall_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->treeView_inventory->ExpandAll();
		 }
private: System::Void button_inventory_collapseall_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
				 this->treeView_inventory->CollapseAll();
		 }
private: System::Void button_inventory_openpartial_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			if (treeView_inventory->SelectedNode == nullptr)
			{
				 this->treeView_inventory->ExpandAll();
			}
			else
			{
				TreeNode^ pTN;
				pTN = treeView_inventory->SelectedNode;
				if (pTN == nullptr)
					return;
				pTN->ExpandAll();
			}
		}
};
}

#endif // _MANAGED
