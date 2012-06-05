#pragma once

#ifndef CAM_OBJECT_HPP
#include "camObject.hpp"
#endif
#ifndef CHAR_OBJECT_HPP
#include "charObject.hpp"
#endif
#ifndef CHTR_COMMANDMESSAGEBOX_HPP
#include "chtrCommandMessageBox.hpp"
#endif
#ifndef CMRA_COMMANDMESSAGEBOX_HPP
#include "cmraCommandMessageBox.hpp"
#endif
#ifndef PROP_OBJECT_HPP
#include "propObject.hpp"
#endif

#ifndef CMA_COMMANDMGR_HPP
#include "Tool/cma/cmaCommandMgr.hpp"
#endif
#ifndef MUI_MENUMGR_HPP
#include "Tool/gui/guiMenuMgr.hpp"
#endif
#ifndef MUI_PACKAGE_HPP
#include "ToolUIManaged/mui/muiPackage.hpp"
#endif
#ifndef PRTY_CONTROLFACTORYBASE_HPP
#include "ToolUIManaged/prtym/prtyControlFactoryBase.hpp"
#endif
#ifndef PRTY_CONTROLFACTORYTMC_HPP
#include "ToolUIManaged/prtym/prtyControlFactoryTMC.hpp"
#endif
#ifndef PRTY_CONTROLMGR_HPP
#include "ToolUIManaged/prtym/prtyControlMgr.hpp"
#endif
#ifndef PRTY_FORMCONTROLBUILDER_HPP
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
#endif
#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif
#ifndef TMA_SYSTEM_HPP
#include "ToolUIManaged/tma/tmaSystem.hpp"
#endif

//
namespace prty
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();

			prtyCallbackMgr::Initialize();
			prtyControlMgr::Initialize();
			prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryBase() );
			prtyControlMgr::AddControlFactory( gcnew prtyControlFactoryTMC() );

			//	add some objects
			if (m_pObject1 == 0)
			{
				m_pObject1 = new propObject();
			}
			if (m_pObject2 == 0)
			{
				m_pObject2 = new charObject();
			}
			if (m_pObject3 == 0)
			{
				m_pObject3 = new camObject();

				//	this should trigger the "notify callbacks" -- purely as a test
				m_pObject3->m_basedata.m_Position.SetValue( maVector3d(1.0f,2.0f,3.0f) );
			}

			//	set up the menus + commands
			//
			tmaSystem::g_pMainForm = this;
			muiPackage::Initialize();

			//	normally commands like this would go in the code project or system.
			int menu_id = guiMenuMgr::AddMenuItem( "View", "chtr mb" );
			cmaCommand* pCmd = new chtrCommandMessageBox();
			cmaCommandMgr::Add( pCmd, chtrCommandMessageBox::GetConstTagName(), menu_id );

			menu_id = guiMenuMgr::AddMenuItem( "View", "cmra mb" );
			pCmd = new cmraCommandMessageBox();
			cmaCommandMgr::Add( pCmd, cmraCommandMessageBox::GetConstTagName(), menu_id );

			//	create a list of items.
			listBox_objects->Items->Add( "1) prop" );
			listBox_objects->Items->Add( "2) character" );
			listBox_objects->Items->Add( "3) camera" );

			m_SortFlag = 0;	// by property
			button_sortbycategory->Enabled = true;
			button_sortbypropertyname->Enabled = false;
		}
  
	protected:
		~Form1()
		{
			prtyControlMgr::DeInitialize();
			prtyCallbackMgr::DeInitialize();

			if (components)
			{
				delete components;
			}
			//__super::Dispose(disposing);
		}


	private: System::Windows::Forms::Button ^  button_clear;
	private: System::Windows::Forms::ListBox ^  listBox_objects;
	private: System::Windows::Forms::TabControl ^  tabControl_properties;
	private: System::Windows::Forms::TabPage ^  tabPage_properties;
	private: System::Windows::Forms::Button ^  button_sortbypropertyname;
	private: System::Windows::Forms::Button ^  button_sortbycategory;
	private: System::Windows::Forms::GroupBox ^  groupBox_sort;
	private: System::Windows::Forms::TabControl^  tabControl_prty;

	private: System::Windows::Forms::TabPage^  tabPage_prty;
	private: System::Windows::Forms::TabPage^  tabPage_hotkeys;

	private: System::Windows::Forms::MenuStrip^  menuStrip_main;
	private: System::Windows::Forms::ToolStripMenuItem^  ToolStripMenuItem_help;

	private: propObject* m_pObject1;
	private: charObject* m_pObject2;
	private: camObject* m_pObject3;
	private: int m_SortFlag;
	private: System::Windows::Forms::ToolStripMenuItem^  file1ToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  helpToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  aboutToolStripMenuItem;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^ components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->button_clear = (gcnew System::Windows::Forms::Button());
			this->listBox_objects = (gcnew System::Windows::Forms::ListBox());
			this->tabControl_properties = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_properties = (gcnew System::Windows::Forms::TabPage());
			this->button_sortbypropertyname = (gcnew System::Windows::Forms::Button());
			this->button_sortbycategory = (gcnew System::Windows::Forms::Button());
			this->groupBox_sort = (gcnew System::Windows::Forms::GroupBox());
			this->tabControl_prty = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_prty = (gcnew System::Windows::Forms::TabPage());
			this->tabPage_hotkeys = (gcnew System::Windows::Forms::TabPage());
			this->menuStrip_main = (gcnew System::Windows::Forms::MenuStrip());
			this->ToolStripMenuItem_help = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->file1ToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->helpToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->aboutToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tabControl_properties->SuspendLayout();
			this->groupBox_sort->SuspendLayout();
			this->tabControl_prty->SuspendLayout();
			this->tabPage_prty->SuspendLayout();
			this->menuStrip_main->SuspendLayout();
			this->SuspendLayout();
			// 
			// button_clear
			// 
			this->button_clear->Location = System::Drawing::Point(20, 94);
			this->button_clear->Name = L"button_clear";
			this->button_clear->Size = System::Drawing::Size(65, 23);
			this->button_clear->TabIndex = 2;
			this->button_clear->Text = L"clear";
			this->button_clear->Click += gcnew System::EventHandler(this, &Form1::button_clear_Click);
			// 
			// listBox_objects
			// 
			this->listBox_objects->Location = System::Drawing::Point(20, 6);
			this->listBox_objects->Name = L"listBox_objects";
			this->listBox_objects->SelectionMode = System::Windows::Forms::SelectionMode::MultiExtended;
			this->listBox_objects->Size = System::Drawing::Size(160, 82);
			this->listBox_objects->TabIndex = 3;
			this->listBox_objects->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::listBox_objects_SelectedIndexChanged);
			// 
			// tabControl_properties
			// 
			this->tabControl_properties->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_properties->Appearance = System::Windows::Forms::TabAppearance::Buttons;
			this->tabControl_properties->Controls->Add(this->tabPage_properties);
			this->tabControl_properties->Location = System::Drawing::Point(6, 122);
			this->tabControl_properties->Name = L"tabControl_properties";
			this->tabControl_properties->SelectedIndex = 0;
			this->tabControl_properties->Size = System::Drawing::Size(485, 369);
			this->tabControl_properties->TabIndex = 4;
			// 
			// tabPage_properties
			// 
			this->tabPage_properties->AutoScroll = true;
			this->tabPage_properties->Location = System::Drawing::Point(4, 25);
			this->tabPage_properties->Name = L"tabPage_properties";
			this->tabPage_properties->Size = System::Drawing::Size(477, 340);
			this->tabPage_properties->TabIndex = 0;
			this->tabPage_properties->Text = L"Properties";
			// 
			// button_sortbypropertyname
			// 
			this->button_sortbypropertyname->Location = System::Drawing::Point(8, 48);
			this->button_sortbypropertyname->Name = L"button_sortbypropertyname";
			this->button_sortbypropertyname->Size = System::Drawing::Size(68, 23);
			this->button_sortbypropertyname->TabIndex = 5;
			this->button_sortbypropertyname->Text = L"Name";
			this->button_sortbypropertyname->Click += gcnew System::EventHandler(this, &Form1::button_sortbypropertyname_Click);
			// 
			// button_sortbycategory
			// 
			this->button_sortbycategory->Location = System::Drawing::Point(8, 16);
			this->button_sortbycategory->Name = L"button_sortbycategory";
			this->button_sortbycategory->Size = System::Drawing::Size(68, 23);
			this->button_sortbycategory->TabIndex = 6;
			this->button_sortbycategory->Text = L"Category";
			this->button_sortbycategory->Click += gcnew System::EventHandler(this, &Form1::button_sortbycategory_Click);
			// 
			// groupBox_sort
			// 
			this->groupBox_sort->Controls->Add(this->button_sortbycategory);
			this->groupBox_sort->Controls->Add(this->button_sortbypropertyname);
			this->groupBox_sort->Location = System::Drawing::Point(195, 6);
			this->groupBox_sort->Name = L"groupBox_sort";
			this->groupBox_sort->Size = System::Drawing::Size(86, 82);
			this->groupBox_sort->TabIndex = 7;
			this->groupBox_sort->TabStop = false;
			this->groupBox_sort->Text = L"Sort Controls";
			// 
			// tabControl_prty
			// 
			this->tabControl_prty->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_prty->Controls->Add(this->tabPage_prty);
			this->tabControl_prty->Controls->Add(this->tabPage_hotkeys);
			this->tabControl_prty->Location = System::Drawing::Point(2, 27);
			this->tabControl_prty->Name = L"tabControl_prty";
			this->tabControl_prty->SelectedIndex = 0;
			this->tabControl_prty->Size = System::Drawing::Size(505, 523);
			this->tabControl_prty->TabIndex = 8;
			// 
			// tabPage_prty
			// 
			this->tabPage_prty->Controls->Add(this->tabControl_properties);
			this->tabPage_prty->Controls->Add(this->groupBox_sort);
			this->tabPage_prty->Controls->Add(this->listBox_objects);
			this->tabPage_prty->Controls->Add(this->button_clear);
			this->tabPage_prty->Location = System::Drawing::Point(4, 22);
			this->tabPage_prty->Name = L"tabPage_prty";
			this->tabPage_prty->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_prty->Size = System::Drawing::Size(497, 497);
			this->tabPage_prty->TabIndex = 0;
			this->tabPage_prty->Text = L"objects";
			this->tabPage_prty->UseVisualStyleBackColor = true;
			// 
			// tabPage_hotkeys
			// 
			this->tabPage_hotkeys->Location = System::Drawing::Point(4, 22);
			this->tabPage_hotkeys->Name = L"tabPage_hotkeys";
			this->tabPage_hotkeys->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_hotkeys->Size = System::Drawing::Size(497, 497);
			this->tabPage_hotkeys->TabIndex = 1;
			this->tabPage_hotkeys->Text = L"hot keys";
			this->tabPage_hotkeys->UseVisualStyleBackColor = true;
			this->tabPage_hotkeys->Enter += gcnew System::EventHandler(this, &Form1::tabPage_hotkeys_Enter);
			// 
			// menuStrip_main
			// 
			this->menuStrip_main->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->file1ToolStripMenuItem, 
				this->helpToolStripMenuItem});
			this->menuStrip_main->Location = System::Drawing::Point(0, 0);
			this->menuStrip_main->Name = L"menuStrip_main";
			this->menuStrip_main->Size = System::Drawing::Size(508, 24);
			this->menuStrip_main->TabIndex = 9;
			this->menuStrip_main->Text = L"menuStrip1";
			// 
			// ToolStripMenuItem_help
			// 
			this->ToolStripMenuItem_help->Name = L"ToolStripMenuItem_help";
			this->ToolStripMenuItem_help->Size = System::Drawing::Size(40, 20);
			this->ToolStripMenuItem_help->Text = L"Help";
			// 
			// file1ToolStripMenuItem
			// 
			this->file1ToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->openToolStripMenuItem});
			this->file1ToolStripMenuItem->Name = L"file1ToolStripMenuItem";
			this->file1ToolStripMenuItem->Size = System::Drawing::Size(35, 20);
			this->file1ToolStripMenuItem->Text = L"File";
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->openToolStripMenuItem->Text = L"Open";
			// 
			// helpToolStripMenuItem
			// 
			this->helpToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->aboutToolStripMenuItem});
			this->helpToolStripMenuItem->Name = L"helpToolStripMenuItem";
			this->helpToolStripMenuItem->Size = System::Drawing::Size(40, 20);
			this->helpToolStripMenuItem->Text = L"Help";
			// 
			// aboutToolStripMenuItem
			// 
			this->aboutToolStripMenuItem->Name = L"aboutToolStripMenuItem";
			this->aboutToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->aboutToolStripMenuItem->Text = L"About";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->AutoScroll = true;
			this->ClientSize = System::Drawing::Size(508, 550);
			this->Controls->Add(this->tabControl_prty);
			this->Controls->Add(this->menuStrip_main);
			this->MainMenuStrip = this->menuStrip_main;
			this->MinimumSize = System::Drawing::Size(336, 272);
			this->Name = L"Form1";
			this->Text = L"Test Properties + Controls";
			this->tabControl_properties->ResumeLayout(false);
			this->groupBox_sort->ResumeLayout(false);
			this->tabControl_prty->ResumeLayout(false);
			this->tabPage_prty->ResumeLayout(false);
			this->menuStrip_main->ResumeLayout(false);
			this->menuStrip_main->PerformLayout();
			this->ResumeLayout(false);
			this->PerformLayout();

		}	
	private: System::Void button_clear_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				listBox_objects->SelectedIndex = -1;
				prtyFormControlBuilder::InitForm(tabPage_properties);
			 }
	private: System::Void listBox_objects_SelectedIndexChanged(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 BuildControls();
			 }

private: System::Void button_sortbycategory_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_SortFlag != 1)
			 {
				button_sortbycategory->Enabled = false;
				button_sortbypropertyname->Enabled = true;
				m_SortFlag = 1;
				BuildControls();
			 }
		 }

private: System::Void button_sortbypropertyname_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (m_SortFlag != 0)
			 {
				button_sortbycategory->Enabled = true;
				button_sortbypropertyname->Enabled = false;
				m_SortFlag = 0;
				BuildControls();
			 }
		 }

private:
	void BuildControls()
	{
		int num_selected = listBox_objects->SelectedIndices->Count;
		if ( num_selected > 1)
		{
			prtyProperty* pProperty = NULL;
			prtyPropertyUIInfoContainer incommon_list;

			prtyFormControlBuilder::InitForm(tabPage_properties);

			for (int i = 0; i < num_selected; ++i)
			{
				switch (this->listBox_objects->SelectedIndices[i])
				{
					case 0:
						prtyFormControlBuilder::AddToFormList(m_pObject1->GetList());
						break;
					case 1:
						prtyFormControlBuilder::AddToFormList(m_pObject2->GetList());
						break;
					case 2:
						prtyFormControlBuilder::AddToFormList(m_pObject3->GetList());
						break;
				}
			}

			prtyFormControlBuilder::SortFormList(m_SortFlag);
			prtyFormControlBuilder::CreateControlsForForm( tabPage_properties, (m_SortFlag == 1) );
		}
		else if (num_selected == 1)
		{
			switch (listBox_objects->SelectedIndex)
			{
				case 0:
				{
					if (m_SortFlag == 0)
						m_pObject1->SortListByPropertyName();
					else
						m_pObject1->SortListByCategory();
					prtyFormControlBuilder::BuildForm( tabPage_properties, (m_pObject1->GetList()), (m_SortFlag == 1) );
					break;
				}
				case 1: 				
				{
					if (m_SortFlag == 0)
						m_pObject2->SortListByPropertyName();
					else
						m_pObject2->SortListByCategory();
					prtyFormControlBuilder::BuildForm( tabPage_properties, (m_pObject2->GetList()), (m_SortFlag == 1) );
					break;
				}
				case 2:
				{
					if (m_SortFlag == 0)
						m_pObject3->SortListByPropertyName();
					else
						m_pObject3->SortListByCategory();
					prtyFormControlBuilder::BuildForm( tabPage_properties, (m_pObject3->GetList()), (m_SortFlag == 1) );
					break;
				}
			}
		}
	}
private:
		void build_hotkeys()
		{
			prtyPropertyUIInfoContainer plist;
			cmaCommandMgr::GetCommandPropertyUIInfoList( plist );
			prtyFormControlBuilder::BuildForm( this->tabPage_hotkeys, plist.GetList(), true );
		}

private: System::Void tabPage_hotkeys_Enter(System::Object^  sender, System::EventArgs^  e) 
		 {
			 build_hotkeys();
		 }
};
}


