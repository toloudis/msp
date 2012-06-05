#pragma once


namespace PropertyPages
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

			button_prev->set_Enabled( false );
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

	private: System::Windows::Forms::TabPage *  tabPage1;
	private: System::Windows::Forms::TabPage *  tabPage2;
	private: System::Windows::Forms::TabPage *  tabPage3;
	private: System::Windows::Forms::TabPage *  tabPage4;


	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Button *  button_next;
	private: System::Windows::Forms::Button *  button_prev;
	private: System::Windows::Forms::TabControl *  tabControl_main;

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
			this->tabControl_main = new System::Windows::Forms::TabControl();
			this->tabPage1 = new System::Windows::Forms::TabPage();
			this->label1 = new System::Windows::Forms::Label();
			this->tabPage2 = new System::Windows::Forms::TabPage();
			this->tabPage3 = new System::Windows::Forms::TabPage();
			this->tabPage4 = new System::Windows::Forms::TabPage();
			this->button_next = new System::Windows::Forms::Button();
			this->button_prev = new System::Windows::Forms::Button();
			this->tabControl_main->SuspendLayout();
			this->tabPage1->SuspendLayout();
			this->SuspendLayout();
			// 
			// tabControl_main
			// 
			this->tabControl_main->Appearance = System::Windows::Forms::TabAppearance::FlatButtons;
			this->tabControl_main->Controls->Add(this->tabPage1);
			this->tabControl_main->Controls->Add(this->tabPage2);
			this->tabControl_main->Controls->Add(this->tabPage3);
			this->tabControl_main->Controls->Add(this->tabPage4);
			this->tabControl_main->Location = System::Drawing::Point(8, 8);
			this->tabControl_main->Name = S"tabControl_main";
			this->tabControl_main->SelectedIndex = 0;
			this->tabControl_main->Size = System::Drawing::Size(520, 464);
			this->tabControl_main->TabIndex = 0;
			// 
			// tabPage1
			// 
			this->tabPage1->Controls->Add(this->label1);
			this->tabPage1->Location = System::Drawing::Point(4, 25);
			this->tabPage1->Name = S"tabPage1";
			this->tabPage1->Size = System::Drawing::Size(512, 435);
			this->tabPage1->TabIndex = 0;
			this->tabPage1->Text = S"tabPage1";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(32, 40);
			this->label1->Name = S"label1";
			this->label1->TabIndex = 0;
			this->label1->Text = S"label1";
			// 
			// tabPage2
			// 
			this->tabPage2->Location = System::Drawing::Point(4, 25);
			this->tabPage2->Name = S"tabPage2";
			this->tabPage2->Size = System::Drawing::Size(512, 435);
			this->tabPage2->TabIndex = 1;
			this->tabPage2->Text = S"tabPage2";
			// 
			// tabPage3
			// 
			this->tabPage3->Location = System::Drawing::Point(4, 25);
			this->tabPage3->Name = S"tabPage3";
			this->tabPage3->Size = System::Drawing::Size(512, 435);
			this->tabPage3->TabIndex = 2;
			this->tabPage3->Text = S"tabPage3";
			// 
			// tabPage4
			// 
			this->tabPage4->Location = System::Drawing::Point(4, 25);
			this->tabPage4->Name = S"tabPage4";
			this->tabPage4->Size = System::Drawing::Size(512, 435);
			this->tabPage4->TabIndex = 3;
			this->tabPage4->Text = S"tabPage4";
			// 
			// button_next
			// 
			this->button_next->Location = System::Drawing::Point(296, 480);
			this->button_next->Name = S"button_next";
			this->button_next->TabIndex = 1;
			this->button_next->Text = S"next";
			this->button_next->Click += new System::EventHandler(this, button_next_Click);
			// 
			// button_prev
			// 
			this->button_prev->Location = System::Drawing::Point(168, 480);
			this->button_prev->Name = S"button_prev";
			this->button_prev->TabIndex = 2;
			this->button_prev->Text = S"prev";
			this->button_prev->Click += new System::EventHandler(this, button_prev_Click);
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(536, 509);
			this->Controls->Add(this->button_prev);
			this->Controls->Add(this->button_next);
			this->Controls->Add(this->tabControl_main);
			this->Name = S"Form1";
			this->Text = S"Form1";
			this->tabControl_main->ResumeLayout(false);
			this->tabPage1->ResumeLayout(false);
			this->ResumeLayout(false);

		}	

private: System::Void button_next_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			int index = tabControl_main->get_SelectedIndex();
			index++;

			if ( index >= (tabControl_main->get_TabCount()-1) )
			{
				//index = 0;
				index = tabControl_main->get_TabCount() - 1;

				button_next->set_Enabled( false );
			}
			else
			{
				button_next->set_Enabled( true );
			}
			button_prev->set_Enabled( true );
			tabControl_main->set_SelectedIndex( index );
		 }

private: System::Void button_prev_Click(System::Object *  sender, System::EventArgs *  e)
		 {
			int index = tabControl_main->get_SelectedIndex();
			index--;

			if ( index <= 0 )
			{
				//index = tabControl_main->get_TabCount() - 1;
				index = 0;

				button_prev->set_Enabled( false );
			}
			else
			{
				button_prev->set_Enabled( true );
			}
			button_next->set_Enabled( true );

			tabControl_main->set_SelectedIndex( index );
		 }
};
}


