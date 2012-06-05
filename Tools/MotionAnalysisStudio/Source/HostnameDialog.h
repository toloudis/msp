#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MotionAnalysisStudio
{
	/// <summary> 
	/// Summary for HostnameDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class HostnameDialog : public System::Windows::Forms::Form
	{
	public: 
		HostnameDialog(void)
		{
			InitializeComponent();
		}

		System::String* GetHostname()
		{
			return this->textBox1->Text;
		}
		void SetHostname(System::String* i_String)
		{
			this->textBox1->Text = i_String;
		}

		float GetScaleFactor()
		{
			return (float) this->floatEdit1->Value;
		}
		void SetScaleFactor(float i_Value)
		{
			this->floatEdit1->Value = i_Value;
		}
        
	protected: 
		void Dispose(Boolean disposing)
		{
			if (disposing && components)
			{
				components->Dispose();
			}
			__super::Dispose(disposing);
		}
	private: System::Windows::Forms::TextBox *  textBox1;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Button *  button1;
	private: TerawattManagedControls::FloatEdit *  floatEdit1;
	private: System::Windows::Forms::Label *  label2;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container* components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->textBox1 = new System::Windows::Forms::TextBox();
			this->label1 = new System::Windows::Forms::Label();
			this->button1 = new System::Windows::Forms::Button();
			this->floatEdit1 = new TerawattManagedControls::FloatEdit();
			this->label2 = new System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// textBox1
			// 
			this->textBox1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBox1->Location = System::Drawing::Point(96, 16);
			this->textBox1->Name = S"textBox1";
			this->textBox1->Size = System::Drawing::Size(224, 20);
			this->textBox1->TabIndex = 0;
			this->textBox1->Text = S"";
			this->textBox1->KeyPress += new System::Windows::Forms::KeyPressEventHandler(this, &HostnameDialog::textBox1_KeyPress);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(64, 24);
			this->label1->TabIndex = 3;
			this->label1->Text = S"Host name";
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(112, 80);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(104, 24);
			this->button1->TabIndex = 2;
			this->button1->Text = S"Connect";
			this->button1->Click += new System::EventHandler(this, &HostnameDialog::button1_Click);
			// 
			// floatEdit1
			// 
			this->floatEdit1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->floatEdit1->Location = System::Drawing::Point(96, 48);
			this->floatEdit1->Name = S"floatEdit1";
			this->floatEdit1->Precision = (System::Int16)2;
			this->floatEdit1->Size = System::Drawing::Size(224, 24);
			this->floatEdit1->TabIndex = 1;
			this->floatEdit1->Value = 0.1;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 48);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(72, 16);
			this->label2->TabIndex = 4;
			this->label2->Text = S"Scale Factor";
			// 
			// HostnameDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(336, 118);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->floatEdit1);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox1);
			this->Name = S"HostnameDialog";
			this->Text = S"Real Time Connection";
			this->ResumeLayout(false);

		}		

		//
	private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				 if (this->textBox1->Text->Length > 0)
				 {
					 this->DialogResult = DialogResult::OK;
					 this->Close();
				 }
				 else
				 {
					 MessageBox::Show("Please enter host name to connect.");
				 }
			 }

	private: System::Void textBox1_KeyPress(System::Object *  sender, System::Windows::Forms::KeyPressEventArgs *  e)
			 {
				 if (e->KeyChar == 13) // 13 = ENTER
				 {
					 this->button1_Click(sender, e);
				 }
			 }

};
}