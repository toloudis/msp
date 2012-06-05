#pragma once


#ifndef CMM_DIALOGINTERESTMGR_HPP
#include "Systems/Common/GUI/cmmDialogInterestMgr.hpp"
#endif
#ifndef CMM_OBJECTDIALOGUTIL_HPP
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#endif
#ifndef CMM_SYSTEMDIALOGUTIL_HPP
#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#endif
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif
#ifndef SCENESETUPDIALOGUTIL_hpp
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"
#endif
#ifndef SCENESETUPDATA_HPP
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#endif
#ifndef SCENESETUPDOCUMENTCHUNK_HPP
#include "Features/SceneSetup/Data/SceneSetupDocumentChunk.hpp"
#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef GSUP_TREEVIEWUTIL_HPP
#include "Support/gsup/gsupTreeViewUtil.hpp"
#endif
#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif
#ifndef VIS_MGR_HPP
#include "Support/vis/visMgr.hpp"
#endif

#ifndef CMA_COMMANDMGR_HPP
#include "Tool/cma/cmaCommandMgr.hpp"
#endif
#ifndef TMA_MESSAGING_HPP
#include "ToolUIManaged/tma/tmaMessaging.hpp"
#endif
#ifndef PICK3D_PICKLIST_HPP
#include "Tool/pick3d/pick3dPickList.hpp"
#endif
#ifndef PICK3D_PICKOBJECT_HPP
#include "Tool/pick3d/pick3dPickObject.hpp"
#endif
#ifndef SEL3D_MGR_HPP
#include "Tool/sel3d/sel3dMgr.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef UNDO_UNDOMGR_HPP
#include "Core/undo/undoUndoMgr.hpp"
#endif

#include <string>
#include <algorithm>

#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;
using namespace TerawattManagedControls;


//============================================================================
//============================================================================
namespace StudioFramework
{
	/// <summary> 
	/// Summary for cmmSystemForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmmSystemForm : public System::Windows::Forms::Form
	{
	public:	static cmmSystemForm^ FormInstance = nullptr;

	public: 
		cmmSystemForm()
		{
			m_bOurChange = false;
			m_bDisableNotify = true;

			InitializeComponent();

			m_pFileListAvailable = new fsysFileList;
			m_pDataListPlaced = new cmmDialogDataList;

			SetupData();

			m_pMemory = gcnew tmaDialogMemory( this );
			m_bDisableNotify = false;

			//create_context_menu(this->treeView_available);
			create_context_menu(this->treeView_placed);
		}

		void Update()
		{
			buildtreeview_system_available();
			buildtreeview_system_placed();

			SceneSetupData& data = SceneSetupDialogUtil::Data();
			Update(data.m_PropertiesData);
		}

	public: void Update(const ScenePropertiesData& i_Data)
		{
			this->text_notes->Text = gcnew System::String(i_Data.m_Notes.c_str());
		}

		void Clear()
		{
			//	clear the data
			m_pDataListPlaced->clear();

			//	clear the trees
			treeView_placed->Nodes->Clear();
			treeView_available->Nodes->Clear();

			//	change the button states
			enable_placed_buttons();
			enable_available_buttons();
		}

	public:
		void UpdatePlacedList(const std::string& i_SystemName, cmmDialogDataList& i_DataList)
		{
			cmmDialogInterestMgr::UpdateList(i_SystemName, i_DataList, *m_pDataListPlaced);

			//	rebuild the treeview
			//
			updatetreeview_system_placed();
		}
		void UpdateAvailableList()
		{
			//	rebuild the treeview
			//
			buildtreeview_system_available();
		}

		void ClearPickList()
		{
			listBox_selected->Items->Clear();
		}

		void UpdatePickList(const pick3dPickList& i_PickList, const pick3dPickObject* i_pSelObj)
		{
			ClearPickList();

			for (int i = 0; i < i_PickList.GetSize(); ++i)
			{
				//nameObject* pObj = static_cast<nameObject*>(i_pSelList->GetItem(i));
				//if (pObj != 0)
				//{
				//	listBox_selected->Items->Add( gcnew System::String( pObj->GetName().GetString().c_str() ) );
				//}
				System::String^ lineitem;
				pick3dPickObject* pListObj = i_PickList.GetItem(i)->GetObject();
				if (pListObj == i_pSelObj)
				{
					lineitem = System::String::Format("->Item #{0} - {1}", i, gcnew System::String(pListObj->GetPick3dName().c_str()) );
				}
				else
				{
					lineitem = System::String::Format("  Item #{0} - {1}", i, gcnew System::String(pListObj->GetPick3dName().c_str()) );
				}
				listBox_selected->Items->Add( lineitem );
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TabPage^ GetTabPage(String^ i_TabName)
		{
			int count = this->tabControl_system->Controls->Count;
			for (int i=0; i<count; ++i)
			{
				if (this->tabControl_system->Controls[i]->Text == i_TabName)
				{
					return static_cast<TabPage^>(this->tabControl_system->Controls[i]);
				}
			}
			return nullptr;
		}

		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		void AddTabPage( TabPage^ i_pTabPage )
		{
			this->tabControl_system->Controls->Add(i_pTabPage);
			//tabControl_system->SelectedIndex = (m_pTabControl->TabCount - 1);
		}
		
		//---------------------------------------------------------------------------
		//	Remove the tab page (by pointer)
		//---------------------------------------------------------------------------
		void RemoveTabPage( TabPage^ i_pTabPage )
		{
			this->tabControl_system->Controls->Remove(i_pTabPage);
		}

		//------------------------------------------------------------------------
		//	Clear selection in tree view
		//------------------------------------------------------------------------
		void ClearTreeSelection()
		{
			this->treeView_placed->SelectedNode = nullptr;
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void SelectObjectOnPlacedList(pick3dPickObject* i_pPickObject)
		{
			//	find the name in the tree
			TreeNode^ pTN = find_node_by_pick_object(i_pPickObject);
			if (pTN != nullptr)
			{
				m_bOurChange = true;
				treeView_placed->SelectedNode = pTN;
				m_bOurChange = false;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void AddToSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
		{
			//	find the name in the tree
			TreeNode^ pTN = find_node_by_pick_object(i_pPickObject);
			if (pTN != nullptr)
			{
				//treeView_placed->AddToSelectedNodes(pTN);
				if (treeView_placed->SelectedNodes->Contains(pTN))
					HighlightObjectOnPlacedList(pTN);
				else
					UnhighlightObjectOnPlacedList(pTN);

				//DBG_LOG1("Add Placed Selected = %d", treeView_placed->SelectedNodes->Count);
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void RemoveFromSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject)
		{
			//	find the name in the tree
			TreeNode^ pTN = find_node_by_pick_object(i_pPickObject);
			if (pTN != nullptr)
			{
				//treeView_placed->RemoveFromSelectedNodes(pTN);
				if (treeView_placed->SelectedNodes->Contains(pTN))
					HighlightObjectOnPlacedList(pTN);
				else
					UnhighlightObjectOnPlacedList(pTN);

				//DBG_LOG1("Remove Placed Selected = %d", treeView_placed->SelectedNodes->Count);
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void HighlightObjectOnPlacedList(TreeNode^ i_TreeNode)
		{
			if (i_TreeNode != nullptr)
			{
				i_TreeNode->ForeColor = System::Drawing::SystemColors::Highlight;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		void UnhighlightObjectOnPlacedList(TreeNode^ i_TreeNode)
		{
			if (i_TreeNode != nullptr)
			{
				i_TreeNode->ForeColor = System::Drawing::SystemColors::ControlText;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		bool GetVisibleFlag()
		{
			return this->m_pMemory->GetVisibleFlag();
		}

		//------------------------------------------------------------------------
		//	Add a command to the system tab page command tree.
		//------------------------------------------------------------------------
		void AddSystemCommand(const std::string& i_System, 
							  const std::string& i_DisplayText, 
							  const std::string& i_CommandTag, 
							  System::Int16 i_CmdID )
		{
			TreeNode^ pTN;
			TreeNode^ pSTN;

			treeView_system->BeginUpdate();

			// if system node doesn't exist yet, create it first
			pSTN = gsupTreeViewUtil::FindTreeNode( treeView_system->Nodes, gcnew System::String(i_System.c_str()) );
			if (pSTN == nullptr)
			{
				TreeNode^ pNewTN = gcnew TreeNode( gcnew String(i_System.c_str()) );
				treeView_system->Nodes->Add( pNewTN );
				pSTN = pNewTN;
				pSTN->BackColor = System::Drawing::Color::LightGray;
				pSTN->ForeColor = System::Drawing::Color::Black;
			}

			pTN = gsupTreeViewUtil::FindTreeNode( pSTN->Nodes, gcnew System::String(i_DisplayText.c_str()) );
			if (pTN == nullptr)
			{
				TreeNode^ pObjectTreeNode = gcnew TreeNode( gcnew String(i_DisplayText.c_str()) );
				//pObjectTreeNode->Tag = gcnew String(i_CommandTag.c_str());
				pObjectTreeNode->Tag = build_objecttreenode_tag(i_CommandTag, i_CmdID);

				//	add the node
				pSTN->Nodes->Add( pObjectTreeNode );
				pSTN->ExpandAll();

				gsupTreeViewUtil::SortTreeView( treeView_system, true, true );
			}

			treeView_system->CollapseAll();
			treeView_system->EndUpdate();
		}
	protected: 
		~cmmSystemForm()
		{
			// clear instance
			if (cmmSystemForm::FormInstance == this)
				cmmSystemForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	//--------------------------------------------------------------------
	//	TO DO - why doesn't this function ever get called?  
	//	It works for MainForm and chnlTraxEditor.
	//--------------------------------------------------------------------
protected: virtual bool ProcessCmdKey(Message% msg, Keys keyData) override
	{
		//bool bProcessed = cmaMessaging::ProcessCmdKey(msg, keyData);

		return (/*bProcessed ||*/ (__super::ProcessCmdKey(msg,keyData)));
	}

	private: System::Windows::Forms::TabControl ^  tabControl_system;
	private: System::Windows::Forms::TabPage ^  tabPage_available;
	private: TerawattManagedControls::TreeViewMS ^  treeView_available;
	private: System::Windows::Forms::Button ^  button_add;
	private: System::Windows::Forms::Button ^  button_refreshavailable;
	private: System::Windows::Forms::Button ^  button_available_openall;
	private: System::Windows::Forms::Button ^  button_available_collapseall;
	private: System::Windows::Forms::Button ^  button_available_openpartial;

	private: System::Windows::Forms::TabPage ^  tabPage_placed;

	private: System::Windows::Forms::Button ^  button_delete;
	private: System::Windows::Forms::Button ^  button_edit;
	private: System::Windows::Forms::Button ^  button_duplicate;
	private: System::Windows::Forms::Button ^  button_placed_openpartial;
	private: System::Windows::Forms::Button ^  button_placed_collapseall;
	private: System::Windows::Forms::Button ^  button_placed_openall;
	private: TerawattManagedControls::TreeViewMS ^  treeView_placed;
	private: System::Windows::Forms::Button ^  button_reload;

	private: System::Windows::Forms::TabPage ^  tabPage_system;
	private: System::Windows::Forms::TreeView ^  treeView_system;
	private: System::Windows::Forms::Button ^  button_systemtree_execute;

	private: System::Windows::Forms::Label ^  label_status;

	private: System::Windows::Forms::TabPage ^  tabPage_notes;
	private: System::Windows::Forms::Label ^  label_notes;
	private: System::Windows::Forms::TextBox ^  text_notes;

	private: System::Windows::Forms::TabPage ^  tabPage_selected;
	private: System::Windows::Forms::Label ^  label_selected;
	private: System::Windows::Forms::ListBox ^  listBox_selected;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl_system = gcnew System::Windows::Forms::TabControl();
			this->tabPage_available = gcnew System::Windows::Forms::TabPage();
			this->button_available_openpartial = gcnew System::Windows::Forms::Button();
			this->button_available_collapseall = gcnew System::Windows::Forms::Button();
			this->button_available_openall = gcnew System::Windows::Forms::Button();
			this->button_add = gcnew System::Windows::Forms::Button();
			this->button_refreshavailable = gcnew System::Windows::Forms::Button();
			this->treeView_available = gcnew TerawattManagedControls::TreeViewMS();
			this->tabPage_placed = gcnew System::Windows::Forms::TabPage();
			this->button_reload = gcnew System::Windows::Forms::Button();
			this->button_placed_openpartial = gcnew System::Windows::Forms::Button();
			this->button_placed_collapseall = gcnew System::Windows::Forms::Button();
			this->button_placed_openall = gcnew System::Windows::Forms::Button();
			this->button_duplicate = gcnew System::Windows::Forms::Button();
			this->button_edit = gcnew System::Windows::Forms::Button();
			this->button_delete = gcnew System::Windows::Forms::Button();
			this->treeView_placed = gcnew TerawattManagedControls::TreeViewMS();
			this->tabPage_system = gcnew System::Windows::Forms::TabPage();
			this->button_systemtree_execute = gcnew System::Windows::Forms::Button();
			this->treeView_system = gcnew System::Windows::Forms::TreeView();
			this->tabPage_notes = gcnew System::Windows::Forms::TabPage();
			this->label_notes = gcnew System::Windows::Forms::Label();
			this->text_notes = gcnew System::Windows::Forms::TextBox();
			this->tabPage_selected = gcnew System::Windows::Forms::TabPage();
			this->label_selected = gcnew System::Windows::Forms::Label();
			this->listBox_selected = gcnew System::Windows::Forms::ListBox();
			this->label_status = gcnew System::Windows::Forms::Label();
			this->tabControl_system->SuspendLayout();
			this->tabPage_available->SuspendLayout();
			this->tabPage_placed->SuspendLayout();
			this->tabPage_system->SuspendLayout();
			this->tabPage_notes->SuspendLayout();
			this->tabPage_selected->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_system
			// 
			this->tabControl_system->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_system->Controls->Add(this->tabPage_available);
			this->tabControl_system->Controls->Add(this->tabPage_placed);
			this->tabControl_system->Controls->Add(this->tabPage_system);
			this->tabControl_system->Controls->Add(this->tabPage_notes);
			this->tabControl_system->Controls->Add(this->tabPage_selected);
			this->tabControl_system->Location = System::Drawing::Point(8, 8);
			this->tabControl_system->Multiline = true;
			this->tabControl_system->Name = "tabControl_system";
			this->tabControl_system->SelectedIndex = 0;
			this->tabControl_system->Size = System::Drawing::Size(368, 376);
			this->tabControl_system->TabIndex = 0;
			// 
			// tabPage_available
			// 
			this->tabPage_available->Controls->Add(this->button_available_openpartial);
			this->tabPage_available->Controls->Add(this->button_available_collapseall);
			this->tabPage_available->Controls->Add(this->button_available_openall);
			this->tabPage_available->Controls->Add(this->button_add);
			this->tabPage_available->Controls->Add(this->button_refreshavailable);
			this->tabPage_available->Controls->Add(this->treeView_available);
			this->tabPage_available->Location = System::Drawing::Point(4, 22);
			this->tabPage_available->Name = "tabPage_available";
			this->tabPage_available->Size = System::Drawing::Size(360, 350);
			this->tabPage_available->TabIndex = 0;
			this->tabPage_available->Text = "Available";
			// 
			// button_available_openpartial
			// 
			this->button_available_openpartial->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_available_openpartial->Location = System::Drawing::Point(40, 323);
			this->button_available_openpartial->Name = "button_available_openpartial";
			this->button_available_openpartial->Size = System::Drawing::Size(16, 16);
			this->button_available_openpartial->TabIndex = 5;
			this->button_available_openpartial->Text = ":";
			this->button_available_openpartial->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_available_openpartial_Click);
			// 
			// button_available_collapseall
			// 
			this->button_available_collapseall->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_available_collapseall->Location = System::Drawing::Point(24, 323);
			this->button_available_collapseall->Name = "button_available_collapseall";
			this->button_available_collapseall->Size = System::Drawing::Size(16, 16);
			this->button_available_collapseall->TabIndex = 4;
			this->button_available_collapseall->Text = "-";
			this->button_available_collapseall->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_available_collapseall_Click);
			// 
			// button_available_openall
			// 
			this->button_available_openall->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_available_openall->Location = System::Drawing::Point(8, 323);
			this->button_available_openall->Name = "button_available_openall";
			this->button_available_openall->Size = System::Drawing::Size(16, 16);
			this->button_available_openall->TabIndex = 3;
			this->button_available_openall->Text = "+";
			this->button_available_openall->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_available_openall_Click);
			// 
			// button_add
			// 
			this->button_add->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_add->Location = System::Drawing::Point(88, 320);
			this->button_add->Name = "button_add";
			this->button_add->TabIndex = 2;
			this->button_add->Text = "Add";
			this->button_add->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_add_Click);
			// 
			// button_refreshavailable
			// 
			this->button_refreshavailable->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_refreshavailable->Location = System::Drawing::Point(176, 320);
			this->button_refreshavailable->Name = "button_refreshavailable";
			this->button_refreshavailable->TabIndex = 1;
			this->button_refreshavailable->Text = "Refresh";
			this->button_refreshavailable->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_refreshavailable_Click);
			// 
			// treeView_available
			// 
			this->treeView_available->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_available->HideSelection = false;
			this->treeView_available->ImageIndex = -1;
			this->treeView_available->Location = System::Drawing::Point(8, 8);
			this->treeView_available->Name = "treeView_available";
			this->treeView_available->SelectedImageIndex = -1;
			this->treeView_available->Size = System::Drawing::Size(344, 304);
			this->treeView_available->Sorted = true;
			this->treeView_available->TabIndex = 0;
			this->treeView_available->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &cmmSystemForm::treeView_available_MouseDown);
			this->treeView_available->Click += gcnew System::EventHandler(this, &cmmSystemForm::treeView_available_Click);
			this->treeView_available->DoubleClick += gcnew System::EventHandler(this, &cmmSystemForm::treeView_available_DoubleClick);
			this->treeView_available->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &cmmSystemForm::treeView_available_AfterSelect);
			// 
			// tabPage_placed
			// 
			this->tabPage_placed->Controls->Add(this->button_reload);
			this->tabPage_placed->Controls->Add(this->button_placed_openpartial);
			this->tabPage_placed->Controls->Add(this->button_placed_collapseall);
			this->tabPage_placed->Controls->Add(this->button_placed_openall);
			this->tabPage_placed->Controls->Add(this->button_duplicate);
			this->tabPage_placed->Controls->Add(this->button_edit);
			this->tabPage_placed->Controls->Add(this->button_delete);
			this->tabPage_placed->Controls->Add(this->treeView_placed);
			this->tabPage_placed->Location = System::Drawing::Point(4, 22);
			this->tabPage_placed->Name = "tabPage_placed";
			this->tabPage_placed->Size = System::Drawing::Size(360, 350);
			this->tabPage_placed->TabIndex = 1;
			this->tabPage_placed->Text = "Placed";
			// 
			// button_reload
			// 
			this->button_reload->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_reload->Location = System::Drawing::Point(208, 320);
			this->button_reload->Name = "button_reload";
			this->button_reload->Size = System::Drawing::Size(64, 23);
			this->button_reload->TabIndex = 9;
			this->button_reload->Text = "Reload";
			this->button_reload->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_reload_Click);
			// 
			// button_placed_openpartial
			// 
			this->button_placed_openpartial->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_placed_openpartial->Enabled = false;
			this->button_placed_openpartial->Location = System::Drawing::Point(40, 323);
			this->button_placed_openpartial->Name = "button_placed_openpartial";
			this->button_placed_openpartial->Size = System::Drawing::Size(16, 16);
			this->button_placed_openpartial->TabIndex = 8;
			this->button_placed_openpartial->Text = ":";
			this->button_placed_openpartial->Visible = false;
			this->button_placed_openpartial->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_placed_openpartial_Click);
			// 
			// button_placed_collapseall
			// 
			this->button_placed_collapseall->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_placed_collapseall->Location = System::Drawing::Point(24, 323);
			this->button_placed_collapseall->Name = "button_placed_collapseall";
			this->button_placed_collapseall->Size = System::Drawing::Size(16, 16);
			this->button_placed_collapseall->TabIndex = 7;
			this->button_placed_collapseall->Text = "-";
			this->button_placed_collapseall->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_placed_collapseall_Click);
			// 
			// button_placed_openall
			// 
			this->button_placed_openall->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_placed_openall->Location = System::Drawing::Point(8, 323);
			this->button_placed_openall->Name = "button_placed_openall";
			this->button_placed_openall->Size = System::Drawing::Size(16, 16);
			this->button_placed_openall->TabIndex = 6;
			this->button_placed_openall->Text = "+";
			this->button_placed_openall->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_placed_openall_Click);
			// 
			// button_duplicate
			// 
			this->button_duplicate->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_duplicate->Location = System::Drawing::Point(136, 320);
			this->button_duplicate->Name = "button_duplicate";
			this->button_duplicate->Size = System::Drawing::Size(64, 23);
			this->button_duplicate->TabIndex = 4;
			this->button_duplicate->Text = "Duplicate";
			this->button_duplicate->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_duplicate_Click);
			// 
			// button_edit
			// 
			this->button_edit->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_edit->Location = System::Drawing::Point(64, 320);
			this->button_edit->Name = "button_edit";
			this->button_edit->Size = System::Drawing::Size(64, 23);
			this->button_edit->TabIndex = 3;
			this->button_edit->Text = "Edit";
			this->button_edit->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_edit_Click);
			// 
			// button_delete
			// 
			this->button_delete->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_delete->Location = System::Drawing::Point(280, 320);
			this->button_delete->Name = "button_delete";
			this->button_delete->Size = System::Drawing::Size(64, 23);
			this->button_delete->TabIndex = 2;
			this->button_delete->Text = "Delete";
			this->button_delete->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_delete_Click);
			// 
			// treeView_placed
			// 
			this->treeView_placed->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_placed->CheckBoxes = true;
			this->treeView_placed->HideSelection = false;
			this->treeView_placed->ImageIndex = -1;
			this->treeView_placed->Location = System::Drawing::Point(8, 8);
			this->treeView_placed->Name = "treeView_placed";
			this->treeView_placed->SelectedImageIndex = -1;
			this->treeView_placed->Size = System::Drawing::Size(344, 304);
			this->treeView_placed->Sorted = true;
			this->treeView_placed->TabIndex = 0;
			this->treeView_placed->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &cmmSystemForm::treeView_placed_KeyDown);
			this->treeView_placed->MouseDown += gcnew System::Windows::Forms::MouseEventHandler(this, &cmmSystemForm::treeView_placed_MouseDown);
			this->treeView_placed->Click += gcnew System::EventHandler(this, &cmmSystemForm::treeView_placed_Click);
			this->treeView_placed->AfterCheck += gcnew System::Windows::Forms::TreeViewEventHandler(this, &cmmSystemForm::treeView_placed_AfterCheck);
			this->treeView_placed->DoubleClick += gcnew System::EventHandler(this, &cmmSystemForm::treeView_placed_DoubleClick);
			this->treeView_placed->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &cmmSystemForm::treeView_placed_AfterSelect);
			// 
			// tabPage_system
			// 
			this->tabPage_system->Controls->Add(this->button_systemtree_execute);
			this->tabPage_system->Controls->Add(this->treeView_system);
			this->tabPage_system->Location = System::Drawing::Point(4, 22);
			this->tabPage_system->Name = "tabPage_system";
			this->tabPage_system->Size = System::Drawing::Size(360, 350);
			this->tabPage_system->TabIndex = 2;
			this->tabPage_system->Text = "System";
			// 
			// button_systemtree_execute
			// 
			this->button_systemtree_execute->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->button_systemtree_execute->Location = System::Drawing::Point(8, 320);
			this->button_systemtree_execute->Name = "button_systemtree_execute";
			this->button_systemtree_execute->TabIndex = 1;
			this->button_systemtree_execute->Text = "Execute";
			this->button_systemtree_execute->Click += gcnew System::EventHandler(this, &cmmSystemForm::button_systemtree_execute_Click);
			// 
			// treeView_system
			// 
			this->treeView_system->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_system->HideSelection = false;
			this->treeView_system->ImageIndex = -1;
			this->treeView_system->Location = System::Drawing::Point(8, 8);
			this->treeView_system->Name = "treeView_system";
			this->treeView_system->SelectedImageIndex = -1;
			this->treeView_system->Size = System::Drawing::Size(344, 304);
			this->treeView_system->TabIndex = 0;
			this->treeView_system->DoubleClick += gcnew System::EventHandler(this, &cmmSystemForm::treeView_system_DoubleClick);
			// 
			// tabPage_notes
			// 
			this->tabPage_notes->Controls->Add(this->label_notes);
			this->tabPage_notes->Controls->Add(this->text_notes);
			this->tabPage_notes->Location = System::Drawing::Point(4, 22);
			this->tabPage_notes->Name = "tabPage_notes";
			this->tabPage_notes->Size = System::Drawing::Size(360, 350);
			this->tabPage_notes->TabIndex = 3;
			this->tabPage_notes->Text = "Notes";
			// 
			// label_notes
			// 
			this->label_notes->Location = System::Drawing::Point(7, 14);
			this->label_notes->Name = "label_notes";
			this->label_notes->Size = System::Drawing::Size(81, 21);
			this->label_notes->TabIndex = 2;
			this->label_notes->Text = "Scene Notes";
			// 
			// text_notes
			// 
			this->text_notes->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->text_notes->Location = System::Drawing::Point(8, 37);
			this->text_notes->Multiline = true;
			this->text_notes->Name = "text_notes";
			this->text_notes->Size = System::Drawing::Size(343, 306);
			this->text_notes->TabIndex = 1;
			this->text_notes->Text = "";
			this->text_notes->TextChanged += gcnew System::EventHandler(this, &cmmSystemForm::text_notes_TextChanged);
			// 
			// tabPage_selected
			// 
			this->tabPage_selected->Controls->Add(this->label_selected);
			this->tabPage_selected->Controls->Add(this->listBox_selected);
			this->tabPage_selected->Location = System::Drawing::Point(4, 22);
			this->tabPage_selected->Name = "tabPage_selected";
			this->tabPage_selected->Size = System::Drawing::Size(360, 350);
			this->tabPage_selected->TabIndex = 4;
			this->tabPage_selected->Text = "Selected";
			// 
			// label_selected
			// 
			this->label_selected->Location = System::Drawing::Point(8, 8);
			this->label_selected->Name = "label_selected";
			this->label_selected->Size = System::Drawing::Size(264, 23);
			this->label_selected->TabIndex = 1;
			this->label_selected->Text = "History of Selected Objects";
			// 
			// listBox_selected
			// 
			this->listBox_selected->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_selected->Location = System::Drawing::Point(8, 32);
			this->listBox_selected->Name = "listBox_selected";
			this->listBox_selected->Size = System::Drawing::Size(344, 303);
			this->listBox_selected->TabIndex = 0;
			this->listBox_selected->SelectedIndexChanged += gcnew System::EventHandler(this, &cmmSystemForm::listBox_selected_SelectedIndexChanged);
			// 
			// label_status
			// 
			this->label_status->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->label_status->Location = System::Drawing::Point(8, 392);
			this->label_status->Name = "label_status";
			this->label_status->Size = System::Drawing::Size(368, 23);
			this->label_status->TabIndex = 1;
			this->label_status->Text = "Status:";
			// 
			// cmmSystemForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(384, 422);
			this->Controls->Add(this->label_status);
			this->Controls->Add(this->tabControl_system);
			this->MinimumSize = System::Drawing::Size(256, 176);
			this->Name = "cmmSystemForm";
			this->Text = "Scene";
			this->Load += gcnew System::EventHandler(this, &cmmSystemForm::cmmSystemForm_Load);
			this->Closed += gcnew System::EventHandler(this, &cmmSystemForm::cmmSystemForm_Closed);
			this->tabControl_system->ResumeLayout(false);
			this->tabPage_available->ResumeLayout(false);
			this->tabPage_placed->ResumeLayout(false);
			this->tabPage_system->ResumeLayout(false);
			this->tabPage_notes->ResumeLayout(false);
			this->tabPage_selected->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void cmmSystemForm_Load(System::Object ^  sender, System::EventArgs ^  e)
			 {
				//m_pMemory = gcnew tmaDialogMemory( this );

				//cmmSystemDialogUtil::SceneDialogOpen();

				Update();
			 }

	private: System::Void cmmSystemForm_Closed(System::Object ^  sender, System::EventArgs ^  e)
			{
				cmmSystemDialogUtil::SceneDialogClose();
			}

private: System::Void button_refreshavailable_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			buildtreeview_system_available();
		 }

private: System::Void button_add_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_available_buttons();

			add_objects();

			//	check again in case the system that added the object doesn't allow any
			//	more adds.
			enable_available_buttons();
		 }

private: System::Void treeView_available_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_bOurChange)
				return;
		 }

private: System::Void treeView_available_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_available_buttons();

			// make sure the user selected a childless node.
			//
			int index = get_available_filename_index();
			if (index >= 0)
			{
				add_object();
			}
			else
			{
				// open up the whole tree below this item
				TreeNode^ pTN = treeView_available->SelectedNode;
				if (pTN != nullptr)
				{
					if (pTN->IsExpanded)
						pTN->ExpandAll();
					//else
					///	pTN->Collapse();
				}
			}
		 }

private: System::Void button_delete_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_placed_buttons();
			delete_placed_objects();
		 }

private: System::Void treeView_placed_KeyDown(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
		 {
			 //	if delete key is pressed and it is okay for the user to delete this object
			 //
			 if ((e->KeyCode == Keys::Delete) && (button_delete->Enabled))
			 {
				enable_placed_buttons();
				delete_placed_objects();
			 }
		 }

private: System::Void button_edit_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_placed_buttons();
			edit_placed_object();
		 }

private: System::Void button_duplicate_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_placed_buttons();
			duplicate_placed_objects();
		 }

private: System::Void button_reload_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			enable_placed_buttons();
			reload_placed_object();
		 }

private: System::Void button_available_openall_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			treeView_available->ExpandAll();
		 }

private: System::Void button_available_collapseall_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			treeView_available->CollapseAll();
		 }

private: System::Void button_available_openpartial_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (treeView_available->SelectedNode == nullptr)
			 {
				 availabletree_openpartial_general();
			 }
			 else
			 {
				 availabletree_openpartial_node();
			 }
		 }

private: System::Void treeView_available_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			// make sure the user selected a childless node.
			//
			enable_available_buttons();

			// display name for test
			if ( treeView_available->SelectedNode != nullptr )
			{
				label_status->Text = treeView_available->SelectedNode->Text;
			}
		 }

private: System::Void button_placed_openall_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			treeView_placed->ExpandAll();
		 }

private: System::Void button_placed_collapseall_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			treeView_placed->CollapseAll();
		 }

private: System::Void button_placed_openpartial_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
		 }

private: System::Void treeView_placed_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			//treeView_placed->CollapseAll();
			//
			//IEnumerator ^ myNodes = (safe_cast<IEnumerable^>(treeView_placed->Nodes))->GetEnumerator();
			//try
			//{
			//	while (myNodes->MoveNext())
			//	{
			//		TreeNode ^ n = safe_cast<TreeNode^>(myNodes->Current);
			//		n->Expand();
			//	}
			//}
			//__finally
			//{
			//	IDisposable ^ disposable = dynamic_cast<System::IDisposable^>(myNodes);
			//	if (disposable != 0) disposable->Dispose();
			//}
		 }

private: System::Void treeView_placed_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 // edit isn't as important now that every selection alters the object dialog
			//edit_placed_object();

			 // Double-click in placed tree control change to "activate"
			 int index = get_placed_selected_index();
			 if (index >= 0)
			 {
				 cmmDialogInterestMgr::ActivateObject((*m_pDataListPlaced)[index].m_Name, (*m_pDataListPlaced)[index].m_SystemName);
			 }
		 }

private: System::Void treeView_placed_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			enable_placed_buttons();
			if (!m_bOurChange)
				select_placed_object();

			// display name for test
			if ( treeView_placed->SelectedNode != nullptr )
			{
				label_status->Text = treeView_placed->SelectedNode->Text;
			}
		 }

private: System::Void text_notes_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (!m_bDisableNotify) 
			{
				std::string name;
				tmaManagedStringUtils::ManagedStringToStdString( text_notes->Text, name );

				SceneSetupData& data = SceneSetupDialogUtil::Data();
				data.m_PropertiesData.m_Notes = name;

				SceneSetupDocumentChunk::GetActiveChunk()->DataChanged();

				//DBG_LOG1("Notes: (%s)", data.m_PropertiesData.m_Notes.c_str());
			}
		 }

private: System::Void treeView_placed_AfterCheck(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			 TreeNode^ pSN = e->Node;
			if ( pSN != nullptr )
			{
				std::string name;
				bool bCheckState = pSN->Checked;
				if (pSN->GetNodeCount(false) > 0)
				{
					// This is a label node, so make all the children's visibility checks match this one
					//
					int childnodes = pSN->GetNodeCount(false);
					for (int i=0; i< childnodes; ++i)
					{
						bool b_childcheck = pSN->Nodes[i]->Checked;
						if ( b_childcheck != bCheckState)
						{
							pSN->Nodes[i]->Checked = bCheckState;

							std::string name;
							extract_valid_name(pSN->Nodes[i]->Text, nullptr, name);
							visMgr::SetVisibleInEditor(name, bCheckState);
						}
					}
				}
				else
				{
					std::string name;
					extract_valid_name(pSN->Text, nullptr, name);

					// A single node's check has changed
					visMgr::SetVisibleInEditor(name, bCheckState);
				}
			}
		 }

private: System::Void treeView_available_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		 {
			m_pSelectedTreeNode = treeView_available->GetNodeAt(e->X, e->Y);
		 }

private: System::Void treeView_placed_MouseDown(System::Object ^  sender, System::Windows::Forms::MouseEventArgs ^  e)
		 {
			m_pSelectedTreeNode = treeView_placed->GetNodeAt(e->X, e->Y);
		 }

private: System::Void treeView_system_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			execute_systemtree_select();
		 }


private: System::Void listBox_selected_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_bOurChange)
				 return;

			 m_bOurChange = true;

			 //	select the object
			 //
			 int sel_ind = listBox_selected->SelectedIndex;
			 if (sel_ind >= 0) 
			 {
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::SelectIndexFromPickList(sel_ind);
			 }

			 m_bOurChange = false;
		 }

private: System::Void button_systemtree_execute_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			execute_systemtree_select();
		 }


	    //--------------------------------------------------------------------
		//--------------------------------------------------------------------
private: void execute_systemtree_select()
		 {
			if ( treeView_system->SelectedNode != nullptr )
			{
				if (treeView_system->SelectedNode->Tag != nullptr)
				{
					int cmd_ID;
					std::string cmd_tag;

					get_objecttreenode_tag( dynamic_cast<String^>(treeView_system->SelectedNode->Tag), 
											cmd_tag, 
											cmd_ID );

					cmaCommandMgr::ExecuteCommand(cmd_tag, cmd_ID);
				}
			}
		 }

private: int get_placed_selected_index()
		 {
			// make sure the user selected a childless node.
			//
			if ( treeView_placed->SelectedNode != nullptr )
			{
				int nodecount = treeView_placed->SelectedNode->GetNodeCount(true);
				if ( nodecount == 0 )
				{
					// Get name of parent node (this will be system name)
					System::String ^sys_name = nullptr;
					if (treeView_placed->SelectedNode->Parent)
						sys_name = treeView_placed->SelectedNode->Parent->Text;

					std::string name;
					int valid_index = extract_valid_name(treeView_placed->SelectedNode->Text, sys_name, name);

					return valid_index;
				}
			}
			return -1;
		 }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
private: void enable_available_buttons()
		 {
			// make sure the user selected a childless node.
			//
			int index = get_available_filename_index();
			if (index >= 0)
			{
				// do enable/disable of buttons
				//
				fsLocator file_path;
				m_pFileListAvailable->GetFilePath(index, file_path);
				int interactions = cmmDialogInterestMgr::GetAllowedInteractions( file_path );
				button_add->Enabled		= ((interactions & e_DIAllow_Add) != 0);
			}
			else
			{
				button_add->Enabled = false;
			}
		 }

private: void enable_placed_buttons()
		 {
			// make sure the user selected a childless node.
			//
			int index = get_placed_selected_index();
			if (index >= 0)
			{
				// do enable/disable of buttons
				//
				fsLocator path;
				path.Push((*m_pDataListPlaced)[index].m_SystemName.c_str());
				int interactions = cmmDialogInterestMgr::GetAllowedInteractions( path );
				button_delete->Enabled	= ((interactions & e_DIAllow_Del) != 0);
				button_edit->Enabled	= ((interactions & e_DIAllow_Edit) != 0);
				button_duplicate->Enabled = ((interactions & e_DIAllow_Dupe) != 0);
				button_reload->Enabled = ((interactions & e_DIAllow_Reload) != 0);
			}
			else
			{
				button_delete->Enabled = false;
				button_edit->Enabled = false;
				button_duplicate->Enabled = false;
				button_reload->Enabled = false;
			}
		 }

private: void edit_placed_object()
		 {
			 int index = get_placed_selected_index();
			 if (index >= 0)
			 {
				 cmmDialogInterestMgr::SelectObject((*m_pDataListPlaced)[index].m_Name, (*m_pDataListPlaced)[index].m_SystemName);
				 cmmObjectDialogUtil::Show();
			 }
		 }

private: void delete_placed_objects()
		 {
			 //	Get all the names into a list
			 ArrayList^ names = gcnew ArrayList;
			 ArrayList^ sys_names = gcnew ArrayList;
			IEnumerator^ myEnum = this->treeView_placed->SelectedNodes->GetEnumerator();
            while (myEnum->MoveNext())
            {
                TreeNode^ node = (TreeNode^)(myEnum->Current);
				names->Add(node->Text);
				
				// Get name of parent node (this will be system name)
				if (node->Parent)
					sys_names->Add(node->Parent->Text);
				else
					sys_names->Add(nullptr);
			}

			if (names->Count > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			//	remove the objects one at a time.
			for (int i=0; i < names->Count; ++i)
			{
				std::string name;
				int valid_index = extract_valid_name((String^)names[i], (String^)sys_names[i], name);
				if (valid_index >= 0)
				{
					cmmDialogInterestMgr::DeleteObject((*m_pDataListPlaced)[valid_index].m_Name, (*m_pDataListPlaced)[valid_index].m_SystemName);
				}
            }

			if (names->Count > 1)
				undoUndoMgr::EndMultipleOperationBlock();
		 }

private: void duplicate_placed_objects()
		 {
			//int index = get_placed_selected_index();
			//if (index >= 0)
			//{
			//	cmmDialogInterestMgr::DuplicateObject((*m_pDataListPlaced)[index].m_Name, (*m_pDataListPlaced)[index].m_SystemName);
			//}

			//	Get all the names into a list
			//
			ArrayList^ names = gcnew ArrayList;
			ArrayList^ sys_names = gcnew ArrayList;
			IEnumerator^ myEnum = this->treeView_placed->SelectedNodes->GetEnumerator();
            while (myEnum->MoveNext())
            {
                TreeNode^ node = (TreeNode^)(myEnum->Current);
				names->Add(node->Text);
				
				// Get name of parent node (this will be system name)
				if (node->Parent)
					sys_names->Add(node->Parent->Text);
				else
					sys_names->Add(nullptr);
			}

			if (names->Count > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			//	remove the objects one at a time.
			//
			for (int i=0; i < names->Count; ++i)
			{
				std::string name;
				int valid_index = extract_valid_name((String^)names[i], (String^)sys_names[i], name);
				if (valid_index >= 0)
				{
					cmmDialogInterestMgr::DuplicateObject((*m_pDataListPlaced)[valid_index].m_Name, (*m_pDataListPlaced)[valid_index].m_SystemName);
				}
            }

			if (names->Count > 1)
				undoUndoMgr::EndMultipleOperationBlock();
		 }

private: void reload_placed_object()
		 {
			 int index = get_placed_selected_index();
			 if (index >= 0)
			 {
				 cmmDialogInterestMgr::ReloadObject((*m_pDataListPlaced)[index].m_Name, (*m_pDataListPlaced)[index].m_SystemName);
			 }
		 }

private: void select_placed_object(int i_Index)
		 {
			//DBG_LOG3("Selected Object [%d] (%s) (%s)",  i_Index,
			//											(*m_pDataListPlaced)[i_Index].m_Name.GetString().c_str(), 
			//											(*m_pDataListPlaced)[i_Index].m_SystemName.c_str() );

			 //	select the object
			 //
			 bool bCtrlPressed = (bool)((this->treeView_placed->ModifierKeys & Keys::Control) != Keys::None);
			 if (bCtrlPressed)
			 {
				cmmDialogInterestMgr::DeselectObject((*m_pDataListPlaced)[i_Index].m_Name, 
					(*m_pDataListPlaced)[i_Index].m_SystemName);
			 }
			 else
			 {
				bool bShiftPressed = (bool)((this->treeView_placed->ModifierKeys & Keys::Shift) != Keys::None);
				cmmDialogInterestMgr::SelectObject((*m_pDataListPlaced)[i_Index].m_Name, 
					(*m_pDataListPlaced)[i_Index].m_SystemName,
					bShiftPressed);
			 }
		 }
private: void select_placed_object()
		 {
			 int index = get_placed_selected_index();
			 if (index >= 0)
			 {
				 select_placed_object(index);
			 }
		 }

private: void buildtreeview_system_available()
		{
			cmmDialogInterestMgr::GetAvailableObjects( *m_pFileListAvailable );

			//m_bDisableNotify = true;
			treeView_available->BeginUpdate();

			treeView_available->Nodes->Clear();
			gsupTreeViewUtil::PopulateTreeView( treeView_available, *m_pFileListAvailable, false );

			treeView_available->EndUpdate();
			//m_bDisableNotify = false;
		}

		 
private: TreeNode^ create_and_flag_node(String^ i_Name, 
										TreeNodeCollection^ i_AddToCollection, 
										bool i_bChecked)
		 {
			TreeNode^ pTreeNode = gcnew TreeNode( i_Name );
			pTreeNode->Checked = i_bChecked;
			gsupTreeViewUtil::FlagTreeNode(pTreeNode, true);
			i_AddToCollection->Add( pTreeNode );
			return pTreeNode;
		 }

		 
private: TreeNode^ find_or_create_node(String^ i_Name, 
										TreeNode^ i_ParentNode, 
										bool i_bChecked)
		 {
			TreeNode^ pCTN = gsupTreeViewUtil::FindTreeNode(i_ParentNode->Nodes, i_Name);
			if (pCTN == nullptr)
			{
				pCTN = create_and_flag_node(i_Name, i_ParentNode->Nodes, i_bChecked);
				i_ParentNode->ExpandAll();
			}
			else
			{
				// already found it
				gsupTreeViewUtil::FlagTreeNode(pCTN, true);
			}
			return pCTN;
		 }

private: void updatetreeview_system_placed()
		 {
			// Go through the tree and update ONLY those nodes that need to be changed
			//	(i.e. added or deleted)
			//
			//DBG_LOG0("::::::UpdateTreeView_System_PLACED:::::");

			cmmDialogInterestMgr::SortList(*m_pDataListPlaced);

			treeView_placed->BeginUpdate();

			//DBG_LOG1("DataList Place size = %d [utv_s_p]", m_pDataListPlaced->size());
			cmmDialogDataList::iterator it;

			// mark all the tree as "untouched"
			gsupTreeViewUtil::FlagTreeNodes( treeView_placed, false );

			//	go through the tree and add new nodes
			//
			for(it = m_pDataListPlaced->begin(); it != m_pDataListPlaced->end(); it++)
			{
				//DBG_LOG2(" ~data list (%s)-(%s) [utv_s_p]", (*it).m_SystemName.c_str(), (*it).m_Name.GetString().c_str());
				String^ name = gcnew String((*it).m_SystemName.c_str());
				TreeNode^ pTN = gsupTreeViewUtil::FindTreeNode(treeView_placed->Nodes, name);
				if (!pTN)
				{
					// TODO set this based on the children nodes checked state
					const bool bChecked = true;
					pTN = create_and_flag_node(name, treeView_placed->Nodes, bChecked);
					pTN->BackColor = System::Drawing::Color::LightGray;
					pTN->ForeColor = System::Drawing::Color::Black;
				}

				//	build the string the user sees
				String^ str1 = gcnew String((*it).m_Name.GetString().c_str());
				String^ str2 = gcnew String((*it).m_Desc.c_str());
				String^ objname = String::Format("{0} - {1}", str1, str2 );
				TreeNode^ pCTN = find_or_create_node(objname, pTN, (*it).m_bVisible);

				// Display categories of object parts
				//std::map<std::string, std::vector<std::string> >::iterator cat_it;
				//for (cat_it = it->m_Parts.begin(); cat_it != it->m_Parts.end(); ++cat_it)
				//{
				//	// Not sure how to handle check state of parts
				//	const bool bChecked = true;
				//	String^ catname = gcnew String(cat_it->first.c_str());
				//	TreeNode^ pCategoryNode = find_or_create_node(catname, pCTN, bChecked);
				//	
				//	std::vector<std::string>::iterator part_it;
				//	for (part_it = cat_it->second.begin(); part_it != cat_it->second.end(); ++part_it)
				//	{
				//		String^ partname = gcnew String(part_it->c_str());
				//		find_or_create_node(partname, pCategoryNode, bChecked);
				//	}
				//}
			}
			//DBG_LOG0(" ~~");

			//	remove the "untouched" nodes
			gsupTreeViewUtil::RemoveUnflaggedTreeNodes(treeView_placed);

			//	sort and end this update
			gsupTreeViewUtil::SortTreeView( treeView_placed, true, true );

			treeView_placed->EndUpdate();
		 }

private: void buildtreeview_system_placed()
		{
			//	Call the mgr to get all placed objects in the world first
			//
			m_pDataListPlaced->clear();
			cmmDialogInterestMgr::GetPlacedObjects(*m_pDataListPlaced);
			updatetreeview_system_placed();
		}

private: int get_available_filename_index(TreeNode^ i_pTreeNode)
		 {
			// make sure the user selected a childless node.
			//
			if ( i_pTreeNode != nullptr )
			{
				if ( i_pTreeNode->GetNodeCount(true) == 0 )
				{
					itString fname;
					tmaManagedStringUtils::ManagedStringToItString(i_pTreeNode->Text, fname );

					int index = m_pFileListAvailable->GetIndex(fname);
					return index;
				}
			}
			return -1;
		 }

private: int get_available_filename_index()
		 {
			 return get_available_filename_index(treeView_available->SelectedNode);
		 }

private: void add_object()
		 {
			// make sure the user selected a childless node.
			//
			int index = get_available_filename_index();
			if (index >= 0)
			{
				itString fname = m_pFileListAvailable->GetFilename(index);
				fsLocator path;
				m_pFileListAvailable->GetFilePath(index, path);
				cmmDialogInterestMgr::AddObject(fname, path);

				// if the preferences flag is set, automatically jump to the placed tab
				//
				PrefsData& data = PrefsMgr::Data();
				if ( data.m_bOnAddSwitchToPlacedTab.GetValue() )
				{
					//	select the placed tab once all the adds are completed
					//
					this->tabControl_system->SelectedTab = this->tabPage_placed;
				}
			}
		 }

private: void add_objects()
		 {
			 if (treeView_available->SelectedNodes->Count > 1)
				undoUndoMgr::BeginMultipleOperationBlock();

			//	Get all the names into a list
			 IEnumerator^ myEnum = this->treeView_available->SelectedNodes->GetEnumerator();
            while (myEnum->MoveNext())
            {
                TreeNode^ node = (TreeNode^)(myEnum->Current);
				int index = get_available_filename_index(node);
				if (index >= 0)
				{
					itString fname = m_pFileListAvailable->GetFilename(index);
					fsLocator path;
					m_pFileListAvailable->GetFilePath(index, path);
					cmmDialogInterestMgr::AddObject(fname, path);
				}
			}

			if (treeView_available->SelectedNodes->Count > 1)
				undoUndoMgr::EndMultipleOperationBlock();

			// if the preferences flag is set, automatically jump to the placed tab
			//
			PrefsData& data = PrefsMgr::Data();
			if ( data.m_bOnAddSwitchToPlacedTab.GetValue() )
			{
				//	select the placed tab once all the adds are completed
				//
				this->tabControl_system->SelectedTab = this->tabPage_placed;
			}
		 }


		void SetupData()
		{
			//if (m_bOurChange) return;
		}

		void availabletree_openpartial_general()
		{
			treeView_available->ExpandAll();
		}

		void availabletree_openpartial_node()
		{
			TreeNode^ pTN;
			//treeView_available->CollapseAll();

			pTN = treeView_available->SelectedNode;
			if (pTN == nullptr)
				return;

			pTN->ExpandAll();
		}

		//--------------------------------------------------------------------
		//	send back a substring of the passed in text using " - " as the 
		//	delimiter.  the return value is the character position of the 
		//	beginning of the delimiter.
		//--------------------------------------------------------------------
		int extract_name(String^ i_Text, System::Int16 charoffset, std::string& o_Name)
		{
			std::string name;
			tmaManagedStringUtils::ManagedStringToStdString(i_Text, name );
			int index = name.find(" - ",charoffset);
			o_Name = name.substr(0,index);
			//DBG_LOG2("name (%s), subname (%s)", name.c_str(), o_Name.c_str());
			return index;
		}

		//--------------------------------------------------------------------
		//	return a VALID name based on the passed in text.  The return 
		//	value will be the index of the valid name in the Data List.
		//--------------------------------------------------------------------
		int extract_valid_name(String^ i_ObjectName, String^ i_SystemName, std::string& o_Name)
		{
			System::Int16 coff = 0;
			std::string name;
			tmaManagedStringUtils::ManagedStringToStdString(i_ObjectName, name);
			std::string sys_name;
			tmaManagedStringUtils::ManagedStringToStdString(i_SystemName, sys_name);

			//  count the number of occurrences of the delimiter
			int delims = 0;
			while ((coff = name.find(" - ", coff)) != std::string::npos)
			{
				delims++;
				coff += 1;
			}

			//
			coff = 0;
			while (delims >= 0)
			{
				coff = extract_name(i_ObjectName, coff, o_Name);
				coff += 1;

				int count = -1;
				cmmDialogDataList::iterator it;
				for(it = m_pDataListPlaced->begin(); it != m_pDataListPlaced->end(); it++)
				{
					++count;
					//DBG_LOG3("%02d name (%s), subname (%s)", count, (*it).m_Name.GetString().c_str(), subname.c_str());
					if ((i_SystemName == nullptr) ||
						(it->m_SystemName == sys_name))
					{
						if (it->m_Name == nameString(o_Name))
						{
							return count;
						}
					}
				}

				--delims;
			}

			return -1;
		}

	private:
		String^ build_objecttreenode_tag( const std::string& i_CommandTag, System::Int16 i_CmdID )
		{
			String^ output_tag;
			output_tag = String::Format("{0}-{1}", i_CmdID.ToString(), gcnew String(i_CommandTag.c_str()));
			return output_tag;
		}
		void get_objecttreenode_tag( String^ i_Tag, std::string& o_CommandTag, int& o_CmdID )
		{
			std::string name;
			tmaManagedStringUtils::ManagedStringToStdString(i_Tag, name );
			int index = name.find("-");
			o_CommandTag = name.substr(index+1);
			o_CmdID = atoi(name.substr(0,index).c_str());
		}
		TreeNode^ find_node_by_pick_object(pick3dPickObject* i_pPickObject)
		{
			cmmDialogDataList::iterator it;
			for(it = m_pDataListPlaced->begin(); it != m_pDataListPlaced->end(); it++)
			{
				if (it->m_pPickObject == i_pPickObject)
				{
					// first, find the system name
					TreeNode ^pSystemNode = gsupTreeViewUtil::FindTreeNode( treeView_placed->Nodes, gcnew System::String(it->m_SystemName.c_str()) );
					if (pSystemNode != nullptr)
					{
						//	find the name in the tree
						//
						//	Note: add a space to prevent objects with similar names to cause
						//	problems.  (e.g. keylight and keylight2 -- if keylight2 is before
						//	keylight then keylight will not get found)
						//
						std::string find_name = it->m_Name.GetString();
						find_name += " ";
						return gsupTreeViewUtil::FindTreeNodeThatStartsWith( pSystemNode->Nodes, gcnew System::String(find_name.c_str()) );
					}
					break;
				}
			}
			return nullptr;
		}
private: void menuItem_ChangeColor(Object^ /*sender*/, System::EventArgs^ /*e*/) 
		{
			if (m_pSelectedTreeNode != nullptr)
			{
				System::Windows::Forms::ColorDialog^ dialog = gcnew System::Windows::Forms::ColorDialog();
				dialog->FullOpen = true;

				if (dialog->ShowDialog() == ::DialogResult::OK)
				{
					m_pSelectedTreeNode->BackColor = dialog->Color;
				}
			}
		}

private: void create_context_menu(System::Windows::Forms::Control^ i_pControl)
		{
			if (i_pControl->ContextMenu == nullptr)
			{
				System::Windows::Forms::ContextMenu ^mnuContextMenu = gcnew System::Windows::Forms::ContextMenu();
				MenuItem ^mnuItemNew = gcnew MenuItem();
				mnuItemNew->Text = "Set Color";
				mnuContextMenu->MenuItems->Add(mnuItemNew);
			    mnuItemNew->Click += gcnew System::EventHandler(this, &cmmSystemForm::menuItem_ChangeColor);
				i_pControl->ContextMenu = mnuContextMenu;
			}
		}

	private: fsysFileList*		m_pFileListAvailable;
	private: cmmDialogDataList*	m_pDataListPlaced;
	private: bool m_bDisableNotify;		// Turns off notify callbacks when setting up form
	private: bool m_bOurChange;			// Turns off setting of data fields when we make the change
	private: tmaDialogMemory^ m_pMemory;
	private: TreeNode^ m_pSelectedTreeNode;

};
}
#endif // _MANAGED
