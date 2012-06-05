#pragma once

#ifndef FGMT_SCRIPTOBJECT_HPP
#include "Support/fgmt/fgmtScriptObject.hpp"
#endif
#ifndef FGMT_OPERATIONS_HPP
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#endif
#ifndef FGMT_PROPERTYOBJECT_HPP
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#endif
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
	/// Summary for fgmtFragmentsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class fgmtFragmentsForm : public System::Windows::Forms::Form
	{
	public:
		static fgmtFragmentsForm^ FormInstance = nullptr;

		fgmtFragmentsForm(void) : m_pObject(NULL)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;

			InitializeComponent();

			SetupData(m_pObject);
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(fgmtScriptObject *i_pObject)
		{
			m_bDisableNotify = true;
			SetupData(i_pObject);
			m_pObject = i_pObject;
			m_bDisableNotify = false;

			// Set selected fragment index based on what was selected last
			int sel_index = i_pObject->GetLastSelectedFragmentIndex();
			if (sel_index < i_pObject->GetNumFragments())
			{
				this->comboBox_Fragments->SelectedIndex = sel_index;
			}
		}

		void UpdateHighlightToggle()
		{
			// set highlight to current state in operations
			this->checkBox_HighlightSurface->Checked = fgmtOperations::IsHighlightFragment();
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~fgmtFragmentsForm()
		{
			// clear instance
			if (fgmtFragmentsForm::FormInstance == this)
				fgmtFragmentsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: fgmtScriptObject* m_pObject;
	private: fgmtPropertyObject* m_pProperties;

		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;

	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage_Fragments;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  comboBox_Fragments;
	private: System::Windows::Forms::Button^  button_OverrideFragments;
	private: System::Windows::Forms::Button^  button_SetAll;

	private: System::Windows::Forms::Button^  button_ApplyAOToAll;
	private: System::Windows::Forms::CheckBox^  checkBox_HighlightSurface;
	private: System::Windows::Forms::Button^  button_SaveAOTextures;
	private: System::Windows::Forms::Panel^  panel_SurfaceProperties;

	protected: 

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Fragments = (gcnew System::Windows::Forms::TabPage());
			this->panel_SurfaceProperties = (gcnew System::Windows::Forms::Panel());
			this->checkBox_HighlightSurface = (gcnew System::Windows::Forms::CheckBox());
			this->button_SetAll = (gcnew System::Windows::Forms::Button());
			this->button_SaveAOTextures = (gcnew System::Windows::Forms::Button());
			this->button_ApplyAOToAll = (gcnew System::Windows::Forms::Button());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->comboBox_Fragments = (gcnew System::Windows::Forms::ComboBox());
			this->button_OverrideFragments = (gcnew System::Windows::Forms::Button());
			this->tabControl1->SuspendLayout();
			this->tabPage_Fragments->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage_Fragments);
			this->tabControl1->Dock = System::Windows::Forms::DockStyle::Fill;
			this->tabControl1->Location = System::Drawing::Point(0, 0);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(463, 650);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage_Fragments
			// 
			this->tabPage_Fragments->Controls->Add(this->panel_SurfaceProperties);
			this->tabPage_Fragments->Controls->Add(this->checkBox_HighlightSurface);
			this->tabPage_Fragments->Controls->Add(this->button_SetAll);
			this->tabPage_Fragments->Controls->Add(this->button_SaveAOTextures);
			this->tabPage_Fragments->Controls->Add(this->button_ApplyAOToAll);
			this->tabPage_Fragments->Controls->Add(this->label2);
			this->tabPage_Fragments->Controls->Add(this->comboBox_Fragments);
			this->tabPage_Fragments->Controls->Add(this->button_OverrideFragments);
			this->tabPage_Fragments->Location = System::Drawing::Point(4, 22);
			this->tabPage_Fragments->Name = L"tabPage_Fragments";
			this->tabPage_Fragments->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Fragments->Size = System::Drawing::Size(455, 624);
			this->tabPage_Fragments->TabIndex = 0;
			this->tabPage_Fragments->Text = L"Surfaces";
			this->tabPage_Fragments->UseVisualStyleBackColor = true;
			// 
			// panel_SurfaceProperties
			// 
			this->panel_SurfaceProperties->AutoScroll = true;
			this->panel_SurfaceProperties->Location = System::Drawing::Point(19, 108);
			this->panel_SurfaceProperties->Name = L"panel_SurfaceProperties";
			this->panel_SurfaceProperties->Size = System::Drawing::Size(416, 508);
			this->panel_SurfaceProperties->TabIndex = 29;
			// 
			// checkBox_HighlightSurface
			// 
			this->checkBox_HighlightSurface->Appearance = System::Windows::Forms::Appearance::Button;
			this->checkBox_HighlightSurface->Location = System::Drawing::Point(315, 18);
			this->checkBox_HighlightSurface->Name = L"checkBox_HighlightSurface";
			this->checkBox_HighlightSurface->Size = System::Drawing::Size(120, 24);
			this->checkBox_HighlightSurface->TabIndex = 25;
			this->checkBox_HighlightSurface->Text = L"Highlight Surface";
			this->checkBox_HighlightSurface->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			this->checkBox_HighlightSurface->CheckedChanged += gcnew System::EventHandler(this, &fgmtFragmentsForm::checkBox_HighlightSurface_CheckedChanged);
			// 
			// button_SetAll
			// 
			this->button_SetAll->Location = System::Drawing::Point(150, 17);
			this->button_SetAll->Name = L"button_SetAll";
			this->button_SetAll->Size = System::Drawing::Size(160, 27);
			this->button_SetAll->TabIndex = 14;
			this->button_SetAll->Text = L"Apply flags to all";
			this->button_SetAll->UseVisualStyleBackColor = true;
			this->button_SetAll->Click += gcnew System::EventHandler(this, &fgmtFragmentsForm::button_SetAll_Click);
			// 
			// button_SaveAOTextures
			// 
			this->button_SaveAOTextures->Location = System::Drawing::Point(150, 48);
			this->button_SaveAOTextures->Name = L"button_SaveAOTextures";
			this->button_SaveAOTextures->Size = System::Drawing::Size(160, 27);
			this->button_SaveAOTextures->TabIndex = 28;
			this->button_SaveAOTextures->Text = L"Save AO Textures";
			this->button_SaveAOTextures->UseVisualStyleBackColor = true;
			this->button_SaveAOTextures->Click += gcnew System::EventHandler(this, &fgmtFragmentsForm::button_SaveAOTextures_Click);
			// 
			// button_ApplyAOToAll
			// 
			this->button_ApplyAOToAll->Location = System::Drawing::Point(17, 48);
			this->button_ApplyAOToAll->Name = L"button_ApplyAOToAll";
			this->button_ApplyAOToAll->Size = System::Drawing::Size(127, 27);
			this->button_ApplyAOToAll->TabIndex = 24;
			this->button_ApplyAOToAll->Text = L"Apply AO to all";
			this->button_ApplyAOToAll->UseVisualStyleBackColor = true;
			this->button_ApplyAOToAll->Click += gcnew System::EventHandler(this, &fgmtFragmentsForm::button_ApplyAOToAll_Click);
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(14, 81);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(48, 24);
			this->label2->TabIndex = 12;
			this->label2->Text = L"Surface";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// comboBox_Fragments
			// 
			this->comboBox_Fragments->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Fragments->Location = System::Drawing::Point(81, 81);
			this->comboBox_Fragments->Name = L"comboBox_Fragments";
			this->comboBox_Fragments->Size = System::Drawing::Size(354, 21);
			this->comboBox_Fragments->TabIndex = 11;
			this->comboBox_Fragments->SelectedIndexChanged += gcnew System::EventHandler(this, &fgmtFragmentsForm::comboBox_Fragments_SelectedIndexChanged);
			// 
			// button_OverrideFragments
			// 
			this->button_OverrideFragments->Location = System::Drawing::Point(17, 18);
			this->button_OverrideFragments->Name = L"button_OverrideFragments";
			this->button_OverrideFragments->Size = System::Drawing::Size(127, 24);
			this->button_OverrideFragments->TabIndex = 10;
			this->button_OverrideFragments->Text = L"Override Surfaces";
			this->button_OverrideFragments->Click += gcnew System::EventHandler(this, &fgmtFragmentsForm::button_OverrideFragments_Click);
			// 
			// fgmtFragmentsForm
			// 
			this->ClientSize = System::Drawing::Size(463, 650);
			this->Controls->Add(this->tabControl1);
			this->Name = L"fgmtFragmentsForm";
			this->Text = L"Fragments";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Fragments->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion
		//

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_Fragments;
		}

	private:
		void SetupData(fgmtScriptObject *i_pObject)
		{
			m_pProperties = NULL;

			UpdateHighlightToggle();

			if (m_bOurChange) return;

			//ClearCurrentFragmentData();
			fill_list(i_pObject);
		}

		// Fill objectList
		void fill_list(fgmtScriptObject *i_pObject)
		{
			this->comboBox_Fragments->Items->Clear();
			this->comboBox_Fragments->Text = "";

			// Disable until a fragment is selected
			this->button_OverrideFragments->Enabled = false;
			//this->checkBox_HighlightSurface->Enabled = false;
			this->button_SetAll->Enabled = false;

			this->button_ApplyAOToAll->Enabled = false;
			this->button_SaveAOTextures->Enabled = false;

			if (i_pObject)
			{
				const int num_items = i_pObject->GetNumFragments();
				for (int i=0; i<num_items; i++)
				{	
					this->comboBox_Fragments->Items->Add( gcnew String(i_pObject->GetFragmentName(i).c_str()) );
				}	

				// Only allow override if we have't done it already
				if (num_items == 0)
				{
					this->button_OverrideFragments->Enabled = true;
				}
			}
			
		}

private: System::Void button_OverrideFragments_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if (m_pObject)
			 {
				 fgmtOperations::OverrideFragments(m_pObject);
				 m_bDisableNotify = true;
				 fill_list(m_pObject);
				 m_bDisableNotify = false;
			  }
		 }
private: System::Void comboBox_Fragments_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 int sel_index = this->comboBox_Fragments->SelectedIndex;
			 fgmtOperations::SetSelectedFragmentIndex(m_pObject, sel_index);

			 //this->checkBox_HighlightFragment->Checked = false;
			 if ( m_pObject && (sel_index >= 0) )
			 {
				 prtyFormControlBuilder::BuildForm( panel_SurfaceProperties, m_pObject->GetFragmentUI(sel_index)->GetList(), true );

				this->button_SetAll->Enabled = true;
				this->button_ApplyAOToAll->Enabled = true;
				this->button_SaveAOTextures->Enabled = true;
			 }	
		 }
private: System::Void button_SetAll_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 // Set all fragments to have the same flags:
			fgmtOperations::SetAllFragments();
		 }
private: System::Void button_ApplyAOToAll_Click(System::Object^  sender, System::EventArgs^  e) {
			 // Set all fragments to have the same flags:
			fgmtOperations::SetAllFragmentsAO();
		 }
private: System::Void checkBox_HighlightSurface_CheckedChanged(System::Object^  sender, System::EventArgs^  e) {
			 fgmtOperations::HighlightFragment(checkBox_HighlightSurface->Checked);
		 }
private: System::Void button_SaveAOTextures_Click(System::Object^  sender, System::EventArgs^  e) {
			 if (!m_bDisableNotify)
			 {
				 fgmtOperations::SaveAOTextures( );
			 }
		 }
};
}

#endif // _MANAGED
