#pragma once


namespace GlobalHotKeys {

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
	public ref class Form1 : public System::Windows::Forms::Form
	{
	public:
		Form1(void);

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~Form1();
	private: System::Windows::Forms::Label^  label_pressed;
	private: System::Windows::Forms::Button^  button_launchdialog;
	private: System::Windows::Forms::Label^  label_launchdialog;
	private: System::Windows::Forms::Label^  label_previewkeydown;
	private: System::Windows::Forms::Label^  label_processcmdkey;
	private: System::Windows::Forms::Label^  label_keysconverter;
	protected: 

	protected: 

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
			this->label_pressed = (gcnew System::Windows::Forms::Label());
			this->button_launchdialog = (gcnew System::Windows::Forms::Button());
			this->label_launchdialog = (gcnew System::Windows::Forms::Label());
			this->label_previewkeydown = (gcnew System::Windows::Forms::Label());
			this->label_processcmdkey = (gcnew System::Windows::Forms::Label());
			this->label_keysconverter = (gcnew System::Windows::Forms::Label());
			this->SuspendLayout();
			// 
			// label_pressed
			// 
			this->label_pressed->AutoSize = true;
			this->label_pressed->Location = System::Drawing::Point(12, 9);
			this->label_pressed->Name = L"label_pressed";
			this->label_pressed->Size = System::Drawing::Size(31, 13);
			this->label_pressed->TabIndex = 0;
			this->label_pressed->Text = L"none";
			// 
			// button_launchdialog
			// 
			this->button_launchdialog->Location = System::Drawing::Point(12, 195);
			this->button_launchdialog->Name = L"button_launchdialog";
			this->button_launchdialog->Size = System::Drawing::Size(75, 23);
			this->button_launchdialog->TabIndex = 1;
			this->button_launchdialog->Text = L"launch!";
			this->button_launchdialog->UseVisualStyleBackColor = true;
			this->button_launchdialog->Click += gcnew System::EventHandler(this, &Form1::button_launchdialog_Click);
			// 
			// label_launchdialog
			// 
			this->label_launchdialog->AutoSize = true;
			this->label_launchdialog->Location = System::Drawing::Point(9, 168);
			this->label_launchdialog->Name = L"label_launchdialog";
			this->label_launchdialog->Size = System::Drawing::Size(42, 13);
			this->label_launchdialog->TabIndex = 2;
			this->label_launchdialog->Text = L"launch!";
			// 
			// label_previewkeydown
			// 
			this->label_previewkeydown->AutoSize = true;
			this->label_previewkeydown->Location = System::Drawing::Point(12, 37);
			this->label_previewkeydown->Name = L"label_previewkeydown";
			this->label_previewkeydown->Size = System::Drawing::Size(31, 13);
			this->label_previewkeydown->TabIndex = 3;
			this->label_previewkeydown->Text = L"none";
			// 
			// label_processcmdkey
			// 
			this->label_processcmdkey->AutoSize = true;
			this->label_processcmdkey->Location = System::Drawing::Point(12, 74);
			this->label_processcmdkey->Name = L"label_processcmdkey";
			this->label_processcmdkey->Size = System::Drawing::Size(31, 13);
			this->label_processcmdkey->TabIndex = 4;
			this->label_processcmdkey->Text = L"none";
			// 
			// label_keysconverter
			// 
			this->label_keysconverter->AutoSize = true;
			this->label_keysconverter->Location = System::Drawing::Point(9, 102);
			this->label_keysconverter->Name = L"label_keysconverter";
			this->label_keysconverter->Size = System::Drawing::Size(31, 13);
			this->label_keysconverter->TabIndex = 5;
			this->label_keysconverter->Text = L"none";
			// 
			// Form1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(292, 266);
			this->Controls->Add(this->label_keysconverter);
			this->Controls->Add(this->label_processcmdkey);
			this->Controls->Add(this->label_previewkeydown);
			this->Controls->Add(this->label_launchdialog);
			this->Controls->Add(this->button_launchdialog);
			this->Controls->Add(this->label_pressed);
			this->Name = L"Form1";
			this->Text = L"Test Global Hot Keys";
			this->FormClosed += gcnew System::Windows::Forms::FormClosedEventHandler(this, &Form1::Form1_FormClosed);
			this->PreviewKeyDown += gcnew System::Windows::Forms::PreviewKeyDownEventHandler(this, &Form1::Form1_PreviewKeyDown);
			this->KeyDown += gcnew System::Windows::Forms::KeyEventHandler(this, &Form1::Form1_KeyDown);
			this->Load += gcnew System::EventHandler(this, &Form1::Form1_Load);
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	protected: virtual bool ProcessCmdKey(Message% msg, Keys keyData) override;  

	private: System::Void Form1_FormClosed(System::Object^  sender, System::Windows::Forms::FormClosedEventArgs^  e);
	private: System::Void Form1_Load(System::Object^  sender, System::EventArgs^  e);
	
     // Constant value was found in the "windows.h" header file.
      static const Int32 WM_ACTIVATEAPP = 0x001C;
      static const Int32 WM_HOTKEYS = 0x312C;

	virtual void WndProc(Message* m) override;

	private: short m_hotkeyID;

	private: System::Void Form1_KeyDown(System::Object^  sender, System::Windows::Forms::KeyEventArgs^  e);
	private: System::Void button_launchdialog_Click(System::Object^  sender, System::EventArgs^  e);
	private: System::Void Form1_PreviewKeyDown(System::Object^  sender, System::Windows::Forms::PreviewKeyDownEventArgs^  e);
};
}

