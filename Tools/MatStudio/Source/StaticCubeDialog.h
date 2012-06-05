#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace MatStudio
{
	/// <summary> 
	/// Summary for StaticCubeDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class StaticCubeDialog : public System::Windows::Forms::Form
	{
	public: 
		StaticCubeDialog(void)
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

	private: TerawattManagedControls::FileChooser *  fileChooser1;
	private: System::Windows::Forms::Button *  butSave;
	private: TerawattManagedControls::FileChooser *  fileChooser2;
	private: TerawattManagedControls::FileChooser *  fileChooser3;
	private: TerawattManagedControls::FileChooser *  fileChooser4;
	private: TerawattManagedControls::FileChooser *  fileChooser5;
	private: TerawattManagedControls::FileChooser *  fileChooser6;
	private: System::Windows::Forms::GroupBox *  groupBox1;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Label *  label2;
	private: System::Windows::Forms::Label *  label3;
	private: System::Windows::Forms::Label *  label4;
	private: System::Windows::Forms::Label *  label5;
	private: System::Windows::Forms::Label *  label6;
	private: System::Windows::Forms::Button *  butLoad;


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
			this->fileChooser1 = new TerawattManagedControls::FileChooser();
			this->butSave = new System::Windows::Forms::Button();
			this->fileChooser2 = new TerawattManagedControls::FileChooser();
			this->fileChooser3 = new TerawattManagedControls::FileChooser();
			this->fileChooser4 = new TerawattManagedControls::FileChooser();
			this->fileChooser5 = new TerawattManagedControls::FileChooser();
			this->fileChooser6 = new TerawattManagedControls::FileChooser();
			this->groupBox1 = new System::Windows::Forms::GroupBox();
			this->label6 = new System::Windows::Forms::Label();
			this->label5 = new System::Windows::Forms::Label();
			this->label4 = new System::Windows::Forms::Label();
			this->label3 = new System::Windows::Forms::Label();
			this->label2 = new System::Windows::Forms::Label();
			this->label1 = new System::Windows::Forms::Label();
			this->butLoad = new System::Windows::Forms::Button();
			this->groupBox1->SuspendLayout();
			this->SuspendLayout();
			// 
			// fileChooser1
			// 
			this->fileChooser1->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser1->Location = System::Drawing::Point(112, 32);
			this->fileChooser1->Name = S"fileChooser1";
			this->fileChooser1->Size = System::Drawing::Size(360, 32);
			this->fileChooser1->TabIndex = 0;
			// 
			// butSave
			// 
			this->butSave->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->butSave->Location = System::Drawing::Point(272, 272);
			this->butSave->Name = S"butSave";
			this->butSave->Size = System::Drawing::Size(88, 32);
			this->butSave->TabIndex = 1;
			this->butSave->Text = S"Save";
			this->butSave->Click += new System::EventHandler(this, butSave_Click);
			// 
			// fileChooser2
			// 
			this->fileChooser2->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser2->Location = System::Drawing::Point(112, 64);
			this->fileChooser2->Name = S"fileChooser2";
			this->fileChooser2->Size = System::Drawing::Size(360, 32);
			this->fileChooser2->TabIndex = 2;
			// 
			// fileChooser3
			// 
			this->fileChooser3->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser3->Location = System::Drawing::Point(112, 96);
			this->fileChooser3->Name = S"fileChooser3";
			this->fileChooser3->Size = System::Drawing::Size(360, 32);
			this->fileChooser3->TabIndex = 3;
			// 
			// fileChooser4
			// 
			this->fileChooser4->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser4->Location = System::Drawing::Point(112, 128);
			this->fileChooser4->Name = S"fileChooser4";
			this->fileChooser4->Size = System::Drawing::Size(360, 32);
			this->fileChooser4->TabIndex = 4;
			// 
			// fileChooser5
			// 
			this->fileChooser5->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser5->Location = System::Drawing::Point(112, 160);
			this->fileChooser5->Name = S"fileChooser5";
			this->fileChooser5->Size = System::Drawing::Size(360, 32);
			this->fileChooser5->TabIndex = 5;
			// 
			// fileChooser6
			// 
			this->fileChooser6->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser6->Location = System::Drawing::Point(112, 192);
			this->fileChooser6->Name = S"fileChooser6";
			this->fileChooser6->Size = System::Drawing::Size(360, 32);
			this->fileChooser6->TabIndex = 6;
			// 
			// groupBox1
			// 
			this->groupBox1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Controls->Add(this->fileChooser5);
			this->groupBox1->Controls->Add(this->fileChooser2);
			this->groupBox1->Controls->Add(this->fileChooser6);
			this->groupBox1->Controls->Add(this->fileChooser3);
			this->groupBox1->Controls->Add(this->fileChooser1);
			this->groupBox1->Controls->Add(this->fileChooser4);
			this->groupBox1->Location = System::Drawing::Point(8, 8);
			this->groupBox1->Name = S"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(488, 248);
			this->groupBox1->TabIndex = 7;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = S"6 Textures Required";
			// 
			// label6
			// 
			this->label6->Location = System::Drawing::Point(16, 192);
			this->label6->Name = S"label6";
			this->label6->Size = System::Drawing::Size(80, 24);
			this->label6->TabIndex = 12;
			this->label6->Text = S"Front (-Z)";
			// 
			// label5
			// 
			this->label5->Location = System::Drawing::Point(16, 160);
			this->label5->Name = S"label5";
			this->label5->Size = System::Drawing::Size(80, 24);
			this->label5->TabIndex = 11;
			this->label5->Text = S"Back (+Z)";
			// 
			// label4
			// 
			this->label4->Location = System::Drawing::Point(16, 128);
			this->label4->Name = S"label4";
			this->label4->Size = System::Drawing::Size(80, 24);
			this->label4->TabIndex = 10;
			this->label4->Text = S"Down (-Y)";
			// 
			// label3
			// 
			this->label3->Location = System::Drawing::Point(16, 96);
			this->label3->Name = S"label3";
			this->label3->Size = System::Drawing::Size(80, 24);
			this->label3->TabIndex = 9;
			this->label3->Text = S"Up (+Y)";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(16, 64);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(80, 24);
			this->label2->TabIndex = 8;
			this->label2->Text = S"Right (-X)";
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(16, 32);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(80, 24);
			this->label1->TabIndex = 7;
			this->label1->Text = S"Left (+X)";
			// 
			// butLoad
			// 
			this->butLoad->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->butLoad->Location = System::Drawing::Point(152, 272);
			this->butLoad->Name = S"butLoad";
			this->butLoad->Size = System::Drawing::Size(88, 32);
			this->butLoad->TabIndex = 8;
			this->butLoad->Text = S"Load";
			this->butLoad->Click += new System::EventHandler(this, butLoad_Click);
			// 
			// StaticCubeDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(6, 15);
			this->ClientSize = System::Drawing::Size(512, 319);
			this->Controls->Add(this->butLoad);
			this->Controls->Add(this->groupBox1);
			this->Controls->Add(this->butSave);
			this->Name = S"StaticCubeDialog";
			this->Text = S"StaticCubeDialog";
			this->groupBox1->ResumeLayout(false);
			this->ResumeLayout(false);

		}		
		
		//
	private: System::Void butLoad_Click(System::Object *  sender, System::EventArgs *  e);
	private: System::Void butSave_Click(System::Object *  sender, System::EventArgs *  e);

};
}