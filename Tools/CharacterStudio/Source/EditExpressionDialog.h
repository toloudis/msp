#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace CharacterStudio
{
	/// <summary> 
	/// Summary for EditExpressionDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class EditExpressionDialog : public System::Windows::Forms::Form
	{
	public: 
		EditExpressionDialog(void)
		{
			InitializeComponent();
		}

		EditExpressionDialog(System::String^ i_Name)
		{
			InitializeComponent();
			textBoxName->Text = i_Name;
			fileChooserAnimation->Enabled = false;
		}
		

		System::String^ GetName()
		{
			return textBoxName->Text;
		}
		System::String^ GetFilename()
		{
			return fileChooserAnimation->Fullpath;
		}
        
	protected: 
		~EditExpressionDialog()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label ^  label1;
	private: System::Windows::Forms::Label ^  label2;
	private: TerawattManagedControls::FileChooser ^  fileChooserAnimation;
	private: System::Windows::Forms::TextBox ^  textBoxName;
	private: System::Windows::Forms::Button ^  buttonOK;
	private: System::Windows::Forms::Button ^  buttonCancel;

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
			this->label1 = gcnew System::Windows::Forms::Label();
			this->label2 = gcnew System::Windows::Forms::Label();
			this->fileChooserAnimation = gcnew TerawattManagedControls::FileChooser();
			this->textBoxName = gcnew System::Windows::Forms::TextBox();
			this->buttonOK = gcnew System::Windows::Forms::Button();
			this->buttonCancel = gcnew System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(7, 14);
			this->label1->Name = "label1";
			this->label1->Size = System::Drawing::Size(66, 21);
			this->label1->TabIndex = 0;
			this->label1->Text = "Name:";
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(7, 49);
			this->label2->Name = "label2";
			this->label2->Size = System::Drawing::Size(66, 27);
			this->label2->TabIndex = 1;
			this->label2->Text = "Animation:";
			// 
			// fileChooserAnimation
			// 
			this->fileChooserAnimation->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooserAnimation->Filter = "Animation files (*.*a)|*.*a|Joint Animation (*.jna)|*.jna|Character Animation (*." 
				"cha)|*.cha|All files (*.*)|*.*";
			this->fileChooserAnimation->Location = System::Drawing::Point(80, 49);
			this->fileChooserAnimation->Name = "fileChooserAnimation";
			this->fileChooserAnimation->Size = System::Drawing::Size(247, 27);
			this->fileChooserAnimation->TabIndex = 2;
			this->fileChooserAnimation->ValueChanged += gcnew System::EventHandler(this, &CharacterStudio::EditExpressionDialog::fileChooserAnimation_ValueChanged);
			// 
			// textBoxName
			// 
			this->textBoxName->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->textBoxName->Location = System::Drawing::Point(80, 14);
			this->textBoxName->Name = "textBoxName";
			this->textBoxName->Size = System::Drawing::Size(180, 20);
			this->textBoxName->TabIndex = 3;
			this->textBoxName->Text = "";
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(80, 83);
			this->buttonOK->Name = "buttonOK";
			this->buttonOK->Size = System::Drawing::Size(62, 20);
			this->buttonOK->TabIndex = 4;
			this->buttonOK->Text = "OK";
			this->buttonOK->Click += gcnew System::EventHandler(this, &CharacterStudio::EditExpressionDialog::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(173, 83);
			this->buttonCancel->Name = "buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(67, 20);
			this->buttonCancel->TabIndex = 5;
			this->buttonCancel->Text = "Cancel";
			// 
			// EditExpressionDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(339, 117);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->textBoxName);
			this->Controls->Add(this->fileChooserAnimation);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Name = "EditExpressionDialog";
			this->Text = "Expression";
			this->ResumeLayout(false);

		}		
		///

	private: System::Void buttonOK_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 // Only Close if filename and name are assigned
				 bool filename_assigned = ((!fileChooserAnimation->Enabled)
					 || (fileChooserAnimation->Fullpath->Length > 0));
				 if (textBoxName->Text->Length > 0 &&
					 filename_assigned)
				 {
					 this->DialogResult = ::DialogResult::OK;
						this->Close();
				 }
				 else 
				 {
					 MessageBox::Show("Fill in Name and Filename fields.");
				 }
			 }

private: System::Void fileChooserAnimation_ValueChanged(System::Object ^  sender, System::EventArgs ^  e)
		 {
			 if (textBoxName->Text->Length == 0)
			 {
				 String^ fname = fileChooserAnimation->Filename;
				 if (fname->EndsWith(".cha"))
				 {
					String^ name = fname->Substring(0, fname->Length - 4);
					textBoxName->Text = name;
				 }
			 }
		 }

};
}