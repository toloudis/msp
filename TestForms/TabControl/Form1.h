#pragma once


namespace TabControl1
{
	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary> 
	/// Summary for Form1
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class Form1 : public System::Windows::Forms::Form
	{	
	public:
		Form1(void)
		{
			InitializeComponent();
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
	private: System::Windows::Forms::TabControl *  tabControl1;
	private: System::Windows::Forms::TabPage *  tabPage1;
	private: System::Windows::Forms::TabPage *  tabPage2;
	private: System::Windows::Forms::TabPage *  tabPage3;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::TextBox *  textBox_index;


	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container * components;

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->tabControl1 = new System::Windows::Forms::TabControl();
			this->tabPage1 = new System::Windows::Forms::TabPage();
			this->tabPage2 = new System::Windows::Forms::TabPage();
			this->tabPage3 = new System::Windows::Forms::TabPage();
			this->label1 = new System::Windows::Forms::Label();
			this->textBox_index = new System::Windows::Forms::TextBox();
			this->tabControl1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl1
			// 
			this->tabControl1->Controls->Add(this->tabPage1);
			this->tabControl1->Controls->Add(this->tabPage2);
			this->tabControl1->Controls->Add(this->tabPage3);
			this->tabControl1->Location = System::Drawing::Point(8, 16);
			this->tabControl1->Name = S"tabControl1";
			this->tabControl1->SelectedIndex = 0;
			this->tabControl1->Size = System::Drawing::Size(392, 248);
			this->tabControl1->TabIndex = 0;
			this->tabControl1->SelectedIndexChanged += new System::EventHandler(this, tabControl1_SelectedIndexChanged);
			// 
			// tabPage1
			// 
			this->tabPage1->Location = System::Drawing::Point(4, 22);
			this->tabPage1->Name = S"tabPage1";
			this->tabPage1->Size = System::Drawing::Size(384, 222);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = S"tabPage1";
			// 
			// tabPage2
			// 
			this->tabPage2->Location = System::Drawing::Point(4, 22);
			this->tabPage2->Name = S"tabPage2";
			this->tabPage2->Size = System::Drawing::Size(384, 222);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = S"tabPage2";
			// 
			// tabPage3
			// 
			this->tabPage3->Location = System::Drawing::Point(4, 22);
			this->tabPage3->Name = S"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(384, 222);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = S"tabPage3";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(248, 296);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(72, 23);
			this->label1->TabIndex = 1;
			this->label1->Text = S"selected tab";
			// 
			// textBox_index
			// 
			this->textBox_index->Location = System::Drawing::Point(336, 296);
			this->textBox_index->Name = S"textBox_index";
			this->textBox_index->Size = System::Drawing::Size(72, 20);
			this->textBox_index->TabIndex = 2;
			this->textBox_index->Text = S"";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(416, 333);
			this->Controls->Add(this->textBox_index);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->tabControl1);
			this->Name = S"Form1";
			this->Text = S"Form1";
			this->tabControl1->ResumeLayout(false);
			this->ResumeLayout(false);

		}	
	private: System::Void tabControl1_SelectedIndexChanged(System::Object *  sender, System::EventArgs *  e)
			 {
				 int index = tabControl1->get_SelectedIndex();
				 textBox_index->SetText("0");
			 }

};
}


