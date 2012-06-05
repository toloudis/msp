#pragma once

#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef GSUP_TREEVIEWUTIL_HPP
#include "Support/gsup/gsupTreeViewUtil.hpp"
#endif

#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifndef TMA_MANAGEDCONTROLUTIL_HPP
#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
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
	/// Summary for chnlDriverSelectForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class chnlDriverSelectForm : public System::Windows::Forms::Form
	{
	public: 
		chnlDriverSelectForm(const tmlnDriverNameList &i_DriverNames)
			: m_Selection(-1), m_pDriverList(NULL)
		{
			InitializeComponent();

			BuildTreeView(i_DriverNames);
		}

		void ClearTreeView()
		{
			//treeView_drivers->Controls->Clear();
			tmaManagedControlUtil::DisposeOfControls(treeView_drivers->Controls);
		}
		void BuildTreeView(const tmlnDriverNameList &i_DriverNames)
		{
			m_Selection = -1;

			//	build the data list and then the tree view
			BuildFileList(i_DriverNames);
			m_ViewType = eHierarchy;
			BuildTreeView();
		}

		std::string* GetSelection() 
		{
			std::string* driver_name = new std::string;
			if ( m_pSelectedTreeNode != nullptr )
			{
				tmaManagedStringUtils::ManagedStringToStdString(m_pSelectedTreeNode->Text, *driver_name);
			}
			return driver_name;
		}

	protected: 
		~chnlDriverSelectForm()
		{
			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::Button ^  buttonCreate;

	private:
		int					m_Selection;
		fsysFileList*		m_pDriverList;
		tmaDialogMemory^	m_pMemory;
		gsupTreeViewType	m_ViewType;
		TreeNode^			m_pSelectedTreeNode;

	private: System::Windows::Forms::TabControl ^  tabControl_drivers;
	private: System::Windows::Forms::TabPage ^  tabPage_drivers;
	private: System::Windows::Forms::TreeView ^  treeView_drivers;
	private: System::Windows::Forms::Button ^  button_category;
	private: System::Windows::Forms::Button ^  button_sort;
	private: System::Windows::Forms::Label ^  label_display;
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
			this->buttonCreate = gcnew System::Windows::Forms::Button();
			this->tabControl_drivers = gcnew System::Windows::Forms::TabControl();
			this->tabPage_drivers = gcnew System::Windows::Forms::TabPage();
			this->label_display = gcnew System::Windows::Forms::Label();
			this->button_sort = gcnew System::Windows::Forms::Button();
			this->button_category = gcnew System::Windows::Forms::Button();
			this->treeView_drivers = gcnew System::Windows::Forms::TreeView();
			this->tabControl_drivers->SuspendLayout();
			this->tabPage_drivers->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonCreate
			// 
			this->buttonCreate->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->buttonCreate->Location = System::Drawing::Point(72, 237);
			this->buttonCreate->Name = "buttonCreate";
			this->buttonCreate->Size = System::Drawing::Size(127, 28);
			this->buttonCreate->TabIndex = 1;
			this->buttonCreate->Text = "Attach Driver";
			this->buttonCreate->Click += gcnew System::EventHandler(this, &chnlDriverSelectForm::buttonCreate_Click);
			// 
			// tabControl_drivers
			// 
			this->tabControl_drivers->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_drivers->Controls->Add(this->tabPage_drivers);
			this->tabControl_drivers->Location = System::Drawing::Point(0, 0);
			this->tabControl_drivers->Name = "tabControl_drivers";
			this->tabControl_drivers->SelectedIndex = 0;
			this->tabControl_drivers->Size = System::Drawing::Size(288, 296);
			this->tabControl_drivers->TabIndex = 2;
			// 
			// tabPage_drivers
			// 
			this->tabPage_drivers->AutoScroll = true;
			this->tabPage_drivers->Controls->Add(this->label_display);
			this->tabPage_drivers->Controls->Add(this->button_sort);
			this->tabPage_drivers->Controls->Add(this->button_category);
			this->tabPage_drivers->Controls->Add(this->treeView_drivers);
			this->tabPage_drivers->Controls->Add(this->buttonCreate);
			this->tabPage_drivers->Location = System::Drawing::Point(4, 22);
			this->tabPage_drivers->Name = "tabPage_drivers";
			this->tabPage_drivers->Size = System::Drawing::Size(280, 270);
			this->tabPage_drivers->TabIndex = 0;
			this->tabPage_drivers->Text = "Drivers";
			// 
			// label_display
			// 
			this->label_display->Location = System::Drawing::Point(8, 8);
			this->label_display->Name = "label_display";
			this->label_display->Size = System::Drawing::Size(64, 16);
			this->label_display->TabIndex = 3;
			this->label_display->Text = "Display by";
			// 
			// button_sort
			// 
			this->button_sort->Location = System::Drawing::Point(120, 5);
			this->button_sort->Name = "button_sort";
			this->button_sort->Size = System::Drawing::Size(48, 19);
			this->button_sort->TabIndex = 2;
			this->button_sort->Text = "A...Z";
			this->button_sort->Click += gcnew System::EventHandler(this, &chnlDriverSelectForm::button_sort_Click);
			// 
			// button_category
			// 
			this->button_category->Location = System::Drawing::Point(64, 5);
			this->button_category->Name = "button_category";
			this->button_category->Size = System::Drawing::Size(48, 19);
			this->button_category->TabIndex = 1;
			this->button_category->Text = "Group";
			this->button_category->Click += gcnew System::EventHandler(this, &chnlDriverSelectForm::button_category_Click);
			// 
			// treeView_drivers
			// 
			this->treeView_drivers->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_drivers->ImageIndex = -1;
			this->treeView_drivers->Location = System::Drawing::Point(0, 32);
			this->treeView_drivers->Name = "treeView_drivers";
			this->treeView_drivers->SelectedImageIndex = -1;
			this->treeView_drivers->Size = System::Drawing::Size(280, 200);
			this->treeView_drivers->TabIndex = 0;
			this->treeView_drivers->Click += gcnew System::EventHandler(this, &chnlDriverSelectForm::treeView_drivers_Click);
			this->treeView_drivers->DoubleClick += gcnew System::EventHandler(this, &chnlDriverSelectForm::treeView_drivers_DoubleClick);
			// 
			// chnlDriverSelectForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(296, 334);
			this->Controls->Add(this->tabControl_drivers);
			this->MinimumSize = System::Drawing::Size(256, 176);
			this->Name = "chnlDriverSelectForm";
			this->Text = "Create Driver";
			this->Load += gcnew System::EventHandler(this, &chnlDriverSelectForm::chnlDriverSelectForm_Load);
			this->Closed += gcnew System::EventHandler(this, &chnlDriverSelectForm::chnlDriverSelectForm_Closed);
			this->tabControl_drivers->ResumeLayout(false);
			this->tabPage_drivers->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void chnlDriverSelectForm_Load(System::Object ^  sender, System::EventArgs ^  e)
			 {
				m_pMemory = gcnew tmaDialogMemory( this );
			 }

private: System::Void chnlDriverSelectForm_Closed(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 delete m_pDriverList;
			 m_pDriverList = NULL;
		 }

	private: System::Void buttonCreate_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->DoSelection();
			 }

	private: System::Void listBox_drivers_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->DoSelection();
			 }

			 // Choose selected item in list and close dialog
	private : void DoSelection()
			  {
				this->m_pSelectedTreeNode = this->treeView_drivers->SelectedNode;
				if (this->m_pSelectedTreeNode != nullptr)
				{
					this->DialogResult = ::DialogResult::OK;
					this->Close();
				}
			  }

private: System::Void button_category_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
 			m_ViewType = eHierarchy;
			gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
		 }

private: System::Void button_sort_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			m_ViewType = eFlat;
			gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
		 }

private: System::Void treeView_drivers_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			this->m_pSelectedTreeNode = this->treeView_drivers->SelectedNode;
			if (this->m_pSelectedTreeNode != nullptr)
			{
				if (this->m_pSelectedTreeNode->Nodes->Count == 0)
				{
					buttonCreate->Enabled = true;
				}
				else
				{
					// not a valid driver
					buttonCreate->Enabled = false;
				}
			}
		 }

private: System::Void treeView_drivers_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (this->m_pSelectedTreeNode->Nodes->Count == 0)
			{
				this->DoSelection();
			}
		 }

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildFileList(const tmlnDriverNameList& i_DriverNames)
		{
			if (m_pDriverList)
				delete m_pDriverList;

			m_pDriverList = new fsysFileList();
			int num_items = i_DriverNames.m_DriverNames.size();
			for (int i=0; i<num_items; i++)
			{
				fsLocator category;
				category.Push("category");
				category.Push( i_DriverNames.m_DriverNames[i].m_Category.c_str() );
				m_pDriverList->AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildTreeView()
		{
			gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
		}
};
}
#endif // _MANAGED
