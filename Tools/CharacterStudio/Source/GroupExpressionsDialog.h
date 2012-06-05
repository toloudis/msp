#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

#ifndef TMA_MANAGEDSTRINGUTILS_HPP
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#endif

#include <string>
#include <vector>

namespace CharacterStudio {

	/// <summary>
	/// Summary for GroupExpressionsDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class GroupExpressionsDialog : public System::Windows::Forms::Form
	{
	public:
		GroupExpressionsDialog(const std::vector<std::string> &i_ExpressionNames)
		{
			InitializeComponent();
			
			load_combo(this->comboBox_Left, i_ExpressionNames);
			load_combo(this->comboBox_Right, i_ExpressionNames);
			load_combo(this->comboBox_Down, i_ExpressionNames);
			load_combo(this->comboBox_Up, i_ExpressionNames);
		}

		std::string GetGroupName()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString( this->textBox_Name->Text, str);
			return str;
		}
		std::string GetLeftExpression()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_Left->Text, str);
			return str;
		}
		std::string GetRightExpression()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_Right->Text, str);
			return str;
		}
		bool IsFourGroup()
		{
			return this->checkBox_Four->Checked;
		}
		std::string GetDownExpression()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_Down->Text, str);
			return str;
		}
		std::string GetUpExpression()
		{
			std::string str;
			tmaManagedStringUtils::ManagedStringToStdString( this->comboBox_Up->Text, str);
			return str;
		}


	private:
		void load_combo(System::Windows::Forms::ComboBox^  i_ComboBox,
						const std::vector<std::string> &i_ExpressionNames)
		{
			for (int i=0; i<i_ExpressionNames.size(); i++)
				i_ComboBox->Items->Add( gcnew System::String(i_ExpressionNames[i].c_str()) );
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~GroupExpressionsDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::CheckBox^  checkBox_Four;
	private: System::Windows::Forms::ComboBox^  comboBox_Left;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::ComboBox^  comboBox_Right;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::ComboBox^  comboBox_Up;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::ComboBox^  comboBox_Down;
	private: System::Windows::Forms::Button^  buttonOk;
	private: System::Windows::Forms::TextBox^  textBox_Name;

	private: System::Windows::Forms::Label^  label5;
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
			this->checkBox_Four = (gcnew System::Windows::Forms::CheckBox());
			this->comboBox_Left = (gcnew System::Windows::Forms::ComboBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->comboBox_Right = (gcnew System::Windows::Forms::ComboBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->comboBox_Up = (gcnew System::Windows::Forms::ComboBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->comboBox_Down = (gcnew System::Windows::Forms::ComboBox());
			this->buttonOk = (gcnew System::Windows::Forms::Button());
			this->textBox_Name = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// checkBox_Four
			// 
			this->checkBox_Four->AutoSize = true;
			this->checkBox_Four->Enabled = false;
			this->checkBox_Four->Location = System::Drawing::Point(15, 183);
			this->checkBox_Four->Name = L"checkBox_Four";
			this->checkBox_Four->Size = System::Drawing::Size(230, 17);
			this->checkBox_Four->TabIndex = 0;
			this->checkBox_Four->Text = L"Four directions (not enabled at the moment)";
			this->checkBox_Four->UseVisualStyleBackColor = true;
			this->checkBox_Four->Visible = false;
			this->checkBox_Four->CheckedChanged += gcnew System::EventHandler(this, &GroupExpressionsDialog::checkBox_Four_CheckedChanged);
			// 
			// comboBox_Left
			// 
			this->comboBox_Left->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Left->FormattingEnabled = true;
			this->comboBox_Left->Location = System::Drawing::Point(60, 69);
			this->comboBox_Left->Name = L"comboBox_Left";
			this->comboBox_Left->Size = System::Drawing::Size(211, 21);
			this->comboBox_Left->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(12, 72);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(28, 13);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Left:";
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(12, 99);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(35, 13);
			this->label2->TabIndex = 4;
			this->label2->Text = L"Right:";
			// 
			// comboBox_Right
			// 
			this->comboBox_Right->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Right->FormattingEnabled = true;
			this->comboBox_Right->Location = System::Drawing::Point(60, 96);
			this->comboBox_Right->Name = L"comboBox_Right";
			this->comboBox_Right->Size = System::Drawing::Size(211, 21);
			this->comboBox_Right->TabIndex = 3;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(15, 238);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(24, 13);
			this->label3->TabIndex = 8;
			this->label3->Text = L"Up:";
			this->label3->Visible = false;
			// 
			// comboBox_Up
			// 
			this->comboBox_Up->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Up->Enabled = false;
			this->comboBox_Up->FormattingEnabled = true;
			this->comboBox_Up->Location = System::Drawing::Point(63, 235);
			this->comboBox_Up->Name = L"comboBox_Up";
			this->comboBox_Up->Size = System::Drawing::Size(211, 21);
			this->comboBox_Up->TabIndex = 7;
			this->comboBox_Up->Visible = false;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(15, 211);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(38, 13);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Down:";
			this->label4->Visible = false;
			// 
			// comboBox_Down
			// 
			this->comboBox_Down->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->comboBox_Down->Enabled = false;
			this->comboBox_Down->FormattingEnabled = true;
			this->comboBox_Down->Location = System::Drawing::Point(63, 208);
			this->comboBox_Down->Name = L"comboBox_Down";
			this->comboBox_Down->Size = System::Drawing::Size(211, 21);
			this->comboBox_Down->TabIndex = 5;
			this->comboBox_Down->Visible = false;
			// 
			// buttonOk
			// 
			this->buttonOk->Location = System::Drawing::Point(103, 127);
			this->buttonOk->Name = L"buttonOk";
			this->buttonOk->Size = System::Drawing::Size(87, 31);
			this->buttonOk->TabIndex = 9;
			this->buttonOk->Text = L"Create Group";
			this->buttonOk->UseVisualStyleBackColor = true;
			this->buttonOk->Click += gcnew System::EventHandler(this, &GroupExpressionsDialog::buttonOk_Click);
			// 
			// textBox_Name
			// 
			this->textBox_Name->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->textBox_Name->Location = System::Drawing::Point(60, 15);
			this->textBox_Name->Name = L"textBox_Name";
			this->textBox_Name->Size = System::Drawing::Size(210, 20);
			this->textBox_Name->TabIndex = 10;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(12, 15);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(38, 13);
			this->label5->TabIndex = 11;
			this->label5->Text = L"Name:";
			// 
			// GroupExpressionsDialog
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(287, 177);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->textBox_Name);
			this->Controls->Add(this->buttonOk);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->comboBox_Up);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->comboBox_Down);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->comboBox_Right);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->comboBox_Left);
			this->Controls->Add(this->checkBox_Four);
			this->Name = L"GroupExpressionsDialog";
			this->Text = L"Group Expressions";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

private: System::Void checkBox_Four_CheckedChanged(System::Object^  sender, System::EventArgs^  e) 
		 {
			 this->comboBox_Down->Enabled = this->checkBox_Four->Checked;
			 this->comboBox_Up->Enabled = this->checkBox_Four->Checked;
		 }

private: System::Void buttonOk_Click(System::Object^  sender, System::EventArgs^  e) 
		 {
			 if (this->textBox_Name->Text->Length == 0)
			 {
				 MessageBox::Show("Need to assign name to group.", "Need Name", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			 }
			 else if (this->comboBox_Left->Text->Length == 0)
			 {
				 MessageBox::Show("Need to assign left expression.", "Need Left Expression", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			 }
			 else if (this->comboBox_Right->Text->Length == 0)
			 {
				 MessageBox::Show("Need to assign right expression.", "Need Right Expression", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			 }
			 else if (this->checkBox_Four->Checked && (this->comboBox_Down->Text->Length == 0))
			 {
				 MessageBox::Show("Need to assign down expression.", "Need Down Expression", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			 }
			 else if (this->checkBox_Four->Checked && (this->comboBox_Up->Text->Length == 0))
			 {
				 MessageBox::Show("Need to assign up expression.", "Need Up Expression", 
					MessageBoxButtons::OK, MessageBoxIcon::Warning);
			 }
			 else 
			 {
				 this->DialogResult = ::DialogResult::OK;
			 }
		 }
};
}
