#pragma once

#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif
#ifdef _MANAGED

//============================================================================
//============================================================================
using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


//============================================================================
//============================================================================
namespace StudioFramework
{
	/// <summary> 
	/// Summary for cmmEnterNameForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmmEnterNameForm : public System::Windows::Forms::Form
	{
	public: 
		cmmEnterNameForm(System::String^ i_DialogTitle, System::String^ i_InitialName)
		{
			InitializeComponent();

			m_pCheckIfNameValidFunction = nullptr;
			this->Text = i_DialogTitle;
			textBox_name->Text = i_InitialName;
		}

		~cmmEnterNameForm()
		{
			if (components)
			{
				delete components;
			}
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		System::String^ GetName()
		{
			return textBox_name->Text;
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
		typedef bool (*CheckIfNameValidFunctionPtr)(const std::string &i_Name);

		void SetNameCheckFunction( CheckIfNameValidFunctionPtr i_pCheckIfNameValidFunction)
		{
			m_pCheckIfNameValidFunction = i_pCheckIfNameValidFunction;
		}

		//--------------------------------------------------------------------------
		//--------------------------------------------------------------------------
	private: System::Windows::Forms::Label ^  label_name;
	private: System::Windows::Forms::TextBox ^  textBox_name;
	private: System::Windows::Forms::Button ^  button_OK;
	private: System::Windows::Forms::Button ^  button_cancel;

	//--------------------------------------------------------------------
	// This command calls functions with no arguments
	//--------------------------------------------------------------------
	private:
		CheckIfNameValidFunctionPtr m_pCheckIfNameValidFunction;

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
			this->label_name = gcnew System::Windows::Forms::Label();
			this->textBox_name = gcnew System::Windows::Forms::TextBox();
			this->button_OK = gcnew System::Windows::Forms::Button();
			this->button_cancel = gcnew System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// label_name
			// 
			this->label_name->Location = System::Drawing::Point(16, 16);
			this->label_name->Name = "label_name";
			this->label_name->Size = System::Drawing::Size(104, 23);
			this->label_name->TabIndex = 0;
			this->label_name->Text = "Name:";
			this->label_name->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
			// 
			// textBox_name
			// 
			this->textBox_name->AcceptsReturn = true;
			this->textBox_name->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox_name->Location = System::Drawing::Point(128, 16);
			this->textBox_name->Name = "textBox_name";
			this->textBox_name->Size = System::Drawing::Size(200, 20);
			this->textBox_name->TabIndex = 1;
			this->textBox_name->Text = "";
			this->textBox_name->KeyUp += gcnew System::Windows::Forms::KeyEventHandler(this, &cmmEnterNameForm::textBox_name_KeyUp);
			// 
			// button_OK
			// 
			this->button_OK->Location = System::Drawing::Point(80, 48);
			this->button_OK->Name = "button_OK";
			this->button_OK->Size = System::Drawing::Size(64, 24);
			this->button_OK->TabIndex = 2;
			this->button_OK->Text = "OK";
			this->button_OK->Click += gcnew System::EventHandler(this, &cmmEnterNameForm::button_OK_Click);
			// 
			// button_cancel
			// 
			this->button_cancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->button_cancel->Location = System::Drawing::Point(176, 48);
			this->button_cancel->Name = "button_cancel";
			this->button_cancel->Size = System::Drawing::Size(88, 24);
			this->button_cancel->TabIndex = 3;
			this->button_cancel->Text = "Cancel";
			// 
			// cmmEnterNameForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(344, 86);
			this->Controls->Add(this->button_cancel);
			this->Controls->Add(this->button_OK);
			this->Controls->Add(this->textBox_name);
			this->Controls->Add(this->label_name);
			this->Name = "cmmEnterNameForm";
			this->Text = "Name";
			this->ResumeLayout(false);

		}		

		//

		void handle_ok()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString(textBox_name->Text, str);

			if (m_pCheckIfNameValidFunction != nullptr)
			{
				if ( (*m_pCheckIfNameValidFunction)(str) )
				{
					this->DialogResult = ::DialogResult::OK;
					this->Close();
				}
				else
				{
					MessageBox::Show("Please enter a valid name.");
				}
			}
		}

	private: System::Void button_OK_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 handle_ok();
			 }

	private: System::Void textBox_name_KeyUp(System::Object ^  sender, System::Windows::Forms::KeyEventArgs ^  e)
			 {
				if (e->KeyCode == System::Windows::Forms::Keys::Return)
				{
					handle_ok();
				}
			 }

};
}
#endif // _MANAGED
