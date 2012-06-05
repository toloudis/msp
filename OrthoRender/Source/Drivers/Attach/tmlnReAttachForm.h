#pragma once

#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifndef NAME_MGR_HPP
#include "Core/name/nameMgr.hpp"
#endif
#ifndef NAME_OBJECT_HPP
#include "Core/name/nameObject.hpp"
#endif
#ifndef NAME_STRING_HPP
#include "Core/name/nameString.hpp"
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
	/// Summary for tmlnReAttachForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class tmlnReAttachForm : public System::Windows::Forms::Form
	{
	public: 
		tmlnReAttachForm(const std::string &i_OldName)
		{
			InitializeComponent();

			SetupComponents();

			this->label_Name->Text = gcnew System::String(i_OldName.c_str());
		}

		void GetAttachObject(std::string &o_String)
		{
			if (this->listBox_Objects->SelectedIndex >= 0)
			{
				tmaManagedStringUtils::ManagedStringToStdString(
					this->listBox_Objects->Text, o_String);
			}
		}
        
	public: 
		~tmlnReAttachForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox ^  listBox_Objects;
	private: System::Windows::Forms::Button ^  buttonOK;
	private: System::Windows::Forms::Button ^  buttonCancel;
	private: System::Windows::Forms::Label ^  label_Name;
	private: System::Windows::Forms::Label ^  label1;

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
			this->listBox_Objects = gcnew System::Windows::Forms::ListBox();
			this->buttonOK = gcnew System::Windows::Forms::Button();
			this->buttonCancel = gcnew System::Windows::Forms::Button();
			this->label_Name = gcnew System::Windows::Forms::Label();
			this->label1 = gcnew System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// listBox_Objects
			// 
			this->listBox_Objects->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox_Objects->Location = System::Drawing::Point(8, 64);
			this->listBox_Objects->Name = "listBox_Objects";
			this->listBox_Objects->Size = System::Drawing::Size(276, 147);
			this->listBox_Objects->TabIndex = 0;
			// 
			// buttonOK
			// 
			this->buttonOK->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonOK->Location = System::Drawing::Point(40, 232);
			this->buttonOK->Name = "buttonOK";
			this->buttonOK->Size = System::Drawing::Size(72, 24);
			this->buttonOK->TabIndex = 1;
			this->buttonOK->Text = "OK";
			this->buttonOK->Click += gcnew System::EventHandler(this, &tmlnReAttachForm::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(160, 232);
			this->buttonCancel->Name = "buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(88, 24);
			this->buttonCancel->TabIndex = 2;
			this->buttonCancel->Text = "Cancel";
			// 
			// label_Name
			// 
			this->label_Name->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label_Name->Font = gcnew System::Drawing::Font("Microsoft Sans Serif", 8.25F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
			this->label_Name->Location = System::Drawing::Point(16, 8);
			this->label_Name->Name = "label_Name";
			this->label_Name->Size = System::Drawing::Size(268, 16);
			this->label_Name->TabIndex = 3;
			this->label_Name->Text = "Name";
			this->label_Name->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// label1
			// 
			this->label1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->label1->Location = System::Drawing::Point(8, 24);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(284, 32);
			this->label1->TabIndex = 4;
			this->label1->Text = "cannot be found. Choose a new object to attach driver or Press Cancel";
			this->label1->TextAlign = System::Drawing::ContentAlignment::TopCenter;
			// 
			// tmlnReAttachForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(296, 266);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->label_Name);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->listBox_Objects);
			this->Name = "tmlnReAttachForm";
			this->Text = "Re-Attach Driver";
			this->ResumeLayout(false);

		}		
		//

private:
	//	Set-up the components on construction
	//
	void SetupComponents()
	{
		//	fill-in combobox
		//
		nameList aNameList;
		nameMgr::GetNameList( aNameList );
		const int num_names = aNameList.size();
		for ( int i = 0; i < num_names; i++ )
		{
			//DBG_LOG2( "%02d) %s", i, aNameList[i]->GetString().c_str() );
	
			this->listBox_Objects->Items->Add( gcnew String(aNameList[i]->GetString().c_str()) );
		}
	}

	private: System::Void buttonOK_Click(System::Object ^  sender, System::EventArgs ^  e)
	{
		if (this->listBox_Objects->SelectedIndex >= 0)
		{
			this->DialogResult = ::DialogResult::OK;
			this->Close();
		}
		else
		{
			MessageBox::Show("Choose a name to reattach or press Cancel.");
		}
	}


};
}
#endif // _MANAGED
