#pragma once

//#ifndef CMM_OPERATIONS_HPP
//#include "cmmOperations.hpp"
//#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
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
	/// Summary for cmmCommonDataForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmmCommonDataForm : public System::Windows::Forms::Form
	{
	public: 
		static cmmCommonDataForm^ FormInstance = nullptr;

		cmmCommonDataForm() : m_pObject(NULL)
		{
			m_bOurChange = false;
			m_bDisableNotify = true;
			InitializeComponent();
			SetupData(m_pObject);
			m_bDisableNotify = false;
		}

		// Call this to update form data
		void Update(nameObject *i_pObject)
		{
			SetupData(i_pObject);
			m_pObject = i_pObject;
		}
        
	protected: 
		~cmmCommonDataForm()
		{
			// clear instance
			if (cmmCommonDataForm::FormInstance == this)
				cmmCommonDataForm::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: System::Windows::Forms::TabControl ^  tabControl_base;
	private: System::Windows::Forms::TabPage ^  tabPage_base;
	private: System::Windows::Forms::Label ^  label_name;
	private: System::Windows::Forms::TextBox ^  text_name;

	private: nameObject* m_pObject;
		// Turns off notify callbacks when setting up form
	private: bool m_bDisableNotify;
		// Turns off setting of data fields when we make the change
	private: bool m_bOurChange;
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::Label ^  labelNameId;

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
			this->tabControl_base = gcnew System::Windows::Forms::TabControl();
			this->tabPage_base = gcnew System::Windows::Forms::TabPage();
			this->label_name = gcnew System::Windows::Forms::Label();
			this->text_name = gcnew System::Windows::Forms::TextBox();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->labelNameId = gcnew System::Windows::Forms::Label();
			this->tabControl_base->SuspendLayout();
			this->tabPage_base->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_base
			// 
			this->tabControl_base->Controls->Add(this->tabPage_base);
			this->tabControl_base->Location = System::Drawing::Point(7, 7);
			this->tabControl_base->Name = "tabControl_base";
			this->tabControl_base->SelectedIndex = 0;
			this->tabControl_base->Size = System::Drawing::Size(233, 222);
			this->tabControl_base->TabIndex = 0;
			// 
			// tabPage_base
			// 
			this->tabPage_base->Controls->Add(this->labelNameId);
			this->tabPage_base->Controls->Add(this->label1);
			this->tabPage_base->Controls->Add(this->label_name);
			this->tabPage_base->Controls->Add(this->text_name);
			this->tabPage_base->Location = System::Drawing::Point(4, 22);
			this->tabPage_base->Name = "tabPage_base";
			this->tabPage_base->Size = System::Drawing::Size(225, 196);
			this->tabPage_base->TabIndex = 0;
			this->tabPage_base->Text = "Base";
			// 
			// label_name
			// 
			this->label_name->Location = System::Drawing::Point(7, 14);
			this->label_name->Name = "label_name";
			this->label_name->Size = System::Drawing::Size(46, 21);
			this->label_name->TabIndex = 2;
			this->label_name->Text = "Name:";
			// 
			// text_name
			// 
			this->text_name->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->text_name->Location = System::Drawing::Point(67, 14);
			this->text_name->Name = "text_name";
			this->text_name->Size = System::Drawing::Size(140, 20);
			this->text_name->TabIndex = 1;
			this->text_name->Text = "";
			this->text_name->TextChanged += gcnew System::EventHandler(this, &cmmCommonDataForm::text_name_TextChanged);
			this->text_name->Leave += gcnew System::EventHandler(this, &cmmCommonDataForm::text_name_Leave);
			// 
			// label1
			// 
			this->label1->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Italic, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
			this->label1->Location = System::Drawing::Point(8, 48);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(104, 24);
			this->label1->TabIndex = 3;
			this->label1->Text = "Internal Name ID:";
			// 
			// labelNameId
			// 
			this->labelNameId->Location = System::Drawing::Point(128, 48);
			this->labelNameId->Name = "labelNameId";
			this->labelNameId->Size = System::Drawing::Size(80, 24);
			this->labelNameId->TabIndex = 4;
			// 
			// cmmCommonDataForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(243, 231);
			this->Controls->Add(this->tabControl_base);
			this->Name = "cmmCommonDataForm";
			this->Text = "cmmCommonDataForm";
			this->tabControl_base->ResumeLayout(false);
			this->tabPage_base->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		//

		void SetupData(nameObject *i_pObject)
		{
			if (m_bOurChange) return;

			if (!i_pObject)
			{
				text_name->Enabled = false;
			}
			else
			{
				text_name->Enabled = true;

				m_bDisableNotify = true;
				text_name->Text = gcnew System::String(i_pObject->GetName().GetString().c_str());
				Int32 uid = i_pObject->GetName().GetUID();
				labelNameId->Text = uid.ToString();
				m_bDisableNotify = false;
			}
		}

private: System::Void text_name_TextChanged(System::Object ^  sender, System::EventArgs ^  e)
		{
			/*if (!m_bDisableNotify && m_pObject) 
			{
				std::string name;
				tmaManagedStringUtils::ManagedStringToStdString( text_name->Text, name );
				m_pObject->UpdateName( name );
			}*/
		}

private: System::Void text_name_Leave(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 //Note: [bga] - I have removed this code because the above calls to
			 // UpdateName should accompish all the name changing that we need
			 // and because there is a case on exiting where the name field 
			 // is "left" but the object has already been deleted and it causes
			 // a crash.

			// here is where a system's list dialog needs to get notified that
			// the name has changed and the list needs to be updated.
			// This is accomplished through the virtual "UpdateName()" function
			//
			//if ( !m_bDisableNotify && m_pObject ) 
			//{
			//	std::string name;
			//	tmaManagedStringUtils::ManagedStringToStdString( text_name->Text, name );

			//	//	only update the name if it has changed.
			//	//
			//	if ( !(m_pObject->GetName() == nameString(name)) )
			//	{
			//		m_pObject->UpdateName( name );
			//	}
			//}
		 }

	public:
    	//	get a pointer to a tab page
		//
		//	note: this system has only one tab page so we can ignore the index
		System::Windows::Forms::TabPage ^ GetTabPage( int i_Index )
		{
			return tabPage_base;
		}
};

}
#endif // _MANAGED
