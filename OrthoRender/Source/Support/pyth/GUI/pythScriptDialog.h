#pragma once

#ifndef PYTH_UTIL_HPP
#include "Support/pyth/pythUtil.hpp"
#endif
#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
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


namespace StudioFramework 
{

	/// <summary>
	/// Summary for pythScriptDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class pythScriptDialog : public System::Windows::Forms::Form
	{
	public:
		static pythScriptDialog^ FormInstance = nullptr;
		static System::String^ LogString = nullptr;

		pythScriptDialog(void)
		{
			InitializeComponent();

			m_pMemory = gcnew tmaDialogMemory( this );

			// If we have some log strings already, add them to 
			// to the textbox
			if (pythScriptDialog::LogString != nullptr)
			{
				this->textBox_pythLog->Text = pythScriptDialog::LogString + gcnew System::String("\r\n");
				pythScriptDialog::LogString = nullptr;
			}
		}
		
		void AddToLogTextbox(System::String^ i_pString)
		{
			this->textBox_pythLog->Text += i_pString;

			// Haver to focus the text box in order to move the caret position
			if (this->button_executePyth->Focused ||
				this->textBox_pythCommand->Focused)
				this->textBox_pythLog->Focus();

			if (this->textBox_pythLog->Focused)
			{
				this->textBox_pythLog->SelectionStart = this->textBox_pythLog->Text->Length;
				this->textBox_pythLog->SelectionLength = 0;
				this->textBox_pythLog->ScrollToCaret();

				this->textBox_pythCommand->Focus();
			}
		}

		static void AddToLog(System::String^ i_pString)
		{
			if (FormInstance != nullptr)
			{
				FormInstance->AddToLogTextbox(i_pString);
			}
			else 
			{
				// If we haven't created the form yet, just
				// store the string
				if (pythScriptDialog::LogString == nullptr)
					pythScriptDialog::LogString = gcnew System::String(i_pString);
				else 
					pythScriptDialog::LogString->Concat(i_pString);
			}
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~pythScriptDialog()
		{
			// clear instance
			if (pythScriptDialog::FormInstance == this)
				pythScriptDialog::FormInstance = nullptr;

			if (components)
			{
				delete components;
			}
		}

	private: tmaDialogMemory^ m_pMemory;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Button^  button_clearPyth;

	private: System::Windows::Forms::Button^  button_executePyth;

	private: System::Windows::Forms::TextBox^  textBox_pythCommand;

	private: System::Windows::Forms::TextBox^  textBox_pythLog;


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
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->button_clearPyth = (gcnew System::Windows::Forms::Button());
			this->button_executePyth = (gcnew System::Windows::Forms::Button());
			this->textBox_pythCommand = (gcnew System::Windows::Forms::TextBox());
			this->textBox_pythLog = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// label2
			// 
			this->label2->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(18, 310);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(85, 13);
			this->label2->TabIndex = 20;
			this->label2->Text = L"Python Comand:";
			// 
			// button_clearPyth
			// 
			this->button_clearPyth->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button_clearPyth->Location = System::Drawing::Point(328, 361);
			this->button_clearPyth->Name = L"button_clearPyth";
			this->button_clearPyth->Size = System::Drawing::Size(78, 24);
			this->button_clearPyth->TabIndex = 19;
			this->button_clearPyth->Text = L"Clear";
			this->button_clearPyth->UseVisualStyleBackColor = true;
			this->button_clearPyth->Click += gcnew System::EventHandler(this, &pythScriptDialog::button_clearPyth_Click);
			// 
			// button_executePyth
			// 
			this->button_executePyth->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Right));
			this->button_executePyth->Location = System::Drawing::Point(328, 330);
			this->button_executePyth->Name = L"button_executePyth";
			this->button_executePyth->Size = System::Drawing::Size(79, 25);
			this->button_executePyth->TabIndex = 18;
			this->button_executePyth->Text = L"Execute";
			this->button_executePyth->UseVisualStyleBackColor = true;
			this->button_executePyth->Click += gcnew System::EventHandler(this, &pythScriptDialog::button_executePyth_Click);
			// 
			// textBox_pythCommand
			// 
			this->textBox_pythCommand->AcceptsReturn = true;
			this->textBox_pythCommand->AcceptsTab = true;
			this->textBox_pythCommand->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_pythCommand->Location = System::Drawing::Point(12, 328);
			this->textBox_pythCommand->Multiline = true;
			this->textBox_pythCommand->Name = L"textBox_pythCommand";
			this->textBox_pythCommand->Size = System::Drawing::Size(310, 59);
			this->textBox_pythCommand->TabIndex = 17;
			this->textBox_pythCommand->KeyPress += gcnew System::Windows::Forms::KeyPressEventHandler(this, &pythScriptDialog::textBox_pythCommand_KeyPress);
			// 
			// textBox_pythLog
			// 
			this->textBox_pythLog->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_pythLog->Location = System::Drawing::Point(12, 12);
			this->textBox_pythLog->Multiline = true;
			this->textBox_pythLog->Name = L"textBox_pythLog";
			this->textBox_pythLog->ReadOnly = true;
			this->textBox_pythLog->ScrollBars = System::Windows::Forms::ScrollBars::Vertical;
			this->textBox_pythLog->Size = System::Drawing::Size(395, 285);
			this->textBox_pythLog->TabIndex = 16;
			// 
			// pythScriptDialog
			// 
			this->ClientSize = System::Drawing::Size(419, 404);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button_clearPyth);
			this->Controls->Add(this->button_executePyth);
			this->Controls->Add(this->textBox_pythCommand);
			this->Controls->Add(this->textBox_pythLog);
			this->Name = L"pythScriptDialog";
			this->ShowInTaskbar = false;
			this->Text = L"Python";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	
	private:
		void ExecuteCommand()
		{
			System::String ^pStr = this->textBox_pythCommand->Text;
			pStr = pStr->Trim();

			System::String ^pLog = gcnew System::String(">> ");
			pLog += pStr->Replace("\r\n", "\r\n> "); 
			pLog += gcnew System::String("\r\n");
			this->AddToLogTextbox(pLog);

			pStr = pStr->Replace("\r\n", "\n");

			std::string command;
			tmaManagedStringUtils::ManagedStringToStdString(pStr, command);

			if (pythUtil::ExecuteCommand( command ))
			{
				// If successful, clear text
				this->textBox_pythCommand->Text = gcnew System::String("");
			}
			else
			{
				//MessageBox::Show(gcnew System::String("Error running command: \n") + pStr,
				//	"Error running message", MessageBoxButtons::OK, MessageBoxIcon::Error);
			}
		}


private: System::Void textBox_pythCommand_KeyPress(System::Object^  sender, System::Windows::Forms::KeyPressEventArgs^  e) 
		 {		 
			 // Handle CTRL-ENTER as "execute script"
			 if ( Control::ModifierKeys == Keys::Control )
			 {
				if (e->KeyChar == (char)10 )
				{
					 e->Handled = true;
					 this->ExecuteCommand();
				}
			 }
		 }
private: System::Void button_executePyth_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->ExecuteCommand();
		 }
private: System::Void button_clearPyth_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->textBox_pythCommand->Text = gcnew System::String("");
			 this->textBox_pythLog->Text = gcnew System::String("");
		 }
};
}

#endif // _MANAGED
