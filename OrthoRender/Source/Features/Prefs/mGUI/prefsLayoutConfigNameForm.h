#pragma once

//#ifndef TMA_DIALOGMEMORY_HPP
//#include "ToolUIManaged/tma/tmaDialogMemory.hpp"
//#endif
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
	/// Summary for prefsLayoutConfigNameForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class prefsLayoutConfigNameForm : public System::Windows::Forms::Form
	{
	public: static prefsLayoutConfigNameForm^ FormInstance = nullptr;
	public: System::String^ GetConfigName()
			{
				return this->textBox_name->Text;
			}
	public: 
		prefsLayoutConfigNameForm(void)
		{
			InitializeComponent();
		}
        
	public: 
		~prefsLayoutConfigNameForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox ^  textBox_name;

	private: System::Windows::Forms::Label ^  label_name;
	private: System::Windows::Forms::Button ^  button_OK;
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
			this->textBox_name = gcnew System::Windows::Forms::TextBox();
			this->button_OK = gcnew System::Windows::Forms::Button();
			this->label_name = gcnew System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// textBox_name
			// 
			this->textBox_name->Location = System::Drawing::Point(16, 32);
			this->textBox_name->Name = "textBox_name";
			this->textBox_name->Size = System::Drawing::Size(216, 20);
			this->textBox_name->TabIndex = 0;
			this->textBox_name->Text = "";
			// 
			// button_OK
			// 
			this->button_OK->Location = System::Drawing::Point(80, 64);
			this->button_OK->Name = "button_OK";
			this->button_OK->TabIndex = 1;
			this->button_OK->Text = "OK";
			this->button_OK->Click += gcnew System::EventHandler(this, &prefsLayoutConfigNameForm::button_OK_Click);
			// 
			// label_name
			// 
			this->label_name->Location = System::Drawing::Point(16, 8);
			this->label_name->Name = "label_name";
			this->label_name->Size = System::Drawing::Size(264, 23);
			this->label_name->TabIndex = 2;
			this->label_name->Text = "Enter the Name for this Layout Configuration";
			// 
			// prefsLayoutConfigNameForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(248, 94);
			this->Controls->Add(this->label_name);
			this->Controls->Add(this->button_OK);
			this->Controls->Add(this->textBox_name);
			this->Name = "prefsLayoutConfigNameForm";
			this->Text = "Layout Configuration Name";
			this->Load += gcnew System::EventHandler(this, &prefsLayoutConfigNameForm::prefsLayoutConfigNameForm_Load);
			this->ResumeLayout(false);

		}		
	private: System::Void button_OK_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 this->Close();
			 }

	private: System::Void prefsLayoutConfigNameForm_Load(System::Object ^  sender, System::EventArgs ^  e)
			 {
//				m_pMemory = gcnew tmaDialogMemory( this );
			 }
	};
}
#endif // _MANAGED
