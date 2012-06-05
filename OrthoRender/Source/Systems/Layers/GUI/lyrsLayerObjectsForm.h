#pragma once

#ifndef LYER_LAYERMGR_HPP
#include "Support/lyer/lyerLayerMgr.hpp"
#endif
#ifndef LYRS_OPERATIONS_HPP
#include "Systems/Layers/Undo/lyrsOperations.hpp"
#endif
#ifndef ENV_STLHELPERS_HPP
#include "Core/env/envSTLHelpers.hpp"
#endif


#include <algorithm>

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemLayers
{

	/// <summary> 
	/// Summary for lyrsLayerObjectsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class lyrsLayerObjectsForm : public System::Windows::Forms::Form
	{
	public: 
		static lyrsLayerObjectsForm^ FormInstance = nullptr;

		lyrsLayerObjectsForm()
		: m_pLayerName(NULL)
		{
			InitializeComponent();
		}
		
		void Update(const nameString &i_LayerName)
		{
			if (m_pLayerName != NULL)
				delete m_pLayerName;
			m_pLayerName = new nameString(i_LayerName);

			m_bDisableNotify = true;
			labelLayerName->Text = gcnew System::String(i_LayerName.GetString().c_str());
			fill_checked_box();
			m_bDisableNotify = false;
		}

		//	get a pointer to a tab page
		//
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_Objects;
		}

		void fill_checked_box()
		{
			this->checkedListBoxObjects->Items->Clear();

			std::vector<nameString> all_objects;
			lyerLayerMgr::GetAllObjects(all_objects);

			std::vector<nameString> set_objects;
			lyerLayerMgr::GetObjectsInLayer(*m_pLayerName, set_objects);

			const int num_objects = all_objects.size();
			for (int i=0; i<num_objects; i++)
			{
				std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
				bool checked = (it != set_objects.end());

				this->checkedListBoxObjects->Items->Add(
					gcnew System::String(all_objects[i].GetString().c_str()), checked );
			}
		}

	public: 
		~lyrsLayerObjectsForm()
		{
			if (m_pLayerName != NULL)
				delete m_pLayerName;

			// clear instance
			if (lyrsLayerObjectsForm::FormInstance == this)
				lyrsLayerObjectsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}


	private: System::Windows::Forms::Label ^  labelLayerName;

	private: nameString * m_pLayerName;
	private: bool m_bDisableNotify;

	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage_Objects;



	private: System::Windows::Forms::CheckedListBox^  checkedListBoxObjects;
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
			this->labelLayerName = (gcnew System::Windows::Forms::Label());
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_Objects = (gcnew System::Windows::Forms::TabPage());
			this->checkedListBoxObjects = (gcnew System::Windows::Forms::CheckedListBox());
			this->tabControl1->SuspendLayout();
			this->tabPage_Objects->SuspendLayout();
			this->SuspendLayout();
			// 
			// labelLayerName
			// 
			this->labelLayerName->Location = System::Drawing::Point(16, 8);
			this->labelLayerName->Name = L"labelLayerName";
			this->labelLayerName->Size = System::Drawing::Size(264, 16);
			this->labelLayerName->TabIndex = 1;
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage_Objects);
			this->tabControl1->Location = System::Drawing::Point(11, 11);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(310, 294);
			this->tabControl1->TabIndex = 3;
			// 
			// tabPage_Objects
			// 
			this->tabPage_Objects->Controls->Add(this->checkedListBoxObjects);
			this->tabPage_Objects->Location = System::Drawing::Point(4, 22);
			this->tabPage_Objects->Name = L"tabPage_Objects";
			this->tabPage_Objects->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_Objects->Size = System::Drawing::Size(302, 268);
			this->tabPage_Objects->TabIndex = 0;
			this->tabPage_Objects->Text = L"Objects";
			this->tabPage_Objects->UseVisualStyleBackColor = true;
			// 
			// checkedListBoxObjects
			// 
			this->checkedListBoxObjects->CheckOnClick = true;
			this->checkedListBoxObjects->Dock = System::Windows::Forms::DockStyle::Fill;
			this->checkedListBoxObjects->FormattingEnabled = true;
			this->checkedListBoxObjects->Location = System::Drawing::Point(3, 3);
			this->checkedListBoxObjects->Name = L"checkedListBoxObjects";
			this->checkedListBoxObjects->Size = System::Drawing::Size(296, 259);
			this->checkedListBoxObjects->TabIndex = 0;
			this->checkedListBoxObjects->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, &lyrsLayerObjectsForm::checkedListBoxObjects_ItemCheck);
			// 
			// lyrsLayerObjectsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(325, 308);
			this->Controls->Add(this->tabControl1);
			this->Controls->Add(this->labelLayerName);
			this->Name = L"lyrsLayerObjectsForm";
			this->Text = L"Layer Objects";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_Objects->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

	private: System::Void checkedListBoxObjects_ItemCheck(System::Object ^  sender, System::Windows::Forms::ItemCheckEventArgs ^  e)
		{
			if (m_bDisableNotify) return;

			if (e->NewValue != e->CurrentValue)
			{
				int index = e->Index;
				std::vector<nameString> all_objects;
				lyerLayerMgr::GetAllObjects(all_objects);

				if (e->NewValue == CheckState::Checked)
				{
					lyrsOperations::AddObjectToLayer(*m_pLayerName, all_objects[index]);
				}
				else
				{
					lyrsOperations::RemoveObjectFromLayer(*m_pLayerName, all_objects[index]);
				}
			}
		}

};
}
#endif // _MANAGED
