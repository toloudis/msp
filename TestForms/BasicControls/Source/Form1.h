#pragma once


namespace Source
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
			SetupComponents();
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
	private: System::Windows::Forms::ComboBox *  comboBox1;

	private: System::Windows::Forms::Button *  button1;
	private: System::Windows::Forms::Button *  button2;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Button *  button3;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Button *  button4;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::GroupBox *  groupBox2;
	private: System::Windows::Forms::GroupBox *  groupBox_textboxes;
	private: System::Windows::Forms::TextBox *  textBox_basic;
	private: System::Windows::Forms::GroupBox *  groupBox_radiobuttons;
	private: System::Windows::Forms::RadioButton *  radioButton_one;
	private: System::Windows::Forms::RadioButton *  radioButton_two;
	private: System::Windows::Forms::RadioButton *  radioButton_three;
	private: System::Windows::Forms::RadioButton *  radioButton_four;

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
			this->comboBox1 = new System::Windows::Forms::ComboBox();
			this->button1 = new System::Windows::Forms::Button();
			this->button2 = new System::Windows::Forms::Button();
			this->label1 = new System::Windows::Forms::Label();
			this->button3 = new System::Windows::Forms::Button();
			this->label3 = new System::Windows::Forms::Label();
			this->button4 = new System::Windows::Forms::Button();
			this->label2 = new System::Windows::Forms::Label();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->groupBox2 = new System::Windows::Forms::GroupBox();
			this->groupBox_textboxes = new System::Windows::Forms::GroupBox();
			this->textBox_basic = new System::Windows::Forms::TextBox();
			this->groupBox_radiobuttons = new System::Windows::Forms::GroupBox();
			this->radioButton_one = new System::Windows::Forms::RadioButton();
			this->radioButton_two = new System::Windows::Forms::RadioButton();
			this->radioButton_three = new System::Windows::Forms::RadioButton();
			this->radioButton_four = new System::Windows::Forms::RadioButton();
			this->groupBox_textboxes->SuspendLayout();
			this->groupBox_radiobuttons->SuspendLayout();
			this->SuspendLayout();
			// 
			// comboBox1
			// 
			System::Object* __mcTemp__1[] = new System::Object*[6];
			__mcTemp__1[0] = S"Arsenic";
			__mcTemp__1[1] = S"Brass";
			__mcTemp__1[2] = S"Choke";
			__mcTemp__1[3] = S"Dream";
			__mcTemp__1[4] = S"Eat";
			__mcTemp__1[5] = S"Fly";
			this->comboBox1->Items->AddRange(__mcTemp__1);
			this->comboBox1->Location = System::Drawing::Point(16, 168);
			this->comboBox1->Name = S"comboBox1";
			this->comboBox1->Size = System::Drawing::Size(208, 21);
			this->comboBox1->TabIndex = 0;
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(240, 168);
			this->button1->Name = S"button1";
			this->button1->Size = System::Drawing::Size(32, 24);
			this->button1->TabIndex = 2;
			this->button1->Text = S"\'A\'";
			this->button1->Click += new System::EventHandler(this, button1_Click);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(288, 168);
			this->button2->Name = S"button2";
			this->button2->Size = System::Drawing::Size(32, 24);
			this->button2->TabIndex = 3;
			this->button2->Text = S"\'B\'";
			this->button2->Click += new System::EventHandler(this, button2_Click);
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 136);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(304, 23);
			this->label1->TabIndex = 4;
			this->label1->Text = S"Click A or B button to change the combo box selected value";
			// 
			// button3
			// 
			this->button3->ImageAlign = System::Drawing::ContentAlignment::TopLeft;
			this->button3->Location = System::Drawing::Point(16, 24);
			this->button3->Name = S"button3";
			this->button3->Size = System::Drawing::Size(32, 32);
			this->button3->TabIndex = 5;
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(64, 32);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(264, 23);
			this->label3->TabIndex = 6;
			this->label3->Text = S"32x32 button with 48x48 image";
			// 
			// button4
			// 
			this->button4->ImageAlign = System::Drawing::ContentAlignment::TopLeft;
			this->button4->Location = System::Drawing::Point(16, 64);
			this->button4->Name = S"button4";
			this->button4->Size = System::Drawing::Size(32, 32);
			this->button4->TabIndex = 7;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(64, 72);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(264, 23);
			this->label2->TabIndex = 8;
			this->label2->Text = S"32x32 button with 48x48 image using ImageList";
			// 
			// groupBox1
			// 
			this->groupBox1->Location = System::Drawing::Point(8, 0);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(336, 104);
			this->groupBox1->TabIndex = 9;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"Buttons with Images";
			// 
			// groupBox2
			// 
			this->groupBox2->Location = System::Drawing::Point(8, 120);
			this->groupBox2->Name = S"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(336, 80);
			this->groupBox2->TabIndex = 10;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = S"Combobox Selection";
			// 
			// groupBox_textboxes
			// 
			this->groupBox_textboxes->Controls->Add(this->textBox_basic);
			this->groupBox_textboxes->Location = System::Drawing::Point(8, 208);
			this->groupBox_textboxes->Name = S"groupBox_textboxes";
			this->groupBox_textboxes->Size = System::Drawing::Size(336, 64);
			this->groupBox_textboxes->TabIndex = 11;
			this->groupBox_textboxes->TabStop = false;
			this->groupBox_textboxes->Text = S"TextBoxes";
			// 
			// textBox_basic
			// 
			this->textBox_basic->Location = System::Drawing::Point(8, 24);
			this->textBox_basic->Name = S"textBox_basic";
			this->textBox_basic->Size = System::Drawing::Size(312, 20);
			this->textBox_basic->TabIndex = 0;
			this->textBox_basic->Text = S"<text goes here>";
			// 
			// groupBox_radiobuttons
			// 
			this->groupBox_radiobuttons->Controls->Add(this->radioButton_four);
			this->groupBox_radiobuttons->Controls->Add(this->radioButton_three);
			this->groupBox_radiobuttons->Controls->Add(this->radioButton_two);
			this->groupBox_radiobuttons->Controls->Add(this->radioButton_one);
			this->groupBox_radiobuttons->Location = System::Drawing::Point(8, 280);
			this->groupBox_radiobuttons->Name = S"groupBox_radiobuttons";
			this->groupBox_radiobuttons->Size = System::Drawing::Size(336, 72);
			this->groupBox_radiobuttons->TabIndex = 12;
			this->groupBox_radiobuttons->TabStop = false;
			this->groupBox_radiobuttons->Text = S"RadioButtons";
			// 
			// radioButton_one
			// 
			this->radioButton_one->Location = System::Drawing::Point(16, 16);
			this->radioButton_one->Name = S"radioButton_one";
			this->radioButton_one->TabIndex = 0;
			this->radioButton_one->Text = S"One";
			// 
			// radioButton_two
			// 
			this->radioButton_two->Location = System::Drawing::Point(16, 40);
			this->radioButton_two->Name = S"radioButton_two";
			this->radioButton_two->TabIndex = 1;
			this->radioButton_two->Text = S"Two";
			// 
			// radioButton_three
			// 
			this->radioButton_three->Location = System::Drawing::Point(176, 16);
			this->radioButton_three->Name = S"radioButton_three";
			this->radioButton_three->TabIndex = 2;
			this->radioButton_three->Text = S"Three";
			// 
			// radioButton_four
			// 
			this->radioButton_four->Location = System::Drawing::Point(176, 40);
			this->radioButton_four->Name = S"radioButton_four";
			this->radioButton_four->TabIndex = 3;
			this->radioButton_four->Text = S"Four";
			// 
			// Form1
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(352, 366);
			this->Controls->Add(this->groupBox_radiobuttons);
			this->Controls->Add(this->groupBox_textboxes);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->button1);
			this->Controls->Add(this->comboBox1);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->groupBox2);
			this->Name = S"Form1";
			this->Text = S"Basic Controls";
			this->groupBox_textboxes->ResumeLayout(false);
			this->groupBox_radiobuttons->ResumeLayout(false);
			this->ResumeLayout(false);

		}	

	private: System::Void button1_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				int index = comboBox1->FindString(S"Dream");
				comboBox1->SelectedIndex = index;
			 }
	private: System::Void button2_Click(System::Object *  sender, System::EventArgs *  e)
			 {
				int index = comboBox1->FindString(S"Fly");
				comboBox1->SelectedIndex = index;
			 }
	private: System::Void SetupComponents()
		 {
				//	attach the image directly
				button3->Image = System::Drawing::Image::FromFile( "..//test.bmp" );

				//	attach the image to an image list first
				int w,h;
				Drawing::Size bsize = button4->Size;
				w = (int)(bsize.Width -10);
				h = (int)(bsize.Height -10);

				button4->ImageList = new ImageList();
				button4->ImageList->ImageSize = *(__nogc new System::Drawing::Size(w,h));

				button4->ImageList->Images->Add( System::Drawing::Image::FromFile( "..//test.bmp" ) );
				button4->ImageIndex = button4->ImageList->Images->Count - 1;
		 }
};
}


