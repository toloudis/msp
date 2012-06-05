#pragma once

#include <string>
#include <vector>

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
	/// Summary for cmraRefNameForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class cmraRefNameForm : public System::Windows::Forms::Form
	{
	public: 
		cmraRefNameForm(const std::vector<std::string> &i_Names)
		{
			InitializeComponent();

			int num_names = i_Names.size();
			for (int i=0; i<num_names; i++)
			{
				this->listBox1->Items->Add(gcnew System::String(i_Names[i].c_str()));
			}
		}

		int GetSelected()
		{
			return this->listBox1->SelectedIndex;
		}
        
	public: 
		~cmraRefNameForm()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::ListBox ^  listBox1;
	private: System::Windows::Forms::Button ^  butAttach;

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
			this->listBox1 = gcnew System::Windows::Forms::ListBox();
			this->butAttach = gcnew System::Windows::Forms::Button();
			this->SuspendLayout();
			// 
			// listBox1
			// 
			this->listBox1->Anchor = (System::Windows::Forms::AnchorStyles)(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right);
			this->listBox1->ItemHeight = 16;
			this->listBox1->Location = System::Drawing::Point(8, 8);
			this->listBox1->Name = "listBox1";
			this->listBox1->Size = System::Drawing::Size(272, 196);
			this->listBox1->TabIndex = 0;
			// 
			// butAttach
			// 
			this->butAttach->Anchor = (System::Windows::Forms::AnchorStyles)(System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left);
			this->butAttach->Location = System::Drawing::Point(96, 224);
			this->butAttach->Name = "butAttach";
			this->butAttach->Size = System::Drawing::Size(104, 32);
			this->butAttach->TabIndex = 1;
			this->butAttach->Text = "Attach";
			this->butAttach->Click += gcnew System::EventHandler(this, &cmraRefNameForm::butAttach_Click);
			// 
			// cmraRefNameForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(6, 15);
			this->ClientSize = System::Drawing::Size(292, 267);
			this->Controls->Add(this->butAttach);
			this->Controls->Add(this->listBox1);
			this->Name = "cmraRefNameForm";
			this->Text = "Attachment Node";
			this->ResumeLayout(false);

		}		
		//

	private: System::Void butAttach_Click(System::Object ^  sender, System::EventArgs ^  e)
			 {
				 // Make sure an item is selected before closing dialog
				 int sel_index = this->listBox1->SelectedIndex;
				 if (sel_index >= 0)
				 {
					 this->DialogResult = ::DialogResult::OK;
					 this->Close();
				 }
			 }

	};
}
#endif // _MANAGED
