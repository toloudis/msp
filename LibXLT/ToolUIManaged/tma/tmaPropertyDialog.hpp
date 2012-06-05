#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaPropertyDialog.hpp
//**
//**      A modal dialog designed to hold property controls.
//**
//**	StudioGPU
//**	Copyright(C) 2008 - All Rights Reserved
//\****************************************************************************/
//#ifdef TMA_PROPERTYDIALOG_HPP
//#error tmatmaPropertyDialog.hpp multiply included
//#endif
//#define TMA_PROPERTYDIALOG_HPP
//
//#ifndef PRTY_FORMCONTROLBUILDER_HPP
//#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
//#endif 
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//using namespace System;
//using namespace System::ComponentModel;
//using namespace System::Collections;
//using namespace System::Windows::Forms;
//using namespace System::Data;
//using namespace System::Drawing;
//
//
//
////============================================================================
//// tmaPropertyDialog
////============================================================================
//public ref class tmaPropertyDialog : public System::Windows::Forms::Form
//{
//public:
//	tmaPropertyDialog(const char*  i_Title,
//					  const PropertyUIIList& i_List,
//					  const char* i_Message)
//	{
//		InitializeComponent();
//
//		if (i_Title != NULL)
//			this->Text =  gcnew System::String(i_Title);
//
//		if (i_Message != NULL)
//			label_Message->Text = gcnew System::String(i_Message);
//
//		prtyFormControlBuilder::BuildForm(panel_Properties, i_List);
//	}
//
//protected:
//	/// <summary>
//	/// Clean up any resources being used.
//	/// </summary>
//	~tmaPropertyDialog()
//	{
//		prtyFormControlBuilder::ClearForm(panel_Properties);
//
//		if (components)
//		{
//			delete components;
//		}
//	}
//private: System::Windows::Forms::Label^  label_Message;
//private: System::Windows::Forms::Panel^  panel_Properties;
//private: System::Windows::Forms::Button^  button_OK;
//private: System::Windows::Forms::Button^  button_Cancel;
//protected: 
//
//protected: 
//
//private:
//	/// <summary>
//	/// Required designer variable.
//	/// </summary>
//	System::ComponentModel::Container ^components;
//
//#pragma region Windows Form Designer generated code
//	/// <summary>
//	/// Required method for Designer support - do not modify
//	/// the contents of this method with the code editor.
//	/// </summary>
//	void InitializeComponent(void)
//	{
//		this->label_Message = (gcnew System::Windows::Forms::Label());
//		this->panel_Properties = (gcnew System::Windows::Forms::Panel());
//		this->button_OK = (gcnew System::Windows::Forms::Button());
//		this->button_Cancel = (gcnew System::Windows::Forms::Button());
//		this->SuspendLayout();
//		// 
//		// label_Message
//		// 
//		this->label_Message->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left) 
//			| System::Windows::Forms::AnchorStyles::Right));
//		this->label_Message->AutoSize = true;
//		this->label_Message->Location = System::Drawing::Point(7, 7);
//		this->label_Message->Name = L"label_Message";
//		this->label_Message->Size = System::Drawing::Size(50, 13);
//		this->label_Message->TabIndex = 0;
//		// 
//		// panel_Properties
//		// 
//		this->panel_Properties->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
//			| System::Windows::Forms::AnchorStyles::Left) 
//			| System::Windows::Forms::AnchorStyles::Right));
//		this->panel_Properties->AutoScroll = true;
//		this->panel_Properties->Location = System::Drawing::Point(7, 28);
//		this->panel_Properties->Name = L"panel_Properties";
//		this->panel_Properties->Size = System::Drawing::Size(283, 194);
//		this->panel_Properties->TabIndex = 1;
//		// 
//		// button_OK
//		// 
//		this->button_OK->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
//		this->button_OK->DialogResult = System::Windows::Forms::DialogResult::OK;
//		this->button_OK->Location = System::Drawing::Point(62, 231);
//		this->button_OK->Name = L"button_OK";
//		this->button_OK->Size = System::Drawing::Size(75, 27);
//		this->button_OK->TabIndex = 2;
//		this->button_OK->Text = L"OK";
//		this->button_OK->UseVisualStyleBackColor = true;
//		// 
//		// button_Cancel
//		// 
//		this->button_Cancel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((System::Windows::Forms::AnchorStyles::Bottom | System::Windows::Forms::AnchorStyles::Left));
//		this->button_Cancel->DialogResult = System::Windows::Forms::DialogResult::Cancel;
//		this->button_Cancel->Location = System::Drawing::Point(155, 231);
//		this->button_Cancel->Name = L"button_Cancel";
//		this->button_Cancel->Size = System::Drawing::Size(75, 27);
//		this->button_Cancel->TabIndex = 3;
//		this->button_Cancel->Text = L"Cancel";
//		this->button_Cancel->UseVisualStyleBackColor = true;
//		// 
//		// tmaPropertyDialog
//		// 
//		this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
//		this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
//		this->ClientSize = System::Drawing::Size(292, 266);
//		this->Controls->Add(this->button_Cancel);
//		this->Controls->Add(this->button_OK);
//		this->Controls->Add(this->panel_Properties);
//		this->Controls->Add(this->label_Message);
//		this->Name = L"tmaPropertyDialog";
//		this->Load += gcnew System::EventHandler(this, &tmaPropertyDialog::tmaPropertyDialog_Load);
//		this->ResumeLayout(false);
//		this->PerformLayout();
//
//	}
//#pragma endregion
//private: System::Void tmaPropertyDialog_Load(System::Object^  sender, System::EventArgs^  e) {
//		 }
//};
//
//#endif // _MANAGED
