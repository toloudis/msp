#pragma once

#ifndef CMM_OBJECTDIALOGUTIL_HPP
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#endif

#ifndef TMLN_CREATOR_HPP
#include "Support/tmln/tmlnCreator.hpp"
#endif

//#ifndef CMA_MESSAGING_HPP
//#include "cmaMessaging.hpp"
//#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef GSUP_TREEVIEWUTIL_HPP
#include "Support/gsup/gsupTreeViewUtil.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
//#ifndef TMA_MANAGEDCONTROLUTIL_HPP
//#include "ToolUIManaged/tma/tmaManagedControlUtil.hpp"
//#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef PRTY_FORMCONTROLBUILDER_HPP
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
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
	/// Summary for cmmObjectForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmmObjectForm : public System::Windows::Forms::Form
	{
	public:
		static cmmObjectForm^ FormInstance = nullptr;

	public: 
		cmmObjectForm()
			: m_Selection(-1), m_pDriverList(NULL), m_pConnectDriverList(NULL)
		{
			InitializeComponent();

			//cmmObjectForm::FormInstance = this;
			m_pMemory = gcnew tmaDialogMemory( this );
		}

public:
		std::string* GetSelection() 
		{
			std::string* driver_name = new std::string;
			if ( m_pSelectedTreeNode != nullptr )
			{
				tmaManagedStringUtils::ManagedStringToStdString(m_pSelectedTreeNode->Text, *driver_name);
			}
			return driver_name;
		}

public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ClearTreeViews()
		{
			treeView_drivers->Nodes->Clear();
			treeView_connectdrivers->Nodes->Clear();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void ClearTabs()
		{
			ClearTreeViews();

			 //	clear out the controls 
			 prtyFormControlBuilder::ClearForm(this->tabPage_properties);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void SetObjectName( const std::string& i_Name )
		{
			this->label_objectname->Text = gcnew String(i_Name.c_str());
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Update(const tmlnDriverNameList &i_DriverNames)
		{
			ClearTreeViews();
			m_Selection = -1;

			//	build the data list and then the tree view
			BuildFileList(i_DriverNames);
			m_ViewType = eHierarchy;
			m_ViewTypeConnect = eFlat;
			BuildTreeViews();
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildTreeViews()
		{
			gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
			gsupTreeViewUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildFileList(const tmlnDriverNameList& i_DriverNames)
		{
			if (m_pDriverList)
				delete m_pDriverList;
			m_pDriverList = new fsysFileList();
			if (m_pConnectDriverList)
				delete m_pConnectDriverList;
			m_pConnectDriverList = new fsysFileList();

			int num_items = i_DriverNames.m_DriverNames.size();
			for (int i=0; i<num_items; i++)
			{
				fsLocator category;
				category.Push("category");
				category.Push( i_DriverNames.m_DriverNames[i].m_Category.c_str() );

				//	check to see if this is a "connect" driver.  If so, add to the connect
				//	list otherwise add it to the driver list.
				if (strstr(i_DriverNames.m_DriverNames[i].m_Name.c_str(), "Connect Channel") == 0)
				{
					m_pDriverList->AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
				}
				else
				{
					m_pConnectDriverList->AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
					m_pConnectDriverList->Sort();
				}
			}
		}

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TabPage^ GetDriverTab()
		{
			return tabPage_drivers;
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TabPage^ GetConnectDriverTab()
		{
			return tabPage_connect;
		}
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		TabPage^ GetPropertyTab()
		{
			return tabPage_properties;
		}

		//---------------------------------------------------------------------------
		//---------------------------------------------------------------------------
		void AddTabPage( TabPage^ i_pTabPage )
		{
			if (!this->tabControl_object->Controls->Contains(i_pTabPage))
			{
				this->tabControl_object->Controls->Add(i_pTabPage);
			}
		}
		
		//---------------------------------------------------------------------------
		//	Remove the tab page (by pointer)
		//---------------------------------------------------------------------------
		void RemoveTabPage( TabPage^ i_pTabPage )
		{
			if (this->tabControl_object->Controls->Contains(i_pTabPage))
			{
				this->tabControl_object->Controls->Remove(i_pTabPage);
			}
		}

		//--------------------------------------------------------------------
		// Returns true if the given tab page is attached.
		//--------------------------------------------------------------------
		bool HasTabPage(TabPage^ i_pTabPage)
		{
			return (this->tabControl_object->Controls->Contains(i_pTabPage));
		}

		//---------------------------------------------------------------------------
		//	SelectTab - by name
		//---------------------------------------------------------------------------
		void SelectTab( String^ i_Text )
		{
			if (i_Text == "Drivers")
			{
				tabControl_object->SelectedTab = tabPage_drivers;

				//	select the top of the tree
				if (this->treeView_drivers->Nodes->Count > 0)
				{
					TreeNode^ pTN = this->treeView_drivers->Nodes[0];
					this->treeView_drivers->SelectedNode = pTN;
				}
			}
			else if (i_Text == "Connect Drivers")
			{
				tabControl_object->SelectedTab = tabPage_connect;

				//	select the top of the tree
				if (this->treeView_connectdrivers->Nodes->Count > 0)
				{
					TreeNode^ pTN = this->treeView_connectdrivers->Nodes[0];
					this->treeView_connectdrivers->SelectedNode = pTN;
				}
			}
			else if (i_Text == "Properties")
			{
				tabControl_object->SelectedTab = tabPage_properties;
			}
			else
			{
				for (int i = 0; i < tabControl_object->Controls->Count; ++i)
				{
					TabPage^ pTP = dynamic_cast<TabPage^>(tabControl_object->Controls[i]);
					if (pTP)
					{
						if (pTP->Text == i_Text)
						{
							tabControl_object->SelectedTab = pTP;
							return;
						}
					}
				}

				// If not found, select properties
				tabControl_object->SelectedTab = tabPage_properties;
			}
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		String^ GetCurrentTabName()
		{
			return (tabControl_object->SelectedTab != nullptr) ?
				tabControl_object->SelectedTab->Text : nullptr;
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		bool GetVisibleFlag()
		{
			return this->m_pMemory->GetVisibleFlag();
		}

		//------------------------------------------------------------------------
		//------------------------------------------------------------------------
		System::Windows::Forms::Label ^ GetDescriptionLabel()
		{
			return label_description;
		}

	protected: 
		~cmmObjectForm()
		{
			ClearTabs();

			// Disconnect the tabs that we do not own
			int num_tabs = this->tabControl_object->Controls->Count;
			for (int i=num_tabs-1; i>=0; --i)
			{
				TabPage^ pTabPage = safe_cast<TabPage^>(this->tabControl_object->Controls[i]);
				if (pTabPage)
				{
					if ( (pTabPage != tabPage_properties)
						&& (pTabPage != tabPage_drivers)
						&& (pTabPage != tabPage_connect) )
					{
						this->tabControl_object->Controls->Remove(pTabPage);
					}
				}
			}

			// clear instance
			if (cmmObjectForm::FormInstance == this)
				cmmObjectForm::FormInstance = nullptr;

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

	private: System::Windows::Forms::Button ^  buttonCreate;
	private: System::Windows::Forms::Label ^  label_objectname;
	private: System::Windows::Forms::Label ^  label_description;
	private: System::Windows::Forms::TabControl ^  tabControl_object;
	private: System::Windows::Forms::TabPage ^  tabPage_drivers;
	private: System::Windows::Forms::TreeView ^  treeView_drivers;
	private: System::Windows::Forms::Button ^  button_category;
	private: System::Windows::Forms::Button ^  button_sort;
	private: System::Windows::Forms::Label ^  label_display;
	private: System::Windows::Forms::TabPage ^  tabPage_properties;

	private: System::Windows::Forms::TabPage ^  tabPage_connect;
	private: System::Windows::Forms::Button ^  button_connect_sort;
	private: System::Windows::Forms::Button ^  button_connect_category;
	private: System::Windows::Forms::Button ^  button_connect_attach;
	private: System::Windows::Forms::TreeView ^  treeView_connectdrivers;
	private: System::Windows::Forms::Label ^  label_connect_sortby;

	private:
		int					m_Selection;
		fsysFileList*		m_pDriverList;
		fsysFileList*		m_pConnectDriverList;
		tmaDialogMemory^	m_pMemory;
		gsupTreeViewType	m_ViewType;
		gsupTreeViewType	m_ViewTypeConnect;
		TreeNode^			m_pSelectedTreeNode;

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
			this->buttonCreate = gcnew System::Windows::Forms::Button();
			this->tabControl_object = gcnew System::Windows::Forms::TabControl();
			this->tabPage_properties = gcnew System::Windows::Forms::TabPage();
			this->tabPage_drivers = gcnew System::Windows::Forms::TabPage();
			this->button_sort = gcnew System::Windows::Forms::Button();
			this->button_category = gcnew System::Windows::Forms::Button();
			this->treeView_drivers = gcnew System::Windows::Forms::TreeView();
			this->label_display = gcnew System::Windows::Forms::Label();
			this->tabPage_connect = gcnew System::Windows::Forms::TabPage();
			this->button_connect_sort = gcnew System::Windows::Forms::Button();
			this->button_connect_category = gcnew System::Windows::Forms::Button();
			this->treeView_connectdrivers = gcnew System::Windows::Forms::TreeView();
			this->button_connect_attach = gcnew System::Windows::Forms::Button();
			this->label_connect_sortby = gcnew System::Windows::Forms::Label();
			this->label_objectname = gcnew System::Windows::Forms::Label();
			this->label_description = gcnew System::Windows::Forms::Label();
			this->tabControl_object->SuspendLayout();
			this->tabPage_drivers->SuspendLayout();
			this->tabPage_connect->SuspendLayout();
			this->SuspendLayout();
			// 
			// buttonCreate
			// 
			this->buttonCreate->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->buttonCreate->Location = System::Drawing::Point(116, 245);
			this->buttonCreate->Name = "buttonCreate";
			this->buttonCreate->Size = System::Drawing::Size(127, 28);
			this->buttonCreate->TabIndex = 1;
			this->buttonCreate->Text = "Attach Driver";
			this->buttonCreate->Click += gcnew System::EventHandler(this, &cmmObjectForm::buttonCreate_Click);
			// 
			// tabControl_object
			// 
			this->tabControl_object->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->tabControl_object->Controls->Add(this->tabPage_properties);
			this->tabControl_object->Controls->Add(this->tabPage_drivers);
			this->tabControl_object->Controls->Add(this->tabPage_connect);
			this->tabControl_object->Location = System::Drawing::Point(0, 32);
			this->tabControl_object->Name = "tabControl_object";
			this->tabControl_object->SelectedIndex = 0;
			this->tabControl_object->Size = System::Drawing::Size(376, 304);
			this->tabControl_object->TabIndex = 2;
			// 
			// tabPage_properties
			// 
			this->tabPage_properties->AutoScroll = true;
			this->tabPage_properties->Location = System::Drawing::Point(4, 22);
			this->tabPage_properties->Name = "tabPage_properties";
			this->tabPage_properties->Size = System::Drawing::Size(368, 278);
			this->tabPage_properties->TabIndex = 1;
			this->tabPage_properties->Text = "Properties";
			// 
			// tabPage_drivers
			// 
			this->tabPage_drivers->AutoScroll = true;
			this->tabPage_drivers->Controls->Add(this->button_sort);
			this->tabPage_drivers->Controls->Add(this->button_category);
			this->tabPage_drivers->Controls->Add(this->treeView_drivers);
			this->tabPage_drivers->Controls->Add(this->buttonCreate);
			this->tabPage_drivers->Controls->Add(this->label_display);
			this->tabPage_drivers->Location = System::Drawing::Point(4, 22);
			this->tabPage_drivers->Name = "tabPage_drivers";
			this->tabPage_drivers->Size = System::Drawing::Size(368, 278);
			this->tabPage_drivers->TabIndex = 0;
			this->tabPage_drivers->Text = "Drivers";
			// 
			// button_sort
			// 
			this->button_sort->Location = System::Drawing::Point(128, 5);
			this->button_sort->Name = "button_sort";
			this->button_sort->Size = System::Drawing::Size(48, 22);
			this->button_sort->TabIndex = 2;
			this->button_sort->Text = "A...Z";
			this->button_sort->Click += gcnew System::EventHandler(this, &cmmObjectForm::button_sort_Click);
			// 
			// button_category
			// 
			this->button_category->Location = System::Drawing::Point(72, 5);
			this->button_category->Name = "button_category";
			this->button_category->Size = System::Drawing::Size(48, 22);
			this->button_category->TabIndex = 1;
			this->button_category->Text = "Group";
			this->button_category->Click += gcnew System::EventHandler(this, &cmmObjectForm::button_category_Click);
			// 
			// treeView_drivers
			// 
			this->treeView_drivers->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_drivers->FullRowSelect = true;
			this->treeView_drivers->ImageIndex = -1;
			this->treeView_drivers->Indent = 15;
			this->treeView_drivers->Location = System::Drawing::Point(0, 32);
			this->treeView_drivers->Name = "treeView_drivers";
			this->treeView_drivers->SelectedImageIndex = -1;
			this->treeView_drivers->Size = System::Drawing::Size(368, 208);
			this->treeView_drivers->TabIndex = 0;
			this->treeView_drivers->DoubleClick += gcnew System::EventHandler(this, &cmmObjectForm::treeView_drivers_DoubleClick);
			this->treeView_drivers->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &cmmObjectForm::treeView_drivers_AfterSelect);
			// 
			// label_display
			// 
			this->label_display->Location = System::Drawing::Point(8, 8);
			this->label_display->Name = "label_display";
			this->label_display->Size = System::Drawing::Size(64, 16);
			this->label_display->TabIndex = 3;
			this->label_display->Text = "Display by";
			// 
			// tabPage_connect
			// 
			this->tabPage_connect->AutoScroll = true;
			this->tabPage_connect->Controls->Add(this->button_connect_sort);
			this->tabPage_connect->Controls->Add(this->button_connect_category);
			this->tabPage_connect->Controls->Add(this->treeView_connectdrivers);
			this->tabPage_connect->Controls->Add(this->button_connect_attach);
			this->tabPage_connect->Controls->Add(this->label_connect_sortby);
			this->tabPage_connect->Location = System::Drawing::Point(4, 22);
			this->tabPage_connect->Name = "tabPage_connect";
			this->tabPage_connect->Size = System::Drawing::Size(368, 278);
			this->tabPage_connect->TabIndex = 2;
			this->tabPage_connect->Text = "Connect Drivers";
			// 
			// button_connect_sort
			// 
			this->button_connect_sort->Location = System::Drawing::Point(128, 5);
			this->button_connect_sort->Name = "button_connect_sort";
			this->button_connect_sort->Size = System::Drawing::Size(48, 22);
			this->button_connect_sort->TabIndex = 2;
			this->button_connect_sort->Text = "A...Z";
			this->button_connect_sort->Click += gcnew System::EventHandler(this, &cmmObjectForm::button_connect_sort_Click);
			// 
			// button_connect_category
			// 
			this->button_connect_category->Location = System::Drawing::Point(72, 5);
			this->button_connect_category->Name = "button_connect_category";
			this->button_connect_category->Size = System::Drawing::Size(48, 22);
			this->button_connect_category->TabIndex = 1;
			this->button_connect_category->Text = "Group";
			this->button_connect_category->Click += gcnew System::EventHandler(this, &cmmObjectForm::button_connect_category_Click);
			// 
			// treeView_connectdrivers
			// 
			this->treeView_connectdrivers->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->treeView_connectdrivers->FullRowSelect = true;
			this->treeView_connectdrivers->ImageIndex = -1;
			this->treeView_connectdrivers->Indent = 15;
			this->treeView_connectdrivers->Location = System::Drawing::Point(0, 32);
			this->treeView_connectdrivers->Name = "treeView_connectdrivers";
			this->treeView_connectdrivers->SelectedImageIndex = -1;
			this->treeView_connectdrivers->Size = System::Drawing::Size(368, 208);
			this->treeView_connectdrivers->TabIndex = 0;
			this->treeView_connectdrivers->DoubleClick += gcnew System::EventHandler(this, &cmmObjectForm::treeView_connectdrivers_DoubleClick);
			this->treeView_connectdrivers->AfterSelect += gcnew System::Windows::Forms::TreeViewEventHandler(this, &cmmObjectForm::treeView_connectdrivers_AfterSelect);
			// 
			// button_connect_attach
			// 
			this->button_connect_attach->Anchor = System::Windows::Forms::AnchorStyles::Bottom;
			this->button_connect_attach->Location = System::Drawing::Point(116, 245);
			this->button_connect_attach->Name = "button_connect_attach";
			this->button_connect_attach->Size = System::Drawing::Size(127, 28);
			this->button_connect_attach->TabIndex = 1;
			this->button_connect_attach->Text = "Attach Driver";
			this->button_connect_attach->Click += gcnew System::EventHandler(this, &cmmObjectForm::button_connect_attach_Click);
			// 
			// label_connect_sortby
			// 
			this->label_connect_sortby->Location = System::Drawing::Point(8, 8);
			this->label_connect_sortby->Name = "label_connect_sortby";
			this->label_connect_sortby->Size = System::Drawing::Size(64, 16);
			this->label_connect_sortby->TabIndex = 3;
			this->label_connect_sortby->Text = "Display by";
			// 
			// label_objectname
			// 
			this->label_objectname->Location = System::Drawing::Point(6, 6);
			this->label_objectname->Name = "label_objectname";
			this->label_objectname->Size = System::Drawing::Size(378, 23);
			this->label_objectname->TabIndex = 3;
			this->label_objectname->Text = "Name: ";
			// 
			// label_description
			// 
			this->label_description->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label_description->Location = System::Drawing::Point(8, 344);
			this->label_description->Name = "label_description";
			this->label_description->Size = System::Drawing::Size(368, 40);
			this->label_description->TabIndex = 4;
			this->label_description->Text = "Description:";
			// 
			// cmmObjectForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(384, 390);
			this->Controls->Add(this->label_description);
			this->Controls->Add(this->label_objectname);
			this->Controls->Add(this->tabControl_object);
			this->MinimumSize = System::Drawing::Size(256, 176);
			this->Name = "cmmObjectForm";
			this->Text = "Object";
			this->Closing += gcnew System::ComponentModel::CancelEventHandler(this, &cmmObjectForm::cmmObjectForm_Closing);
			this->Closed += gcnew System::EventHandler(this, &cmmObjectForm::cmmObjectForm_Closed);
			this->tabControl_object->ResumeLayout(false);
			this->tabPage_drivers->ResumeLayout(false);
			this->tabPage_connect->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

			 // Choose selected item in list and close dialog
	private : void CreateDriver(TreeView^ i_pTree)
			  {
				this->m_pSelectedTreeNode = i_pTree->SelectedNode;
				if (this->m_pSelectedTreeNode != nullptr)
				{
					std::string drivername;
					tmaManagedStringUtils::ManagedStringToStdString( m_pSelectedTreeNode->Text, drivername );
					cmmObjectDialogUtil::CreateDriver( drivername );
				}
			  }

private: System::Void cmmObjectForm_Closing(System::Object ^  sender, System::ComponentModel::CancelEventArgs ^  e)
		 {
			 //	clear out the controls 
			 prtyFormControlBuilder::ClearForm(this->tabPage_properties);
		 }

	private: System::Void cmmObjectForm_Closed(System::Object ^  sender, System::EventArgs ^  e)
			{
				delete m_pDriverList;
				m_pDriverList = NULL;
				delete m_pConnectDriverList;
				m_pConnectDriverList = NULL;
			}

	private: System::Void buttonCreate_Click(System::Object ^  sender, System::EventArgs ^  e)
			{
				this->CreateDriver(treeView_drivers);
			}

	private: System::Void treeView_drivers_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
			{
				this->CreateDriver(treeView_drivers);
			}

private: System::Void button_category_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_pDriverList != 0)
			{
 				m_ViewType = eHierarchy;
				gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
			}
		 }

private: System::Void button_sort_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_pDriverList != 0)
			{
				m_ViewType = eFlat;
				gsupTreeViewUtil::PopulateTreeView(treeView_drivers, (*m_pDriverList), m_ViewType, true);
			}
		 }

private: System::Void treeView_drivers_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
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

private: System::Void button_connect_sort_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_pDriverList != 0)
			{
 				m_ViewTypeConnect = eFlat;
				gsupTreeViewUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
			}
		 }

private: System::Void button_connect_category_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			if (m_pDriverList != 0)
			{
 				m_ViewTypeConnect = eHierarchy;
				gsupTreeViewUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
			}
		 }

private: System::Void button_connect_attach_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			this->CreateDriver(treeView_connectdrivers);
		 }

private: System::Void treeView_connectdrivers_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			this->CreateDriver(treeView_connectdrivers);
		 }

private: System::Void treeView_connectdrivers_AfterSelect(System::Object ^  sender, System::Windows::Forms::TreeViewEventArgs ^  e)
		 {
			this->m_pSelectedTreeNode = this->treeView_connectdrivers->SelectedNode;
			if (this->m_pSelectedTreeNode != nullptr)
			{
				if (this->m_pSelectedTreeNode->Nodes->Count == 0)
				{
					this->button_connect_attach->Enabled = true;
				}
				else
				{
					// not a valid driver
					button_connect_attach->Enabled = false;
				}
			}
		 }

};
}
#endif // _MANAGED
