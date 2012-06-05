#error THIS_FILE_IS_OBSOLETE

///*****************************************************************************
//**  tmaSplashMessage.hpp
//**
//**      A splash box that holds one message.
//**
//**	StudioGPU
//**	Copyright(C) 2004 - All Rights Reserved
//\****************************************************************************/
////#pragma once
//
//#ifdef TMA_SPLASHMESSAGE_HPP
//#error tmaSplashMessage.hpp multiply included
//#endif
//#define TMA_SPLASHMESSAGE_HPP
//
//#ifndef APP_APPLICATION_HPP
//#include "Core/app/appApplication.hpp"
//#endif
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//using namespace System;
//using namespace System::Data;
//using namespace System::Windows::Forms;
//
//
////============================================================================
////============================================================================
//public ref class tmaSplashMessage : public System::Windows::Forms::Form
//{
//	public:
//		tmaSplashMessage( System::String^ i_Message, System::String^ i_ImageLocation )
//		{
//			m_pImageLocation = i_ImageLocation;
//
//			InitializeComponent();
//
//			//	set the text box text
//			//SetMessageText( i_Message );
//		}
//
//	protected:
//		~tmaSplashMessage()
//		{
//			if (components)
//			{
//				delete components;
//			}
//		}
//
//	public: System::Windows::Forms::PictureBox ^  pictureBox_splash;
//	public: System::Windows::Forms::TextBox ^  textBox_status;
//
//	private:
//		/// <summary>
//		/// Required designer variable.
//		/// </summary>
//		System::ComponentModel::Container^ components;
//
//		/// <summary>
//		/// Required method for Designer support - do not modify
//		/// the contents of this method with the code editor.
//		/// </summary>
//		void InitializeComponent(void)
//		{
//			System::Resources::ResourceManager ^  resources = gcnew System::Resources::ResourceManager(tmaSplashMessage::typeid);
//			
//			this->pictureBox_splash = gcnew System::Windows::Forms::PictureBox();
//			this->textBox_status = gcnew System::Windows::Forms::TextBox();
//
//			this->SuspendLayout();
//
//			//
//			// pictureBox_splash
//			//
//			this->pictureBox_splash->BackColor = System::Drawing::Color::White;
//			this->pictureBox_splash->Location = System::Drawing::Point(6, 6);
//			this->pictureBox_splash->Name = "pictureBox_splash";
//			this->pictureBox_splash->Size = System::Drawing::Size( 256, 128 );
//			this->pictureBox_splash->TabIndex = 0;
//			this->pictureBox_splash->TabStop = false;
//			this->pictureBox_splash->Visible = true;
//			//this->pictureBox_splash = true;
//			pictureBox_splash_Configure();
//
//			//
//			// textBox_status
//			//
//			this->textBox_status->AutoSize = false;
//			this->textBox_status->BackColor = System::Drawing::Color::AliceBlue;
//			this->textBox_status->BorderStyle = System::Windows::Forms::BorderStyle::None;
//			this->textBox_status->ForeColor = System::Drawing::Color::Black;
//			this->textBox_status->Location = System::Drawing::Point(6, 142);
//			this->textBox_status->MaxLength = 256;
//			this->textBox_status->Name = "textBox_status";
//			this->textBox_status->ReadOnly = true;
//			this->textBox_status->Size = System::Drawing::Size(256, 80);
//			this->textBox_status->TabIndex = 1;
//			this->textBox_status->TabStop = false;
//			this->textBox_status->Text = "message";
//			//
//			// SplashForm
//			//
//			this->AutoScale = false;
//			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
//			this->ClientSize = System::Drawing::Size(268, 228);
//			this->ControlBox = false;
//			this->Controls->Add(this->textBox_status);
//			this->Controls->Add(this->pictureBox_splash);
//			this->FormBorderStyle = System::Windows::Forms::FormBorderStyle::None;
//			this->MaximizeBox = false;
//			this->MinimizeBox = false;
//			this->Name = "SplashForm";
//			this->ShowInTaskbar = false;
//			this->StartPosition = System::Windows::Forms::FormStartPosition::CenterScreen;
//			this->Text = "Studio GPU";
//			this->TopMost = true;
//			this->ResumeLayout(false);
//		}
//
//private:
//	System::String^ m_pImageLocation;
//
//	//
//	System::Void pictureBox_splash_Configure()
//	{
//		if ( m_pImageLocation != nullptr )
//		{
//			this->pictureBox_splash->Image = System::Drawing::Image::FromFile( m_pImageLocation );
//		}
//	}
//
////
////	The user interface for this form
////
//public:
//	//---------------------------------------------------------------------------
//	// SetImageFile() - set the path + filename of the image
//	//---------------------------------------------------------------------------
//	System::Void SetImageFile( System::String^ i_File )
//	{
//		m_pImageLocation = i_File;
//
//		pictureBox_splash_Configure();
//	}
//
//	//---------------------------------------------------------------------------
//	// SetMessageText() - set/update the text for this form
//	//---------------------------------------------------------------------------
//	System::Void SetMessageText( System::String^ i_Message )
//	{
//		this->textBox_status->Text = i_Message;
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	System::Void Show()
//	{
//		System::Windows::Forms::Form::Show();
//
//		appApplication::DialogLoopTasks();
//	}
//
//	//---------------------------------------------------------------------------
//	//---------------------------------------------------------------------------
//	System::Void Hide()
//	{
//		System::Windows::Forms::Form::Hide();
//	}
//};
//
//#endif // _MANAGED
