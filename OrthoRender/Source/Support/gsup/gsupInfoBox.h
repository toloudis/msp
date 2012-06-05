#pragma once

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace Support
{
	/// <summary> 
	/// gsupInfoBox is used to display info while a task is working 
	/// </summary>
	public ref class gsupInfoBox : public System::Windows::Forms::Form
	{
	public: 
		gsupInfoBox(System::String^ i_Message)
		{
			InitializeComponent();
			labelText->Text = i_Message;
		}
        
	public: 
		~gsupInfoBox()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Label ^  labelText;

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
			this->labelText = gcnew System::Windows::Forms::Label();
			this->SuspendLayout();
			// 
			// labelText
			// 
			this->labelText->Dock = System::Windows::Forms::DockStyle::Fill;
			this->labelText->Location = System::Drawing::Point(0, 0);
			this->labelText->Name = "labelText";
			this->labelText->Size = System::Drawing::Size(320, 78);
			this->labelText->TabIndex = 0;
			this->labelText->TextAlign = System::Drawing::ContentAlignment::MiddleCenter;
			// 
			// gsupInfoBox
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(320, 78);
			this->ControlBox = false;
			this->Controls->Add(this->labelText);
			this->Name = "gsupInfoBox";
			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
			this->Text = "Info";
			this->ResumeLayout(false);

		}		
	};
}
#endif // _MANAGED
