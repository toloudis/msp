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
	/// Summary for SwitchModelDialog
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public __gc class SwitchModelDialog : public System::Windows::Forms::Form
	{
	public: 
		SwitchModelDialog(void)
		{
			InitializeComponent();
		}

		System::String* GetModelFilename()
		{
			return fileChooser_Model->Fullpath;
		}
		System::String* GetAnimationFilename()
		{
			return fileChooser_Animation->Fullpath;
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
	private: TerawattManagedControls::FileChooser *  fileChooser_Animation;
	private: System::Windows::Forms::Label *  label2;
	private: TerawattManagedControls::FileChooser *  fileChooser_Model;
	private: System::Windows::Forms::Label *  label1;
	private: System::Windows::Forms::Button *  buttonOK;
	private: System::Windows::Forms::Button *  buttonCancel;

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
			this->fileChooser_Animation = new TerawattManagedControls::FileChooser();
			this->label2 = new System::Windows::Forms::Label();
			this->fileChooser_Model = new TerawattManagedControls::FileChooser();
			this->label1 = new System::Windows::Forms::Label();
			this->buttonOK = new System::Windows::Forms::Button();
			this->buttonCancel = new System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// fileChooser_Animation
			// 
			this->fileChooser_Animation->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser_Animation->Filter = S"Animation files (*.*a)|*.*a|Joint Animation (*.jna)|*.jna|Character Animation (*." 
				S"cha)|*.cha|All files (*.*)|*.*";
			this->fileChooser_Animation->Location = System::Drawing::Point(80, 56);
			this->fileChooser_Animation->Name = S"fileChooser_Animation";
			this->fileChooser_Animation->Size = System::Drawing::Size(272, 27);
			this->fileChooser_Animation->TabIndex = 4;
			// 
			// label2
			// 
			this->label2->Location = System::Drawing::Point(8, 56);
			this->label2->Name = S"label2";
			this->label2->Size = System::Drawing::Size(66, 27);
			this->label2->TabIndex = 3;
			this->label2->Text = S"Idle Anim:";
			// 
			// fileChooser_Model
			// 
			this->fileChooser_Model->Anchor = (System::Windows::Forms::AnchorStyles)((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->fileChooser_Model->Filter = S"Model files (*.*x)|*.*x|Joint Models (*.jnx)|*.jnx|Character Models (*.chx)|*.chx" 
				S"|All files (*.*)|*.*";
			this->fileChooser_Model->Location = System::Drawing::Point(80, 16);
			this->fileChooser_Model->Name = S"fileChooser_Model";
			this->fileChooser_Model->Size = System::Drawing::Size(272, 27);
			this->fileChooser_Model->TabIndex = 6;
			// 
			// label1
			// 
			this->label1->Location = System::Drawing::Point(8, 16);
			this->label1->Name = S"label1";
			this->label1->Size = System::Drawing::Size(66, 27);
			this->label1->TabIndex = 5;
			this->label1->Text = S"Model:";
			// 
			// buttonOK
			// 
			this->buttonOK->Location = System::Drawing::Point(80, 104);
			this->buttonOK->Name = S"buttonOK";
			this->buttonOK->Size = System::Drawing::Size(72, 24);
			this->buttonOK->TabIndex = 7;
			this->buttonOK->Text = S"OK";
			this->buttonOK->Click += new System::EventHandler(this, &SwitchModelDialog::buttonOK_Click);
			// 
			// buttonCancel
			// 
			this->buttonCancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
			this->buttonCancel->Location = System::Drawing::Point(184, 104);
			this->buttonCancel->Name = S"buttonCancel";
			this->buttonCancel->Size = System::Drawing::Size(72, 24);
			this->buttonCancel->TabIndex = 8;
			this->buttonCancel->Text = S"Cancel";
			// 
			// SwitchModelDialog
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(368, 142);
			this->Controls->Add(this->buttonCancel);
			this->Controls->Add(this->buttonOK);
			this->Controls->Add(this->fileChooser_Model);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->fileChooser_Animation);
			this->Controls->Add(this->label2);
			this->Name = S"SwitchModelDialog";
			this->Text = S"Switch Model";
			this->ResumeLayout(false);

		}		




		//

private: System::Void buttonOK_Click(System::Object *  sender, System::EventArgs *  e)
		 {
				 // Only Close if filename is assigned, idle animation
				 // is optional.
				 bool filename_assigned = (fileChooser_Model->Fullpath->Length > 0);
				 if (filename_assigned)
				 {
						this->DialogResult = DialogResult::OK;
						this->Close();
				 }
				 else 
				 {
					 MessageBox::Show(S"Fill in the Model filename field.");
				 }
		 }


};
}