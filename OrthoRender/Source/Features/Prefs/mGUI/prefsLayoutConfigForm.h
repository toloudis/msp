#pragma once

#ifndef PREFSDATA_HPP
#include "Features/Prefs/PrefsData.hpp"
#endif
#include "Features/Prefs/mGUI/prefsLayoutConfigNameForm.h"
#ifndef PREFSMGR_HPP
#include "Features/Prefs/PrefsMgr.hpp"
#endif

#ifndef TMA_REGISTRYUTIL_HPP
#include "ToolUIManaged/tma/tmaRegistryUtil.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif
#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Features
{
	/// <summary> 
	/// Summary for prefsLayoutConfigForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class prefsLayoutConfigForm : public System::Windows::Forms::Form
	{
	public: static prefsLayoutConfigForm^ FormInstance = nullptr;

	public: TabPage^ GetTabPage()
		{
			return this->tabPage_layoutconfig;
		}

	public: 
		prefsLayoutConfigForm(void)
		{
			InitializeComponent();

			grab_registry_list();
		}
        
	protected: 
		~prefsLayoutConfigForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListView ^  listView_layoutconfigs;
	private: System::Windows::Forms::Label ^  label_layoutconfigs;

	private: System::Windows::Forms::Button ^  button_delete;

	private: System::Windows::Forms::TabControl ^  tabControl_layoutconfig;
	private: System::Windows::Forms::TabPage ^  tabPage_layoutconfig;
	private: System::Windows::Forms::Button^  button_load;
	private: System::Windows::Forms::Button^  button_save;

//	private: tmaDialogMemory^	m_pMemory;
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
			this->listView_layoutconfigs = (gcnew System::Windows::Forms::ListView());
			this->label_layoutconfigs = (gcnew System::Windows::Forms::Label());
			this->button_delete = (gcnew System::Windows::Forms::Button());
			this->tabControl_layoutconfig = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_layoutconfig = (gcnew System::Windows::Forms::TabPage());
			this->button_load = (gcnew System::Windows::Forms::Button());
			this->button_save = (gcnew System::Windows::Forms::Button());
			this->tabControl_layoutconfig->SuspendLayout();
			this->tabPage_layoutconfig->SuspendLayout();
			this->SuspendLayout();
			// 
			// listView_layoutconfigs
			// 
			this->listView_layoutconfigs->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->listView_layoutconfigs->AutoArrange = false;
			this->listView_layoutconfigs->FullRowSelect = true;
			this->listView_layoutconfigs->HideSelection = false;
			this->listView_layoutconfigs->Location = System::Drawing::Point(8, 40);
			this->listView_layoutconfigs->MultiSelect = false;
			this->listView_layoutconfigs->Name = L"listView_layoutconfigs";
			this->listView_layoutconfigs->Size = System::Drawing::Size(272, 244);
			this->listView_layoutconfigs->TabIndex = 0;
			this->listView_layoutconfigs->UseCompatibleStateImageBehavior = false;
			this->listView_layoutconfigs->View = System::Windows::Forms::View::List;
			this->listView_layoutconfigs->DoubleClick += gcnew System::EventHandler(this, &prefsLayoutConfigForm::listView_layoutconfigs_DoubleClick);
			this->listView_layoutconfigs->SelectedIndexChanged += gcnew System::EventHandler(this, &prefsLayoutConfigForm::listView_layoutconfigs_SelectedIndexChanged);
			// 
			// label_layoutconfigs
			// 
			this->label_layoutconfigs->Location = System::Drawing::Point(16, 16);
			this->label_layoutconfigs->Name = L"label_layoutconfigs";
			this->label_layoutconfigs->Size = System::Drawing::Size(176, 23);
			this->label_layoutconfigs->TabIndex = 1;
			this->label_layoutconfigs->Text = L"Layout Configurations";
			// 
			// button_delete
			// 
			this->button_delete->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_delete->Location = System::Drawing::Point(200, 293);
			this->button_delete->Name = L"button_delete";
			this->button_delete->Size = System::Drawing::Size(75, 23);
			this->button_delete->TabIndex = 3;
			this->button_delete->Text = L"Delete";
			this->button_delete->Click += gcnew System::EventHandler(this, &prefsLayoutConfigForm::button_delete_Click);
			// 
			// tabControl_layoutconfig
			// 
			this->tabControl_layoutconfig->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl_layoutconfig->Controls->Add(this->tabPage_layoutconfig);
			this->tabControl_layoutconfig->Location = System::Drawing::Point(8, 8);
			this->tabControl_layoutconfig->Name = L"tabControl_layoutconfig";
			this->tabControl_layoutconfig->SelectedIndex = 0;
			this->tabControl_layoutconfig->Size = System::Drawing::Size(296, 353);
			this->tabControl_layoutconfig->TabIndex = 5;
			// 
			// tabPage_layoutconfig
			// 
			this->tabPage_layoutconfig->Controls->Add(this->button_load);
			this->tabPage_layoutconfig->Controls->Add(this->button_save);
			this->tabPage_layoutconfig->Controls->Add(this->listView_layoutconfigs);
			this->tabPage_layoutconfig->Controls->Add(this->label_layoutconfigs);
			this->tabPage_layoutconfig->Controls->Add(this->button_delete);
			this->tabPage_layoutconfig->Location = System::Drawing::Point(4, 22);
			this->tabPage_layoutconfig->Name = L"tabPage_layoutconfig";
			this->tabPage_layoutconfig->Size = System::Drawing::Size(288, 327);
			this->tabPage_layoutconfig->TabIndex = 0;
			this->tabPage_layoutconfig->Text = L"Layout";
			// 
			// button_load
			// 
			this->button_load->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_load->Location = System::Drawing::Point(108, 293);
			this->button_load->Name = L"button_load";
			this->button_load->Size = System::Drawing::Size(75, 23);
			this->button_load->TabIndex = 6;
			this->button_load->Text = L"Load";
			this->button_load->UseVisualStyleBackColor = true;
			this->button_load->Click += gcnew System::EventHandler(this, &prefsLayoutConfigForm::button_load_Click);
			// 
			// button_save
			// 
			this->button_save->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->button_save->Location = System::Drawing::Point(16, 293);
			this->button_save->Name = L"button_save";
			this->button_save->Size = System::Drawing::Size(75, 23);
			this->button_save->TabIndex = 5;
			this->button_save->Text = L"Save";
			this->button_save->UseVisualStyleBackColor = true;
			this->button_save->Click += gcnew System::EventHandler(this, &prefsLayoutConfigForm::button_save_Click);
			// 
			// prefsLayoutConfigForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(312, 367);
			this->Controls->Add(this->tabControl_layoutconfig);
			this->Name = L"prefsLayoutConfigForm";
			this->Text = L"Layout Configuration";
			this->TopMost = true;
			this->Click += gcnew System::EventHandler(this, &prefsLayoutConfigForm::prefsLayoutConfigForm_Click);
			this->Load += gcnew System::EventHandler(this, &prefsLayoutConfigForm::prefsLayoutConfigForm_Load);
			this->tabControl_layoutconfig->ResumeLayout(false);
			this->tabPage_layoutconfig->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
	private: System::Void button_add_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 if ( prefsLayoutConfigNameForm::FormInstance == nullptr )
				 {
					 prefsLayoutConfigNameForm::FormInstance = gcnew prefsLayoutConfigNameForm;
				 }
				 prefsLayoutConfigNameForm::FormInstance->ShowDialog();

				//	set the registry key
			 	PrefsData& data = PrefsMgr::Data();
				
				std::string temp;
				tmaManagedStringUtils::ManagedStringToStdString( prefsLayoutConfigNameForm::FormInstance->GetConfigName(), temp );
				data.m_CurrentLayout.SetValue(temp);
				std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
				keyname += "\\";
				keyname += data.m_CurrentLayout.GetValue();
				tmaRegistryUtil::SetKey( tmaRegistryUtil::e_CurrentUser, keyname );

				//	add it to the list
				this->listView_layoutconfigs->Items->Add( gcnew System::String(data.m_CurrentLayout.GetValue().c_str()) );
				this->listView_layoutconfigs->Invalidate();

				//	set config to selected name, write out locations, reset to default name
				tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(data.m_CurrentLayout.GetValue().c_str());
				tmaDialogMemoryMgr::ChangeWriteFlag();
				tmaDialogMemoryMgr::Write();
				tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 
				delete prefsLayoutConfigNameForm::FormInstance;
			 }

	private: System::Void button_delete_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				for (int i = 0; i < this->listView_layoutconfigs->SelectedItems->Count; ++i )
				{
					//	delete the registry key
					PrefsData& data = PrefsMgr::Data();
					std::string selected_key;
					tmaManagedStringUtils::ManagedStringToStdString( this->listView_layoutconfigs->SelectedItems[i]->Text, selected_key );
					if ( data.m_CurrentLayout == selected_key)
					{
						data.m_CurrentLayout = tmaDialogMemory::lc_Key_LayoutConfig_Default;
						read_selected( data.m_CurrentLayout.GetValue() );
					}
					std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
					keyname += "\\";
					keyname += selected_key;
					tmaRegistryUtil::DeleteKey( tmaRegistryUtil::e_CurrentUser, keyname );

					this->listView_layoutconfigs->Items->Remove( this->listView_layoutconfigs->SelectedItems[i] );
				}
			 }

private: System::Void button_select_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			load_selected();
		 }

private: System::Void listView_layoutconfigs_DoubleClick(System::Object ^  sender, System::EventArgs ^  e)
		 {
			load_selected();
		 }

private: System::Void prefsLayoutConfigForm_Click(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 set_selected();
		 }

private: System::Void prefsLayoutConfigForm_Load(System::Object ^  sender, System::EventArgs ^  e)
		 {
//			m_pMemory = gcnew tmaDialogMemory( this );
		 }

private: void set_selected()
		 {
			//	search through and set the selected
			//
			PrefsData& data = PrefsMgr::Data();
			for (int i = 0; i < this->listView_layoutconfigs->Items->Count; ++i)
			{
				ListViewItem^ pItem = this->listView_layoutconfigs->Items[i];
				if ( pItem->Text == gcnew System::String(data.m_CurrentLayout.GetValue().c_str()) )
				{
					pItem->Selected = true;
					tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(data.m_CurrentLayout.GetValue().c_str());
				}
			}
		 }

private: void read_selected( const std::string& i_Layout )
		{
			tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(i_Layout.c_str());
			tmaDialogMemoryMgr::Read();
			tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 
		}

private: void write_selected( const std::string& i_Layout )
		{
			tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(i_Layout.c_str());
			tmaDialogMemoryMgr::Write();
			tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 
		}

private: void save_selected()
		 {
			if (   (this->listView_layoutconfigs->SelectedItems->Count > 0)
				&& (this->listView_layoutconfigs->SelectedIndices[0] != 0))		// 0 = "new layout"
			{
				// apply the first selected item...only one can be applied
				//
			 	PrefsData& data = PrefsMgr::Data();
				std::string temp;
				tmaManagedStringUtils::ManagedStringToStdString( this->listView_layoutconfigs->SelectedItems[0]->Text, temp );

				data.m_CurrentLayout.SetValue(temp);
				read_selected( temp );
				write_selected(temp);
			}
		 }

private: void load_selected()
		 {
			if (   (this->listView_layoutconfigs->SelectedItems->Count > 0)
				&& (this->listView_layoutconfigs->SelectedIndices[0] != 0))		// 0 = "new layout"
			{
				// apply the first selected item...only one can be applied
				//
			 	PrefsData& data = PrefsMgr::Data();
				std::string temp;
				tmaManagedStringUtils::ManagedStringToStdString( this->listView_layoutconfigs->SelectedItems[0]->Text, temp );

				data.m_CurrentLayout.SetValue(temp);
				read_selected( temp );
			}
		 }

private: void grab_registry_list()
		 {
			 this->listView_layoutconfigs->Items->Add( gcnew System::String( "new layout" ) );

			 std::vector<std::string> keys;
			 std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
			 tmaRegistryUtil::GetKeys( tmaRegistryUtil::e_CurrentUser, keyname, keys );
			 for (int i = 0; i < keys.size(); ++i)
			 {
				 this->listView_layoutconfigs->Items->Add( gcnew System::String( keys[i].c_str() ) );
			 }
		 }

private: System::Void button_save_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			if (this->listView_layoutconfigs->SelectedItems->Count > 0)
			{
				if ( prefsLayoutConfigNameForm::FormInstance == nullptr )
				{
					prefsLayoutConfigNameForm::FormInstance = gcnew prefsLayoutConfigNameForm;
				}
				prefsLayoutConfigNameForm::FormInstance->ShowDialog();

				//	set the registry key
		 		PrefsData& data = PrefsMgr::Data();
				std::string temp;
				tmaManagedStringUtils::ManagedStringToStdString( prefsLayoutConfigNameForm::FormInstance->GetConfigName(), temp );
				data.m_CurrentLayout.SetValue(temp);
				std::string keyname(tmaDialogMemory::lc_Key_LayoutConfiguration);
				keyname += "\\";
				keyname += data.m_CurrentLayout.GetValue();
				tmaRegistryUtil::SetKey( tmaRegistryUtil::e_CurrentUser, keyname );

				//	add it to the list
				this->listView_layoutconfigs->Items->Add( gcnew System::String(data.m_CurrentLayout.GetValue().c_str()) );
				this->listView_layoutconfigs->Invalidate();

				//	set config to selected name, write out locations, reset to default name
				tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String(data.m_CurrentLayout.GetValue().c_str());
				tmaDialogMemoryMgr::ChangeWriteFlag();
				tmaDialogMemoryMgr::Write();
				tmaDialogMemory::m_CurrentLayoutConfig = gcnew System::String("Default");	 

				delete prefsLayoutConfigNameForm::FormInstance;

				save_selected();
			}
		}

private: System::Void button_load_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			load_selected();
		 }

private: System::Void listView_layoutconfigs_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			if (this->listView_layoutconfigs->SelectedItems->Count > 0)
			{
				if (this->listView_layoutconfigs->SelectedIndices[0] != 0)		// 0 = "new layout"
				{
					this->button_save->Enabled = true;
					this->button_load->Enabled = true;
					this->button_delete->Enabled = true;
				}
				else
				{
					this->button_save->Enabled = true;
					this->button_load->Enabled = false;
					this->button_delete->Enabled = false;
				}
			}
			else
			{
				this->button_save->Enabled = false;
				this->button_load->Enabled = false;
				this->button_delete->Enabled = false;
			}
		}
	};
}
#endif // _MANAGED
