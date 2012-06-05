#pragma once

#define WIN32_LEAN_AND_MEAN	
#include <tchar.h>

#ifndef MRU_MANAGER_HPP
#include "MRUManager.hpp"
#endif
#ifndef MUI_PACKAGE_HPP
#include "ToolUIManaged/mui/muiPackage.hpp"
#endif
#ifndef TMA_SYSTEM_HPP
#include "ToolUIManaged/tma/tmaSystem.hpp"
#endif

#ifdef _MANAGED

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;


namespace SimpleViewer
{
	/// <summary> 
	/// Summary for MainForm
	///
	/// WARNING: If you change the name of this class, you will need to change the 
	///          'Resource File Name' property for the managed resource compiler tool 
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class MainForm : public System::Windows::Forms::Form
	{
	public: 
		MainForm(void)
		{
			InitializeComponent();

			//	initialize the managed stuff
			//
			tmaSystem::g_pMainForm = this;
			muiPackage::Initialize();

			// MRU File list
			m_pMRUManager = gcnew MRUManager(this->recentFilesToolStripMenuItem, 5 );
		}
        
		System::Windows::Forms::Panel ^  GetRenderWindow()
		{
			return panelRender;
		}

		System::Windows::Forms::Panel ^  GetInitRenderWindow()
		{
			return panelInit;
		}

	protected: 
		~MainForm()
		{
			muiPackage::DeInitialize();

			if (components)
			{
				delete components;
			}
		}

	private: MRUManager^ m_pMRUManager;				// MRU list manager

	private: System::Windows::Forms::Panel ^  panelInit;


	private: System::Windows::Forms::Panel ^  panelRender;
	private: System::Timers::Timer ^  timer1;








	private: System::Windows::Forms::MenuStrip^  menuStrip1;
	private: System::Windows::Forms::ToolStripMenuItem^  fileToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  newToolStripMenuItem;
	private: System::Windows::Forms::ToolStripMenuItem^  openToolStripMenuItem;
	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator1;
	private: System::Windows::Forms::ToolStripMenuItem^  recentFilesToolStripMenuItem;

	private: System::Windows::Forms::ToolStripSeparator^  toolStripSeparator2;
	private: System::Windows::Forms::ToolStripMenuItem^  exitToolStripMenuItem;



	private: System::ComponentModel::IContainer ^  components;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>


		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->panelInit = (gcnew System::Windows::Forms::Panel());
			this->panelRender = (gcnew System::Windows::Forms::Panel());
			this->menuStrip1 = (gcnew System::Windows::Forms::MenuStrip());
			this->fileToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->newToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->openToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->recentFilesToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->toolStripSeparator2 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->exitToolStripMenuItem = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->timer1 = (gcnew System::Timers::Timer());
			this->panelRender->SuspendLayout();
			this->menuStrip1->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timer1))->BeginInit();
			this->SuspendLayout();
			// 
			// panelInit
			// 
			this->panelInit->BackColor = System::Drawing::SystemColors::ActiveCaption;
			this->panelInit->Location = System::Drawing::Point(7, 7);
			this->panelInit->Name = L"panelInit";
			this->panelInit->Size = System::Drawing::Size(26, 29);
			this->panelInit->TabIndex = 14;
			this->panelInit->Visible = false;
			// 
			// panelRender
			// 
			this->panelRender->Anchor = static_cast<System::Windows::Forms::AnchorStyles>((((System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Bottom) 
				| System::Windows::Forms::AnchorStyles::Left) 
				| System::Windows::Forms::AnchorStyles::Right));
			this->panelRender->BackColor = System::Drawing::Color::Black;
			this->panelRender->Controls->Add(this->menuStrip1);
			this->panelRender->Location = System::Drawing::Point(7, 7);
			this->panelRender->Name = L"panelRender";
			this->panelRender->Size = System::Drawing::Size(533, 416);
			this->panelRender->TabIndex = 15;
			this->panelRender->MouseLeave += gcnew System::EventHandler(this, &MainForm::panelRender_MouseLeave);
			this->panelRender->Resize += gcnew System::EventHandler(this, &MainForm::panelRender_Resize);
			this->panelRender->MouseEnter += gcnew System::EventHandler(this, &MainForm::panelRender_MouseEnter);
			// 
			// menuStrip1
			// 
			this->menuStrip1->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(1) {this->fileToolStripMenuItem});
			this->menuStrip1->Location = System::Drawing::Point(0, 0);
			this->menuStrip1->Name = L"menuStrip1";
			this->menuStrip1->Size = System::Drawing::Size(533, 24);
			this->menuStrip1->TabIndex = 0;
			this->menuStrip1->Text = L"menuStrip1";
			// 
			// fileToolStripMenuItem
			// 
			this->fileToolStripMenuItem->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(6) {this->newToolStripMenuItem, 
				this->openToolStripMenuItem, this->toolStripSeparator1, this->recentFilesToolStripMenuItem, this->toolStripSeparator2, this->exitToolStripMenuItem});
			this->fileToolStripMenuItem->Name = L"fileToolStripMenuItem";
			this->fileToolStripMenuItem->Size = System::Drawing::Size(35, 20);
			this->fileToolStripMenuItem->Text = L"File";
			// 
			// newToolStripMenuItem
			// 
			this->newToolStripMenuItem->Name = L"newToolStripMenuItem";
			this->newToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->newToolStripMenuItem->Text = L"New";
			this->newToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::menuFileNew_Click);
			// 
			// openToolStripMenuItem
			// 
			this->openToolStripMenuItem->Name = L"openToolStripMenuItem";
			this->openToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->openToolStripMenuItem->Text = L"Open";
			this->openToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::menuFileOpen_Click);
			// 
			// toolStripSeparator1
			// 
			this->toolStripSeparator1->Name = L"toolStripSeparator1";
			this->toolStripSeparator1->Size = System::Drawing::Size(149, 6);
			// 
			// recentFilesToolStripMenuItem
			// 
			this->recentFilesToolStripMenuItem->Name = L"recentFilesToolStripMenuItem";
			this->recentFilesToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->recentFilesToolStripMenuItem->Text = L"Recent Files";
			// 
			// toolStripSeparator2
			// 
			this->toolStripSeparator2->Name = L"toolStripSeparator2";
			this->toolStripSeparator2->Size = System::Drawing::Size(149, 6);
			// 
			// exitToolStripMenuItem
			// 
			this->exitToolStripMenuItem->Name = L"exitToolStripMenuItem";
			this->exitToolStripMenuItem->Size = System::Drawing::Size(152, 22);
			this->exitToolStripMenuItem->Text = L"Exit";
			this->exitToolStripMenuItem->Click += gcnew System::EventHandler(this, &MainForm::menuFileExit_Click);
			// 
			// timer1
			// 
			this->timer1->AutoReset = false;
			this->timer1->Enabled = true;
			this->timer1->Interval = 20;
			this->timer1->SynchronizingObject = this;
			this->timer1->Elapsed += gcnew System::Timers::ElapsedEventHandler(this, &MainForm::OnTimer);
			// 
			// MainForm
			// 
			this->AutoScaleBaseSize = System::Drawing::Size(5, 13);
			this->ClientSize = System::Drawing::Size(546, 434);
			this->Controls->Add(this->panelRender);
			this->Controls->Add(this->panelInit);
			this->MainMenuStrip = this->menuStrip1;
			this->Name = L"MainForm";
			this->Text = L"XLT Model Inspector";
			this->panelRender->ResumeLayout(false);
			this->panelRender->PerformLayout();
			this->menuStrip1->ResumeLayout(false);
			this->menuStrip1->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->timer1))->EndInit();
			this->ResumeLayout(false);

		}		
		//

	private: System::Void OnTimer(System::Object ^  sender, System::Timers::ElapsedEventArgs ^  e);
	private: System::Void menuFileNew_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileOpen_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void menuFileExit_Click(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_Resize(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseEnter(System::Object ^  sender, System::EventArgs ^  e);
	private: System::Void panelRender_MouseLeave(System::Object ^  sender, System::EventArgs ^  e);

};
}
#endif // _MANAGED

