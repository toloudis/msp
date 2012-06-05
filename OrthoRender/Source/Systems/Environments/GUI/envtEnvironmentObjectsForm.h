#pragma once

#ifndef EVMT_ENVIRONMENTMGR_HPP
#include "Support/evmt/evmtEnvironmentMgr.hpp"
#endif
#ifndef ENVT_OPERATIONS_HPP
#include "Systems/Environments/Undo/envtOperations.hpp"
#endif
#ifndef ENVT_SCRIPTOBJECT_HPP
#include "Systems/Environments/Object/envtScriptObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
#endif
#ifndef TMA_DIALOGMEMORY_HPP
#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
#endif


#include <algorithm>

#ifdef _MANAGED


using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SystemEnvironments
{

	/// <summary> 
	/// Summary for envtEnvironmentObjectsForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class envtEnvironmentObjectsForm : public System::Windows::Forms::Form
	{
	public: 
		static envtEnvironmentObjectsForm^ FormInstance = nullptr;

		envtEnvironmentObjectsForm()
			: m_pObject(NULL)
		{
			InitializeComponent();

			SetUpComponents();
		}

		void SetUpComponents()
		{
			// Dialog memory remembers size, location, visiblity of dialog 
			m_pMemory = gcnew tmaDialogMemory( this );
		}

		void fill_checked_box()
		{
			this->checkedListBoxObjects->Items->Clear();

			std::vector<nameString> all_objects;
			evmtEnvironmentMgr::GetAllObjects(all_objects);

			std::vector<nameString> set_objects;
			if (m_pObject)
				m_pObject->GetObjectsInEnvironment(set_objects);

			const int num_objects = all_objects.size();
			for (int i=0; i<num_objects; i++)
			{
				std::vector<nameString>::const_iterator it = std::find(set_objects.begin(), set_objects.end(), all_objects[i]);
				bool checked = (it != set_objects.end());

				this->checkedListBoxObjects->Items->Add(
					gcnew System::String(all_objects[i].GetString().c_str()), checked );
			}
		}
		
		void Update(envtScriptObject* i_pObject)
		{
			m_pObject = i_pObject;
			m_bDisableNotify = true;
			if (i_pObject)
			{
				fill_checked_box();
			}
			m_bDisableNotify = false;
		}

		//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_objects;
		}
        
	public: 
		~envtEnvironmentObjectsForm()
		{
			// clear instance
			if (envtEnvironmentObjectsForm::FormInstance == this)
				envtEnvironmentObjectsForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}




	private: envtScriptObject* m_pObject;
	private: bool m_bDisableNotify;
	private: System::Windows::Forms::TabControl^  tabControl1;
	private: System::Windows::Forms::TabPage^  tabPage_objects;

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
			this->tabControl1 = (gcnew System::Windows::Forms::TabControl());
			this->tabPage_objects = (gcnew System::Windows::Forms::TabPage());
			this->checkedListBoxObjects = (gcnew System::Windows::Forms::CheckedListBox());
			this->tabControl1->SuspendLayout();
			this->tabPage_objects->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->tabControl1->Controls->Add(this->tabPage_objects);
			this->tabControl1->Location = System::Drawing::Point(2, 2);
			this->tabControl1->Name = L"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(422, 420);
			this->tabControl1->TabIndex = 0;
			// 
			// tabPage_objects
			// 
			this->tabPage_objects->Controls->Add(this->checkedListBoxObjects);
			this->tabPage_objects->Location = System::Drawing::Point(4, 22);
			this->tabPage_objects->Name = L"tabPage_objects";
			this->tabPage_objects->Padding = System::Windows::Forms::Padding(3);
			this->tabPage_objects->Size = System::Drawing::Size(414, 394);
			this->tabPage_objects->TabIndex = 0;
			this->tabPage_objects->Text = L"Objects";
			this->tabPage_objects->UseVisualStyleBackColor = true;
			// 
			// checkedListBoxObjects
			// 
			this->checkedListBoxObjects->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->checkedListBoxObjects->CheckOnClick = true;
			this->checkedListBoxObjects->Location = System::Drawing::Point(4, 15);
			this->checkedListBoxObjects->Name = L"checkedListBoxObjects";
			this->checkedListBoxObjects->Size = System::Drawing::Size(407, 364);
			this->checkedListBoxObjects->TabIndex = 1;
			this->checkedListBoxObjects->ItemCheck += gcnew System::Windows::Forms::ItemCheckEventHandler(this, 
				&envtEnvironmentObjectsForm::checkedListBoxObjects_ItemCheck);
			// 
			// envtEnvironmentObjectsForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(427, 422);
			this->Controls->Add(this->tabControl1);
			this->Name = L"envtEnvironmentObjectsForm";
			this->Text = L"Environment Objects";
			this->tabControl1->ResumeLayout(false);
			this->tabPage_objects->ResumeLayout(false);
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
				evmtEnvironmentMgr::GetAllObjects(all_objects);

				if (e->NewValue == CheckState::Checked)
				{
					envtOperations::AddObjectToEnvironment(m_pObject, all_objects[index]);
				}
				else
				{
					envtOperations::RemoveObjectFromEnvironment(m_pObject, all_objects[index]);
				}
			}
		}

	private:
		tmaDialogMemory^	m_pMemory;
	};
}
#endif // _MANAGED
